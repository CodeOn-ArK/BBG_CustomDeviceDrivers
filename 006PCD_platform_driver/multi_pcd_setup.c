
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
#include <linux/platform_device.h>
#include "platform.h"

#define RDO       0
#define WRO       1
#define RDWR      2

#define DEVICE_NAME  "psuedo-character-device"

/* 1. Create Platform data for the devices 
 * (This one is created by us to store data related to the platform devices)
 */
struct platform_device_data pcdev_pdata[2] = {
    [0]  = {
      .perm = RDWR,
      .str  = "Platform-PCDEV-0",
      .size = 512
      
    },
    [1]  = {
      .perm = RDWR,
      .str  = "Platform-PCDEV-1",
      .size = 512
      
    }
};


/* 2. Create Platform devices 
 *    This is used by the VFS to register with the platform bus
 */
struct platform_device pcdev[2] = {
    [0] = {
      .name = DEVICE_NAME,
      .id   = 0,
      .dev = {
        .platform_data = &pcdev_pdata[0],
        .release = pcd_pdevice_release
      }
    },
    [1] = {
      .name = DEVICE_NAME,
      .id   = 1,
      .dev = {
        .platform_data = &pcdev_pdata[1],
        .release = pcd_pdevice_release
      }
    }
};

void pcd_pdevice_release(struct device *dev){
    pr_info("Platform device released");

}

static int __init pcd_pdevice_init(void){
  //1. First register your device when you load the module
  //Unless this is done, your driver cannot interact witht the device
      platform_device_register(&pcdev[0]);
      platform_device_register(&pcdev[1]);
      pr_info("Platform Device registered");
      return 0;

}


static void __exit pcd_pdevice_exit(void){
  //1. Now you can unregister your device
      platform_device_unregister(&pcdev[0]);
      platform_device_unregister(&pcdev[1]);
      pr_info("Platform Device removed");

}


/*
 * REGISTRATION SECTION
 */
module_init(pcd_pdevice_init);     /* Used to tell the build system about the init funx*/
module_exit(pcd_pdevice_exit);     /* Used to tell the build system about the exit funx*/

/*
 * MODULE DESCRIPTION
 */
MODULE_LICENSE("GPL");            /* This provides information about license; beware to use proprietary licenses*/
MODULE_AUTHOR("ARK");
MODULE_DESCRIPTION("A small kernel module to inteface multiple psuedo character platform devices");
MODULE_INFO(board, "BeagleBone Green v1");
