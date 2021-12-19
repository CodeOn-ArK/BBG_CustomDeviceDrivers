
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

#undef pr_fmt 
#define pr_fmt(fmt) "%s : " fmt, __func__
/* Psuedo Character Device's memory */
#define DEV_MEM     512

/* psuedo character device  */
char psuedo_char_device[DEV_MEM];

dev_t device_number;    //variable to hold device number supplied by the kernel dynamically
struct cdev pcd_cdev;   //cdev variable to hold pcd info


/* HEADERs Section  */
loff_t pcd_llseek (struct file *fp, loff_t off, int whence);
ssize_t pcd_read (struct file *fp, char __user *user_buf, size_t count, loff_t *f_pos);
ssize_t pcd_write (struct file *fp, const char __user *buf, size_t count, loff_t *f_pos);
int pcd_open (struct inode *pinode, struct file *fp);
int pcd_release (struct inode *pinode, struct file *fp);



loff_t pcd_llseek (struct file *fp, loff_t off, int whence)
{
  pr_info("lseek requested\n");
  return 0;
}
ssize_t pcd_read (struct file *fp, char __user *user_buf, size_t count, loff_t *f_pos)
{
  pr_info("read requested for %zu bytes\n", count);
  return 0;
}
ssize_t pcd_write (struct file *fp, const char __user *buf, size_t count, loff_t *f_pos)
{
  pr_info("write requested for %zu bytes\n",count);
  return 0;
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
  //file operations variable to hold the systemcalls implemented for the module
  .owner  = THIS_MODULE,
  .llseek = pcd_llseek,
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
  //1.  Dynamically allocate a device number 
  alloc_chrdev_region(&device_number, 0, 1, "psuedo character device");

  pr_info("Device number <major>:<minor> = %d:%d\n", MAJOR(device_number), MINOR(device_number));

  //2.  Init the cdev structure with fops
  cdev_init(&pcd_cdev, &pcd_fops);

  //3.  Register the pcd's cdev structure with the kernel's VFS
  pcd_cdev.owner = THIS_MODULE;
  cdev_add(&pcd_cdev, device_number, 1);

  //4.  Create a device class under /sys/class
  class_pcd =  class_create(THIS_MODULE, "pcd_class");

  //5.  Populate the sysfs with device information
  device_pcd = device_create(class_pcd, NULL, device_number, NULL, "pcd");

  pr_info("Module Init was successful\n" );

  return 0;
}

static void __exit pcd_exit(void)
{
/* Drivers exit point  */
  device_destroy(class_pcd, device_number);
  class_destroy(class_pcd);
  cdev_del(&pcd_cdev);
  unregister_chrdev_region(device_number, 1);

  pr_info("Module Unloaded\n");

}

/*
 * REGISTRATION SECTION
 */
module_init(pcd_init);     //Used to tell the build system about the init funx
module_exit(pcd_exit);     //Used to tell the build system about the exit funx

/*
 * MODULE DESCRIPTION
 */
MODULE_LICENSE("GPL");            //This provides information about license; beware to use proprietary licenses
MODULE_AUTHOR("ARK");
MODULE_DESCRIPTION("A small kernel module to inteface a psuedo char device");
MODULE_INFO(board, "BeagleBone Green v1");
