
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
}

/* This struct stores driver private data  */
struct  pcdrv_private_data{
  int total_devices;
  struct  pcdev_private_data  pcdev_data[TOTAL_DEVICES];
};

dev_t device_number;    /* variable to hold device number supplied by the kernel dynamically*/



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
