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

/*
 * CODE SECTION
 */
static int __init helloworld_init(void)
{
}

static void __exit helloworld_exit(void)
{
}


/*
 * REGISTRATION SECTION
 */
module_init(helloworld_init);     //Used to tell the build system about the init funx
module_exit(helloworld_exit);     //Used to tell the build system about the exit funx

/*
 * MODULE DESCRIPTION
 */
MODULE_LICENSE("GPL");            //This provides information about license; beware to use proprietary licenses
MODULE_AUTHOR("ARK");
MODULE_DESCRIPTION("A small kernel module to print hello world");
MODULE_INFO(board, "BeagleBone Green v1");
