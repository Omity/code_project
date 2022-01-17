/**
 * 创建关于字符驱动最基础的创建方式
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/version.h>


#define DEMO_NAME          "demo_cdev_driver"

#define DEBUG
#ifdef DEBUG
#define DBG(fmt, arg, ...)   printk(fmt, ##arg)
#else
#define DBG(fmt, arg, ...)   do{}while(0)
#endif

static struct cdev *demo_dev;
static dev_t demo_dev_no;

static ssize_t demo_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	return 0;
}

static ssize_t demo_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	return 0;
}

static struct file_operations demo_fops = {
	.owner = THIS_MODULE,
	.read = demo_read,
	.write = demo_write,
};

static int __init demo_init(void)
{
	int ret;
	
	printk("%s initialize\n", DEMO_NAME);
	
	demo_dev = cdev_alloc();   //申请cdev内存
	if(!demo_dev) {
		ret = -ENOMEM;
		goto alloc_err;
	}
	
	demo_dev->ops = &demo_fops; //指定file_opreations结构体，因为用alloc，所以可直接使用，不用init
	
	ret = alloc_chrdev_region(&demo_dev_no, 0, 1, DEMO_NAME);  //动态申请设备号
	if(!ret) {
		printk("failed to request minor\n");
		goto region_err;
	}
	
	ret = cdev_add(demo_dev, demo_dev_no, 1);    //注册设备
	if(!ret)
	{
		printk("falied to add cdev\n");
		goto add_err;
	}

	printk("succeed to add cdev\n");
	return 0;
	
add_err:
	cdev_del(demo_dev);
region_err:
	unregister_chrdev_region(demo_dev_no, 1);
alloc_err:
	cdev_del(demo_dev);
	
	return ret;
	
}


static void __exit demo_exit(void)
{
	cdev_del(demo_dev);
	unregister_chrdev_region(demo_dev->dev, 1);
	kfree(demo_dev);
	
	printk("%s exit successfully\n", DEMO_NAME);
}


module_init(demo_init);
module_exit(demo_exit);

MODULE_LICENSE("GPL v2");
