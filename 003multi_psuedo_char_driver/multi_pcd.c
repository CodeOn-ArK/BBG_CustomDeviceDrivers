
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
  char  *buf;
  unsigned  size;
  const char  *serial_num;
  int perm;
  struct cdev  cdev;
};

/* This struct stores driver private data  */
struct  pcdrv_private_data{
  int total_devices;
  dev_t dev_number;    /* variable to hold device number supplied by the kernel dynamically*/
  struct  class *class_pcd;
  struct  device  *device_pcd;
  struct  pcdev_private_data  pcdev_data[TOTAL_DEVICES];
};


struct pcdrv_private_data driver_private_data = {
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



loff_t pcd_lseek (struct file *fp, loff_t offset, int whence)
{
#if 0
  loff_t temp;
  pr_info("lseek requested\n");
  pr_info("current file position = %lld\n", fp->f_pos); 

  switch(whence){
    case SEEK_SET : 
      if((offset > DEV_MEM_SIZE) || (offset < 0))
        return -EINVAL;
      fp->f_pos = offset;
      break;

    case SEEK_CUR :
      temp = fp->f_pos + offset;
      if((temp > DEV_MEM_SIZE) || temp < 0)
        return -EINVAL;
      fp->f_pos = temp;
      break;

    case SEEK_END :
      temp = fp->f_pos + DEV_MEM_SIZE;
      if((temp > DEV_MEM_SIZE) || temp < 0)
        return -EINVAL;
      fp->f_pos = temp;
      break;
      
    default:
      return -EINVAL;
  }

  pr_info("updated file position = %lld\n", fp->f_pos);
  return fp->f_pos;
#endif
  return  0;
}

ssize_t pcd_read (struct file *fp, char __user *user_buf, size_t count, loff_t *f_pos)
{
#if 0
  pr_info("read requested for %zu bytes\n", count);
  pr_info("current file position = %lld\n", *f_pos);

  /* Adjust the count  */
  if(*f_pos + count > DEV_MEM_SIZE){
    count = DEV_MEM_SIZE - *f_pos;   
  }                        

  /* Copy to user  */
  if (copy_to_user(user_buf, &pcd_buf[*f_pos], count)){
    return -EFAULT; 
  }

  /* Update the current file position  */
  *f_pos += count;

  pr_info("bytes successfuly read = %zu bytes\n", count);
  pr_info("updated file position = %lld\n", *f_pos);

  return count;                 
#endif
  return  0;
}                          

ssize_t pcd_write (struct file *fp, const char __user *buf, size_t count, loff_t *f_pos)
{                           
#if 0
  pr_info("write requested  for %zu bytes\n",count);
  pr_info("current file position = %lld\n", *f_pos);

  /* Check the count value  */
  if(count + *f_pos > DEV_MEM_SIZE){
    count = DEV_MEM_SIZE - *f_pos; 
  }

  if(!count){
    pr_err("Error No memory left\n");
    return ENOMEM; 
  }

  /* Copy count number of bytes from user to the local buffer   */
  if(copy_from_user(&pcd_buf[*f_pos], buf, count)){
    return -EFAULT;
  }

  /* Update the current file position  */
  *f_pos += count;
  
  pr_info("bytes successfuly written = %zu bytes\n", count);
  pr_info("updated file position = %lld\n", *f_pos);

  /* Return the number of bytes succesfully written */
  return count; 
#endif
  return  0;
}

int pcd_open (struct inode *pinode, struct file *fp)
{
#if 0
  pr_info("open was successful\n");
  return 0;
#endif
  return  0;
}

int pcd_release (struct inode *pinode, struct file *fp)
{
#if 0
  pr_info("close was successful\n");
  return 0;
#endif
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

struct class *class_pcd;
struct device *device_pcd;

static int __init pcd_init(void)
{
#if 0
/* Drivers entry point  */
  int ret_code;
  /* 1.  Dynamically allocate a device number */
  ret_code = alloc_chrdev_region(&device_number, 0, 1, "psuedo character device");
  if(ret_code < 0)
    goto alloc_fail;

  pr_info("Device number <major>:<minor> = %d:%d\n", MAJOR(device_number), MINOR(device_number));

  /* 2.  Init the cdev structure with the file operation variable ; returns nothing*/
  cdev_init(&pcd_cdev, &pcd_fops);

  /* 3.  Register the pcd's cdev structure with the kernel's VFS */
  pcd_cdev.owner = THIS_MODULE;       /* This tells that this module is the owner of the associated cdev struct*/
  ret_code = cdev_add(&pcd_cdev, device_number, 1);
  if(ret_code < 0)
    goto dealloc_chrdev;

  /* 4.  Create a device class under /sys/class */
  class_pcd =  class_create(THIS_MODULE, "pcd_class");
  if(IS_ERR(class_pcd)){
    pr_err("class creation failed\n");
    ret_code = PTR_ERR(class_pcd) ;
    goto cdev_remove;
  }

  /* 5.  Populate the sysfs with device information */
  device_pcd = device_create(class_pcd, NULL, device_number, NULL, "pcd");
  if(IS_ERR(device_pcd)){
    pr_err("device creation failed\n");
    ret_code = PTR_ERR(device_pcd) ;
    goto class_destroy;
  }

  pr_info("Module Init was successful\n" );

  return 0;

class_destroy:
  class_destroy(class_pcd);
cdev_remove:
  cdev_del(&pcd_cdev);
dealloc_chrdev:
  unregister_chrdev_region(device_number, 1);
alloc_fail:
  return ret_code;
#endif
  return  0;
}

static void __exit pcd_exit(void)
{
#if 0
/* Drivers exit point 
 * To deinitialize follow the exact reverse order as for initialization
 */
  device_destroy(class_pcd, device_number);
  class_destroy(class_pcd);
  cdev_del(&pcd_cdev);
  unregister_chrdev_region(device_number, 1);

  pr_info("Module Unloaded\n");
#endif

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
