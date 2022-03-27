
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


/* 1. Create Platform data for the devices  */
struct platform_device_data pcdev_pdata[2] = {
    [0]  = {
      .perm = RDWR,
      .str  = "Platform_PCDEV_0",
      .size = 512
      
    },
    [1]  = {
      .perm = RDWR,
      .str  = "Platform_PCDEV_1",
      .size = 512
      
    }
};


/* 2. Create Platform devices  */
struct platform_device pcdev[2] = {
    [0] = {
      .name = "platform_pcdev-0",
      .id   = 0,
      .dev = {
        .platform_data = &pcdev_pdata[0]
      }
    },
    [1] = {
      .name = "platform_pcdev-1",
      .id   = 1,
      .dev = {
        .platform_data = &pcdev_pdata[1]
      }
    }
};


static int __init platform_pcdev_init(void){
      platform_device_register(&pcdev[0]);
      platform_device_register(&pcdev[1]);
      return 0;

}


static void __exit platform_pcdev_exit(void){
      platform_device_register(&pcdev[0]);
      platform_device_register(&pcdev[1]);

}



/*
 * REGISTRATION SECTION
 */
module_init(platform_pcdev_init);     /* Used to tell the build system about the init funx*/
module_exit(platform_pcdev_exit);     /* Used to tell the build system about the exit funx*/

/*
 * MODULE DESCRIPTION
 */
MODULE_LICENSE("GPL");            /* This provides information about license; beware to use proprietary licenses*/
MODULE_AUTHOR("ARK");
MODULE_DESCRIPTION("A small kernel module to inteface multiple psuedo character platform devices");
MODULE_INFO(board, "BeagleBone Green v1");
