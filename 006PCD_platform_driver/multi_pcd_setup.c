
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
#include <linux/slab.h>
#include "platform.h"

#define RDO       0
#define WRO       1
#define RDWR      2

#define DEVICE_NAME  "psuedo-character-device"
static int create_device(struct platform_device_data *plat_dev_data,  struct platform_device *plat_dev, int i);

/* 1. Create Platform data for the devices 
 * (This one is created by us to store data related to the platform devices)
 */
struct platform_device_data pcdev_pdata[TOTAL_DEVICES]; /*= {
    [0]  = {
      .perm = RDWR,
      .str  = "Platform-PCDEV-0",
      .size = 512
      
    },
    [1]  = {
      .perm = RDWR,
      .str  = "Platform-PCDEV-1",
      .size = 512
      
    },
    [2]  = {
      .perm = RDWR,
      .str  = "Platform-PCDEV-2",
      .size = 712 
      
    },
    [3]  = {
      .perm = RDWR,
      .str  = "Platform-PCDEV-3",
      .size = 1024 
      
    },
};
*/

/* 2. Create Platform devices 
 *    This is used by the VFS to register with the platform bus
 */
struct platform_device pcdev[TOTAL_DEVICES]; 

void pcd_pdevice_release(struct device *dev){
    pr_info("Platform device released");

}

static int __init pcd_pdevice_init(void){
    int ret, i=0;
    do{
      ret = create_device(&pcdev_pdata[i], &pcdev[i], i);
      if(ret)
        pr_warn("Can't allocate memory to create device");

    //Register your device when you load the module
    //Unless this is done, your driver cannot interact witht the device
      platform_device_register(&pcdev[i]);
      i++;

    }while(i<TOTAL_DEVICES);

    pr_info("Platform Device registered");
    return 0;

}


static void __exit pcd_pdevice_exit(void){
  //1. Now you can unregister your device
      int i=0;
      do{
        platform_device_unregister(&pcdev[i++]);
        
      }while(i<TOTAL_DEVICES);
      pr_info("Platform Device removed");

}

static int create_device(struct platform_device_data *plat_dev_data,  struct platform_device *plat_dev, int i){
      plat_dev_data->perm = RDWR;
      plat_dev_data->str = (char *)kmalloc(strlen("Platform-PCDEV-0"), GFP_KERNEL);
      if(!plat_dev_data->str)
        return 1;
      snprintf(plat_dev_data->str, strlen("Platform-PCDEV-0") + 1, "Platform-PCDEV-%d", i);
      pr_info("String val -> %s", plat_dev_data->str);
      plat_dev_data->size = 512;

      plat_dev->name = DEVICE_NAME;
      plat_dev->id   = i;
      plat_dev->dev.platform_data = plat_dev_data;
      plat_dev->dev.release = pcd_pdevice_release;

      return 0;
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
MODULE_AUTHOR("ArK");
MODULE_DESCRIPTION("A small kernel module to inteface multiple psuedo character platform devices");
MODULE_INFO(board, "BeagleBone Green v1");
