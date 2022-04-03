
#undef pr_fmt 
#define pr_fmt(fmt) "%s : " fmt "\n", __func__

/* Device Permission  */
#define RO    0x01
#define WO    0x10
#define RW    0x11


/* HEADERs Section  */
loff_t pcd_lseek (struct file *fp, loff_t off, int whence);
ssize_t pcd_read (struct file *fp, char __user *user_buf, size_t count, loff_t *f_pos);
ssize_t pcd_write (struct file *fp, const char __user *buf, size_t count, loff_t *f_pos);
int pcd_open (struct inode *pinode, struct file *fp);
int pcd_release (struct inode *pinode, struct file *fp);
int check_perm(struct file *fp);


int pcd_pdriver_probe(struct platform_device *pcdev);
int pcd_pdriver_remove(struct platform_device *pcdev);
void pcd_pdevice_release(struct device *dev);



struct platform_device_data{
    int perm;
    int size;
    const char *str;      //Serial Number 
};
