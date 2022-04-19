
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
#include <linux/slab.h>
#include <linux/platform_device.h>
#include "platform.h"


/* Device Private data structure  */
struct pcdev_private_data{
    struct  platform_device_data pdev_pdata;    //Holds device specific data
    char    *buffer;                            //points to the buffer associated with the device
    dev_t   curr_dev_num;                       //holds current device num
    struct  cdev  cdev;                         //holds current cdev struct
};

/* Driver private data  */
struct pcdriver_private_data{
    int total_devices;
    dev_t device_num_base;
    struct class *class_pcd;
    struct device *device_pcd;
};

/* Platform Driver structure to probe and remove  */
struct platform_driver pcd_pdriver = {
    .probe = pcd_pdriver_probe,
    .remove = pcd_pdriver_remove,
    .driver = {
        .name   =   "psuedo-character-device" 
    }
};

/* Structure to hold file operation  */
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

struct pcdriver_private_data pcdriver_private_data;

int pcd_pdriver_probe(struct platform_device *pcdev){
  int ret;
  struct pcdev_private_data *prv_data;
  struct platform_device_data *pdata; 

  //1.  Fetch the platform data
  pdata = (struct platform_device_data  *)pcdev->dev.platform_data;
  if(!pdata){
    pr_err("No platform data available");
    ret = -EINVAL;
    goto out;

  }

  //2.  Dynamically allocate memory for the device private data
  prv_data = kzalloc(sizeof(*prv_data), GFP_KERNEL);
  if(!prv_data){
    pr_err("Memory not available");
    ret = -ENOMEM;
    goto out;

  }
  dev_set_drvdata(&pcdev->dev, prv_data);
  memcpy(&prv_data->pdev_pdata, pdata, sizeof(*pdata));
  pr_info("Private DATA; Permission = %d",prv_data->pdev_pdata.perm);
  pr_info("Private DATA; String = %s",prv_data->pdev_pdata.str);
  pr_info("Private DATA; Size = %d",prv_data->pdev_pdata.size);

  //3.  Dynamically allocate memory for the device buffer using size information
  //    from the platform data
  prv_data->buffer = kzalloc(prv_data->pdev_pdata.size, GFP_KERNEL);
  if(!prv_data->buffer){
    pr_err("Memory not available");
    ret = -ENOMEM;
    goto free_prv_data;

  }

  //4.  Get the device number
  prv_data->curr_dev_num = pcdriver_private_data.device_num_base + pcdev->id;

  //5.  Do cdev init and cdev add
  cdev_init(&prv_data->cdev, &pcd_fops);
  prv_data->cdev.owner = THIS_MODULE;
  ret = cdev_add(&prv_data->cdev, prv_data->curr_dev_num, 1);
  if(ret < 0){
    cdev_del(&prv_data->cdev);
    goto free_buf;

  }

  //6.  Create device file for the detected platform device
  pcdriver_private_data.device_pcd = device_create(pcdriver_private_data.class_pcd, NULL, prv_data->curr_dev_num, NULL, "pcdev-%d", pcdev->id);
  if(IS_ERR(pcdriver_private_data.device_pcd)){
    pr_err("Device Creation Failed");
    ret = PTR_ERR(pcdriver_private_data.device_pcd );
    goto cdev_del;

  }
  pr_info("Driver Probe Success; Device found");
  return 0;

  //7.  Error Handling
cdev_del:
  cdev_del(&prv_data->cdev);
free_buf:
  kfree(prv_data->buffer);
free_prv_data:
  kfree(prv_data );
out:
  return ret;

}

int pcd_pdriver_remove(struct platform_device *pcdev){
  struct pcdev_private_data *prv_data;
  prv_data = dev_get_drvdata(&pcdev->dev);
  
  /* 1. Remove a device that was created with device_create()  */
  device_destroy(pcdriver_private_data.class_pcd, prv_data->curr_dev_num);

  /* 2. Remove a cdev entry from the system  */
  cdev_del(&prv_data->cdev);

  /* 3. Free the memory held by the device  */  
  kfree(prv_data->buffer);
  kfree(prv_data);

  pr_info("Device Removed ");
  return 0;

}

loff_t pcd_lseek (struct file *fp, loff_t offset, int whence)
{
#if 0
  loff_t temp;
  struct pcdev_private_data *curr_pcdev = (struct pcdev_private_data *) fp->private_data;
  int dev_mem_size = curr_pcdev->size;
  pr_info("lseek requested");
  pr_info("current file position = %lld", fp->f_pos); 

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

  pr_info("updated file position = %lld", fp->f_pos);
  return fp->f_pos;
#endif
  return 0;
}

ssize_t pcd_read (struct file *fp, char __user *user_buf, size_t count, loff_t *f_pos)
{
#if 0
  struct pcdev_private_data *curr_pcdev = (struct pcdev_private_data *) fp->private_data;
  pr_info("read requested for %zu bytes", count);
  pr_info("current file position = %lld", *f_pos);

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

  pr_info("bytes successfuly read = %zu bytes", count);
  pr_info("updated file position = %lld", *f_pos);

  return count;                 
#endif
  return 0;
}                          

ssize_t pcd_write (struct file *fp, const char __user *buf, size_t count, loff_t *f_pos)
{                           
#if 0
  struct pcdev_private_data *curr_pcdev = (struct pcdev_private_data *) fp->private_data;
  pr_info("write requested  for %zu bytes",count);
  pr_info("current file position = %lld", *f_pos);

  /* Check the count value  */
  if(count + *f_pos > curr_pcdev->size){
    count = curr_pcdev->size - *f_pos; 
  }

  if(!count){
    pr_err("Error No memory left");
    return -ENOMEM; 
  }

  /* Copy count number of bytes from user to the local buffer   */
  if(copy_from_user(&curr_pcdev->buf[*f_pos], buf, count)){
    return -EFAULT;
  }

  /* Update the current file position  */
  *f_pos += count;
  
  pr_info("bytes successfuly written = %zu bytes", count);
  pr_info("updated file position = %lld", *f_pos);

  /* Return the number of bytes succesfully written */
  return count; 
#endif
  return 0;
}

int check_perm(struct file *fp){
#if 0
  fmode_t file_mode = fp->f_mode;
  struct pcdev_private_data *curr_pcdev = (struct pcdev_private_data *) fp->private_data;
  pr_info("serial num %s", curr_pcdev->serial_num);

  if(curr_pcdev->perm == 0x11){
    return 0;
  }

  if((file_mode & FMODE_READ) && !(file_mode & FMODE_WRITE)){
  /* RO  */
    if(curr_pcdev->perm == 0x01) 
       return 0;

  }else if((file_mode & FMODE_WRITE) && !(file_mode & FMODE_READ)){
  /* WO  */  
    if(curr_pcdev->perm == 0x10) 
       return 0;

  }else if(file_mode & FMODE_READ){
  /* RO or RW*/

  }else if(file_mode & FMODE_WRITE){
  /* WO or RW  */ 
    
  }
  pr_err("passed all checks");

  return -EPERM;
#endif
  return 0;
}

int pcd_open (struct inode *pinode, struct file *fp)
{
#if 0
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

  ret = check_perm(fp);
  if(!ret){
    pr_info("open was successful");
    return 0;
  }else{
    pr_err("open was un-successful");
    return -EPERM;
  }
#endif
  return 0;
}

int pcd_release (struct inode *pinode, struct file *fp)
{
  pr_info("close was successful");
  return  0;
}


static int __init pcd_pdriver_init(void)
{
    /* Driver's entry point  */
      int ret;

      //1. Dynamically allocate a device number for the number of devices
      ret = alloc_chrdev_region(&pcdriver_private_data.device_num_base, 0, pcdriver_private_data.total_devices, "pcdevices");
      if(ret < 0){
        pr_err("Alloc Chrdev Region failed");
        goto out;

      }

      //2. Create device class 
      pcdriver_private_data.class_pcd = class_create(THIS_MODULE, "pcd_class")   ;
      if(IS_ERR(pcdriver_private_data.class_pcd)){
        pr_err("Class creation failed");
        unregister_chrdev_region(pcdriver_private_data.device_num_base, pcdriver_private_data.total_devices);
        ret = PTR_ERR(pcdriver_private_data.class_pcd);
        return ret;

      }

      //3. Register the platform driver
      platform_driver_register(&pcd_pdriver);
      pr_info("PCD Platform Driver loaded");

out:
      return 0;

}

static void __exit pcd_pdriver_exit(void)
{
      //1. Unregister the platform driver
      platform_driver_unregister(&pcd_pdriver);

      //2. Destroy the class
      class_destroy(pcdriver_private_data.class_pcd);

      //3. Unregister device numbers
      unregister_chrdev_region(pcdriver_private_data.device_num_base, pcdriver_private_data.total_devices);
      pr_info("PCD Platform Driver unloaded");

}

/*
 * REGISTRATION SECTION
 */
module_init(pcd_pdriver_init);     /* Used to tell the build system about the init funx*/
module_exit(pcd_pdriver_exit);     /* Used to tell the build system about the exit funx*/

/*
 * MODULE DESCRIPTION
 */
MODULE_LICENSE("GPL");            /* This provides information about license; beware to use proprietary licenses*/
MODULE_AUTHOR("ARK");
MODULE_DESCRIPTION("A small kernel module to inteface multiple psuedo char device");
MODULE_INFO(board, "BeagleBone Green v1");

