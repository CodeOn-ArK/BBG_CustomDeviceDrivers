
   /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*\
   *                                                                                       *
   | Copyright (C) 2022 CodeOn-ArK@github                                                  |
   *                                                                                       *
   | This program is free software: you can redistribute it and/or modify                  |
   * it under the terms of the GNU General Public License as published by                  *
   | the Free Software Foundation, either version 3 of the License, or                     |
   * (at your option) any later version.                                                   *
   |                                                                                       |
   * This program is distributed in the hope that it will be useful,                       *
   | but WITHOUT ANY WARRANTY; without even the implied warranty of                        |
   * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                         *
   | GNU General Public License for more details.                                          |
   *                                                                                       *
   | You should have received a copy of the GNU General Public License                     |
   * along with this program.  If not, see <https://www.gnu.org/licenses/>.                *
   |                                                                                       |
   \*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/*
 * HEADER SECTION
 * Linux kernel headers are located separetly inside linux folder in the kernel source tree
 */
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>

#undef pr_fmt 
#define pr_fmt(fmt) "%s : " fmt, __func__

#define TOTAL_DEVICES           4

/* Psuedo Character Device's memory */
#define DEV_MEM_SIZE_PCDEV1     512
#define DEV_MEM_SIZE_PCDEV2     512
#define DEV_MEM_SIZE_PCDEV3     512
#define DEV_MEM_SIZE_PCDEV4     512

/* psuedo character device  */
char pcd_buf_dev1[DEV_MEM_SIZE_PCDEV1];
char pcd_buf_dev2[DEV_MEM_SIZE_PCDEV2];
char pcd_buf_dev3[DEV_MEM_SIZE_PCDEV3];
char pcd_buf_dev4[DEV_MEM_SIZE_PCDEV4];

/* This struct contains devices specific data  */
struct  pcdev_private_data{
  int           perm;     //permission
  char          *buf;
  unsigned      size;
  const char    *serial_num;
  struct cdev   cdev;
};

/* This struct stores driver private data  */
struct  pcdrv_private_data{
  int total_devices;
  dev_t dev_number;    /* variable to hold device number supplied by the kernel dynamically*/
  struct  class *class_pcd;
  struct  device  *device_pcd;
  struct  pcdev_private_data      pcdev_data[TOTAL_DEVICES];
};


struct pcdrv_private_data     driver_private_data = {
  .total_devices  = TOTAL_DEVICES,
  .pcdev_data = {
    [0] ={
      .buf = pcd_buf_dev1,
      .size = DEV_MEM_SIZE_PCDEV1,
      .serial_num = "PCD1",
      .perm = 0x01  /* RDONLY */
    },

    [1] ={
      .buf = pcd_buf_dev2,
      .size = DEV_MEM_SIZE_PCDEV2,
      .serial_num = "PCD2",
      .perm = 0x10  /* WRONLY */
    },

    [2] ={
      .buf = pcd_buf_dev3,
      .size = DEV_MEM_SIZE_PCDEV3,
      .serial_num = "PCD3",
      .perm = 0x11  /* RDWR */
    },

    [3] ={
      .buf = pcd_buf_dev4,
      .size = DEV_MEM_SIZE_PCDEV4,
      .serial_num = "PCD4",
      .perm = 0x11  /* RDWR */
    },

  }
};


/* HEADERs Section  */
loff_t pcd_lseek (struct file *fp, loff_t off, int whence);
ssize_t pcd_read (struct file *fp, char __user *user_buf, size_t count, loff_t *f_pos);
ssize_t pcd_write (struct file *fp, const char __user *buf, size_t count, loff_t *f_pos);
int pcd_open (struct inode *pinode, struct file *fp);
int pcd_release (struct inode *pinode, struct file *fp);
int check_perm(void);



loff_t pcd_lseek (struct file *fp, loff_t offset, int whence)
{
  loff_t temp;
  struct pcdev_private_data *curr_pcdev = (struct pcdev_private_data *) fp->private_data;
  int dev_mem_size = curr_pcdev->size;
  pr_info("lseek requested\n");
  pr_info("current file position = %lld\n", fp->f_pos); 

  switch(whence){
    case SEEK_SET : 
      if((offset > dev_mem_size) || (offset < 0))
        return -EINVAL;
      fp->f_pos = offset;
      break;

    case SEEK_CUR :
      temp = fp->f_pos + offset;
      if((temp > dev_mem_size) || temp < 0)
        return -EINVAL;
      fp->f_pos = temp;
      break;

    case SEEK_END :
      temp = fp->f_pos + dev_mem_size;
      if((temp > dev_mem_size) || temp < 0)
        return -EINVAL;
      fp->f_pos = temp;
      break;
      
    default:
      return -EINVAL;
  }

  pr_info("updated file position = %lld\n", fp->f_pos);
  return fp->f_pos;
}

ssize_t pcd_read (struct file *fp, char __user *user_buf, size_t count, loff_t *f_pos)
{
  struct pcdev_private_data *curr_pcdev = (struct pcdev_private_data *) fp->private_data;
  pr_info("read requested for %zu bytes\n", count);
  pr_info("current file position = %lld\n", *f_pos);

  /* Adjust the count  */
  if(*f_pos + count > curr_pcdev->size){
    count = curr_pcdev->size - *f_pos;   
  }                        

  /* Copy to user  */
  if (copy_to_user(user_buf, &curr_pcdev->buf[*f_pos], count)){
    return -EFAULT; 
  }

  /* Update the current file position  */
  *f_pos += count;

  pr_info("bytes successfuly read = %zu bytes\n", count);
  pr_info("updated file position = %lld\n", *f_pos);

  return count;                 
}                          

ssize_t pcd_write (struct file *fp, const char __user *buf, size_t count, loff_t *f_pos)
{                           
  struct pcdev_private_data *curr_pcdev = (struct pcdev_private_data *) fp->private_data;
  pr_info("write requested  for %zu bytes\n",count);
  pr_info("current file position = %lld\n", *f_pos);

  /* Check the count value  */
  if(count + *f_pos > curr_pcdev->size){
    count = curr_pcdev->size - *f_pos; 
  }

  if(!count){
    pr_err("Error No memory left\n");
    return -ENOMEM; 
  }

  /* Copy count number of bytes from user to the local buffer   */
  if(copy_from_user(&curr_pcdev->buf[*f_pos], buf, count)){
    return -EFAULT;
  }

  /* Update the current file position  */
  *f_pos += count;
  
  pr_info("bytes successfuly written = %zu bytes\n", count);
  pr_info("updated file position = %lld\n", *f_pos);

  /* Return the number of bytes succesfully written */
  return count; 
}

int check_perm(void){

  return 0;
}

int pcd_open (struct inode *pinode, struct file *fp)
{
  int ret, minor_num;
  struct pcdev_private_data *curr_pcdev;

  minor_num = MINOR(pinode->i_rdev);
  pr_info("device number -> %d", minor_num);

 /* Get the struct containing the current device number, and assign it to curr_pcdev ptr  */ 
  curr_pcdev = container_of(pinode->i_cdev, struct pcdev_private_data, cdev);

 /* Since we need this data(private data of the current opened device) 
  * in various places, we store it in file structures' private_data field
  * For use by other methods.
  */ 
  fp->private_data = curr_pcdev;

  ret = check_perm();
  if(!ret){
    pr_info("open was successful\n");
    return 0;
  }else{
    pr_err("open was un-successful\n");
    return 1;
  }
}

int pcd_release (struct inode *pinode, struct file *fp)
{
  pr_info("close was successful\n");
  return  0;
}

struct file_operations pcd_fops =  
{
  /* file operations variable to hold the systemcalls implemented for the module */
  .owner  = THIS_MODULE,
  .llseek = pcd_lseek,
  .read   = pcd_read,
  .write  = pcd_write,
  .open   = pcd_open,
  .release= pcd_release

};

/*
struct class *class_pcd;
struct device *device_pcd;
*/

static int __init pcd_init(void)
{
/* Driver's entry point  */
  int ret_code, i;
  /* 1.  Dynamically allocate a device number
   * Here the base device number is stored in driver_private_data.dev_number which corresponds 
   * to pcd1. It will allocate space for 4 devices starting from pcd0
   * subsequent device numbers can be obtained by the following approach :
   *        MAJOR(driver_private_data.dev_number) + MINOR(driver_private_data.dev_number+i)
   *        where i follows 0<= i <= 3
   * */
  ret_code = alloc_chrdev_region(&driver_private_data.dev_number, 0, TOTAL_DEVICES, "pdcevices");
  if(ret_code < 0)
    goto alloc_fail;

  /* 2.  Create a device class under /sys/class ; this has to be created once for any driver, all the 4 
   * devices' dev directory reside inside this class
   */
  driver_private_data.class_pcd =  class_create(THIS_MODULE, "pcd_class");
  if(IS_ERR(driver_private_data.class_pcd)){
    pr_err("class creation failed\n");
    ret_code = PTR_ERR(driver_private_data.class_pcd) ;
    goto dealloc_chrdev;

  }

  for(i=0;  i<TOTAL_DEVICES;  i++){
    /* This needs to be done for every device hence we loop this no of device times  */
    pr_info("Device number <major>:<minor> = %d:%d\n", MAJOR(driver_private_data.dev_number), MINOR(driver_private_data.dev_number+i));

    /* 3.  Init the cdev structure with the file operation variable ; returns nothing*/
    cdev_init(&driver_private_data.pcdev_data[i].cdev, &pcd_fops);

    /* 4.  Register the pcd's cdev structure with the kernel's VFS */
    driver_private_data.pcdev_data[i].cdev.owner = THIS_MODULE;       /* This tells that this module is the owner of the associated cdev struct*/
    ret_code = cdev_add(&driver_private_data.pcdev_data[i].cdev, driver_private_data.dev_number + i, 1);
    if(ret_code < 0)
      goto cdev_remove;

    /* 5.  Populate the sysfs with device information */
    driver_private_data.device_pcd = device_create(driver_private_data.class_pcd, NULL, driver_private_data.dev_number+i, NULL, "pcd%d",i);
    if(IS_ERR(driver_private_data.device_pcd )){
      pr_err("device creation failed\n");
      ret_code = PTR_ERR(driver_private_data.device_pcd) ;
      goto class_destroy;

    }
  }

  pr_info("Module Init was successful\n" );
  return 0;

cdev_remove:

class_destroy:
  for(;i >= 0; i-- ){
    device_destroy(driver_private_data.class_pcd, driver_private_data.dev_number + i);
    cdev_del(&driver_private_data.pcdev_data[i].cdev);
  }
  class_destroy(driver_private_data.class_pcd);

dealloc_chrdev:
  unregister_chrdev_region(driver_private_data.dev_number, TOTAL_DEVICES);

alloc_fail:
  return ret_code;

}

static void __exit pcd_exit(void)
{
  int i;
  for(i=4;  i>0; i-- ){
    device_destroy(driver_private_data.class_pcd, driver_private_data.dev_number + i-1);
    cdev_del(&driver_private_data.pcdev_data[i-1].cdev);
  }
  class_destroy(driver_private_data.class_pcd);
  unregister_chrdev_region(driver_private_data.dev_number, TOTAL_DEVICES);

  pr_info("Module Removal was successful\n" );

}

/*
 * REGISTRATION SECTION
 */
module_init(pcd_init);     /* Used to tell the build system about the init funx*/
module_exit(pcd_exit);     /* Used to tell the build system about the exit funx*/

/*
 * MODULE DESCRIPTION
 */
MODULE_LICENSE("GPL");            /* This provides information about license; beware to use proprietary licenses*/
MODULE_AUTHOR("ARK");
MODULE_DESCRIPTION("A small kernel module to inteface multiple psuedo char device");
MODULE_INFO(board, "BeagleBone Green v1");
