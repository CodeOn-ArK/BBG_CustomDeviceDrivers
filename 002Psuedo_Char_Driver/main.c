
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

/* Psuedo Character Device's memory */
#define DEV_MEM     512

/* psuedo character device  */
char psuedo_char_device[DEV_MEM];

dev_t device_number;    //variable to hold device number supplied by the kernel dynamically
struct cdev pcd_cdev;   //cdev variable to hold pcd info

/*
 *
struct file_operations {
	struct module *owner;
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
	ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
	int (*iterate) (struct file *, struct dir_context *);
	int (*iterate_shared) (struct file *, struct dir_context *);
	unsigned int (*poll) (struct file *, struct poll_table_struct *);
	long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
	long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
	int (*mmap) (struct file *, struct vm_area_struct *);
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *, fl_owner_t id);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, loff_t, loff_t, int datasync);
	int (*fasync) (int, struct file *, int);
	int (*lock) (struct file *, int, struct file_lock *);
	ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
	unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
	int (*check_flags)(int);
	int (*setfl)(struct file *, unsigned long);
	int (*flock) (struct file *, int, struct file_lock *);
	ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
	ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
	int (*setlease)(struct file *, long, struct file_lock **, void **);
	long (*fallocate)(struct file *file, int mode, loff_t offset,
			  loff_t len);
	void (*show_fdinfo)(struct seq_file *m, struct file *f);
#ifndef CONFIG_MMU
	unsigned (*mmap_capabilities)(struct file *);
#endif
	ssize_t (*copy_file_range)(struct file *, loff_t, struct file *,
			loff_t, size_t, unsigned int);
	int (*clone_file_range)(struct file *, loff_t, struct file *, loff_t,
			u64);
	ssize_t (*dedupe_file_range)(struct file *, u64, u64, struct file *,
			u64);
};
 */

/* HEADERs Section  */
loff_t pcd_llseek (struct file *fp, loff_t off, int whence);
ssize_t pcd_read (struct file *fp, char __user *user_buf, size_t count, loff_t *f_pos);
ssize_t pcd_write (struct file *fp, const char __user *buf, size_t count, loff_t *f_pos);
int pcd_open (struct inode *pinode, struct file *fp);
int pcd_release (struct inode *pinode, struct file *fp);



loff_t pcd_llseek (struct file *fp, loff_t off, int whence)
{
  return 0;
}
ssize_t pcd_read (struct file *fp, char __user *user_buf, size_t count, loff_t *f_pos)
{
  return 0;
}
ssize_t pcd_write (struct file *fp, const char __user *buf, size_t count, loff_t *f_pos)
{
  return 0;
}
int pcd_open (struct inode *pinode, struct file *fp)
{
  return 0;
}
int pcd_release (struct inode *pinode, struct file *fp)
{
  return 0;
}

struct file_operations pcd_fops =  
{
  //file operations variable to hold the systemcalls implemented for the module
  .llseek = pcd_llseek,
  .read   = pcd_read,
  .write  = pcd_write,
  .open   = pcd_open,
  .release= pcd_release
};

static int __init pcd_init(void)
{
/* Drivers entry point  */
  //1.  Dynamically allocate a device number 
  alloc_chrdev_region(&device_number, 0, 1, "psuedo character device");

  //2.  Init the cdev structure with fops
  cdev_init(&pcd_cdev, &pcd_fops);

  //3.  Register the pcd's cdev structure with the kernel's VFS
  pcd_cdev.owner = THIS_MODULE;
  cdev_add(&pcd_cdev, device_number, 1);

  //4.
  return 0;
}

static void __exit pcd_exit(void)
{
/* Drivers exit point  */

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
