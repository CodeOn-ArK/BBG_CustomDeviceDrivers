
   /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*\
   *                                                                                       *
   | Copyright (C) 2021 CodeOn-ArK@github                                                  |
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

/* Psuedo Character Device's memory */
#define DEV_MEM_SIZE     512

/* psuedo character device  */
char pcd_buf[DEV_MEM_SIZE];

dev_t device_number;    /* variable to hold device number supplied by the kernel dynamically*/
struct cdev pcd_cdev;   /* cdev variable to hold pcd info */


/* HEADERs Section  */
loff_t pcd_lseek (struct file *fp, loff_t off, int whence);
ssize_t pcd_read (struct file *fp, char __user *user_buf, size_t count, loff_t *f_pos);
ssize_t pcd_write (struct file *fp, const char __user *buf, size_t count, loff_t *f_pos);
int pcd_open (struct inode *pinode, struct file *fp);
int pcd_release (struct inode *pinode, struct file *fp);



loff_t pcd_lseek (struct file *fp, loff_t offset, int whence)
{
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
}

ssize_t pcd_read (struct file *fp, char __user *user_buf, size_t count, loff_t *f_pos)
{
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
}                          

ssize_t pcd_write (struct file *fp, const char __user *buf, size_t count, loff_t *f_pos)
{                           
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
}

int pcd_open (struct inode *pinode, struct file *fp)
{
  pr_info("open was successful\n");
  return 0;
}

int pcd_release (struct inode *pinode, struct file *fp)
{
  pr_info("close was successful\n");
  return 0;
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
/* Drivers entry point  */
  /* 1.  Dynamically allocate a device number */
  alloc_chrdev_region(&device_number, 0, 1, "psuedo character device");

  pr_info("Device number <major>:<minor> = %d:%d\n", MAJOR(device_number), MINOR(device_number));

  /* 2.  Init the cdev structure with the file operation variable */
  cdev_init(&pcd_cdev, &pcd_fops);

  /* 3.  Register the pcd's cdev structure with the kernel's VFS */
  pcd_cdev.owner = THIS_MODULE;       /* This tells that this module is the owner of the associated cdev struct*/
  cdev_add(&pcd_cdev, device_number, 1);

  /* 4.  Create a device class under /sys/class */
  class_pcd =  class_create(THIS_MODULE, "pcd_class");

  /* 5.  Populate the sysfs with device information */
  device_pcd = device_create(class_pcd, NULL, device_number, NULL, "pcd");

  pr_info("Module Init was successful\n" );

  return 0;
}

static void __exit pcd_exit(void)
{
/* Drivers exit point 
 * To deinitialize follow the exact reverse order as for initialization
 */
  device_destroy(class_pcd, device_number);
  class_destroy(class_pcd);
  cdev_del(&pcd_cdev);
  unregister_chrdev_region(device_number, 1);

  pr_info("Module Unloaded\n");

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
MODULE_DESCRIPTION("A small kernel module to inteface a psuedo char device");
MODULE_INFO(board, "BeagleBone Green v1");
