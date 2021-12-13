/*
    Copyright (C) 2021 CodeOn-ArK@github 

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/*
 * Linux kernel headers are located separetly inside linux folder in the kernel source tree
 */
#include <linux/module.h>

static int __init helloworld_init(void)
{
  /*
   * This is used to initialize the module. __init is a macro which is used to tell
   * the build system that the following funciton is an entry point to the kernel module
   * The function is static because it is dynamic kernel module and this function will
   * not be called by any other program/function from another file
   * The init functions are stored in a separate segment of memory called .init section
   * After the initialization is done the memory occupied by the funx in .init section is 
   * freed.
   * The function must return integer value
   */
  pr_info("Hello World\n"); //Wrapper function around the kernel's printk implementation

  return 0;                 /*The init function always returns some value
                              If it returns 0; the initialization was successful
                              else there was some error
                            */
}

static void __exit helloworld_exit(void)
{
  /*
   * This is cleanup function called after the module has completed execution
   * It returns void
   */
  pr_info("Bye World\n");
}


module_init(helloworld_init);     //Used to tell the build system about the init funx
module_exit(helloworld_exit);     //Used to tell the build system about the exit funx

MODULE_LICENSE("GPL");            //This provides information about license; beware to use proprietary licenses
MODULE_AUTHOR("ARK");
MODULE_DESCRIPTION("A small kernel module to print hello world");
MODULE_INFO(board, "BeagleBone Green v1");
