   /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
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
   **-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/*
 * HEADER SECTION
 * Linux kernel headers are located separetly inside linux folder in the kernel source tree
 */
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define MINOR_START 2
#define MINOR_COUNT 3

static int module_open(struct inode *inode, struct file *file){
  pr_info("%s called\n", __func__);
  
  return 0;
}

static int module_close(struct inode *inode, struct file *file){
  pr_info("%s called\n", __func__);
  
  return 0;
}

static long module_ioctl(struct file *file, unsigned int cmd, long unsigned arg){
  pr_info("%s called cmd = %d, arg = %ld\n", __func__, cmd, arg);
  
  return 0;
}

const struct file_operations m_fops = {
  .owner= THIS_MODULE,
  .open = module_open,
  .unlocked_ioctl = module_ioctl,
  .release = module_close,
};

struct cdev module_cdev[MINOR_COUNT];
dev_t m_dev;

/*
 * CODE SECTION
 */
static int __init ioctl_test_entry(void){
  int i=0, ret=0 ;
  ret = alloc_chrdev_region(&m_dev, MINOR_START, MINOR_COUNT, "test-device")  ;
  if(ret<0)
    goto EXIT;

  for(i; i<MINOR_COUNT; i++){
    cdev_init(&module_cdev[i], &m_fops);
    ret = cdev_add(&module_cdev[i], m_dev, 1);
    if(ret < 0)
      goto dealloc_chrdev;

  }

dealloc_chrdev:
  for(; i>0; i--){
   unregister_chrdev_region(m_dev, 1) ;
  }
EXIT:
  return 0;

}

static void __exit ioctl_test_exit(void){
  int i=0;
  for(; i<MINOR_COUNT; i++){
    cdev_del(&module_cdev[i]);

  }
  unregister_chrdev_region(m_dev, MINOR_COUNT);
  
}


/*
 * REGISTRATION SECTION
 */
module_init(ioctl_test_entry);     //Used to tell the build system about the init funx
module_exit(ioctl_test_exit);     //Used to tell the build system about the exit funx

/*
 * MODULE DESCRIPTION
 */
MODULE_LICENSE("GPL");            //This provides information about license; beware to use proprietary licenses
MODULE_AUTHOR("ARK");
MODULE_DESCRIPTION("A small kernel module to print hello world");
MODULE_INFO(board, "BeagleBone Green v1");
