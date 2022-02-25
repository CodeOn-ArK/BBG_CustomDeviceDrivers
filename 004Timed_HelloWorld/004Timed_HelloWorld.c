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

#include <linux/module.h>
#include <linux/time.h>

static int num = 10;
static struct timeval start_time;

module_param(num, int, S_IRUGO);

static void hello(void){
	int i;
	for(i=0; i<num; i++){
		pr_info("[%d/%d] Hello!\n", i, num);
	}
}

static int __init helloworld_entry(void){
	do_gettimeofday(&start_time);
	hello();

	return 0;
}

static void __exit helloworld_exit(void){
	struct timeval end_time;
	do_gettimeofday(&end_time);

	pr_info("Unloading module after %ld seconds\n", end_time.tv_sec - start_time.tv_sec);
}

module_init(helloworld_entry);
module_exit(helloworld_exit);


/*
 * MODULE DESCRIPTION
 */
MODULE_LICENSE("GPL");            //This provides information about license; beware to use proprietary licenses
MODULE_AUTHOR("ARK");
MODULE_DESCRIPTION("A small kernel module to print hello world");
MODULE_INFO(board, "BeagleBone Green v1");
