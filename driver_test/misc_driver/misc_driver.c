#include <linux/module.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/miscdevice.h>
#include <linux/ioctl.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#define DEMO_MISC_GPIO_CTL_NAME   "demo_misc_ctl"
#define DRV_AUTHOR                "sn03955@rigol"
#define DRV_DESC                  "gpio used by spi2k7"


static int demo_misc_open(struct inode * inode, struct file * file)
{
	printk("open file successfully\n");
	return 0;
}
 
static int demo_misc_close(struct inode * inode, struct file * file)
{
	return 0;
}

static long demo_misc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{

	printk("into demo_misc_ioctl\n");
	switch(cmd)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			printk("into case %u and arg=%lu\n", cmd, arg);
			break;
		default:
			printk("unsupport case %u and arg=%lu\n", cmd, arg);
			return -EINVAL;
	}
	
	return 0;
}

static struct file_operations demo_misc_fops = {
	.owner          = THIS_MODULE,
	.open           = demo_misc_open, 
	.release        = demo_misc_close,
	.unlocked_ioctl = demo_misc_ioctl, 
	.compat_ioctl   = demo_misc_ioctl, 
};

static struct miscdevice demo_misc_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = DEMO_MISC_GPIO_CTL_NAME,
	.fops  = &demo_misc_fops,
};

static int __init demo_misc_ctl_init(void)
{
	int ret;
	ret = misc_register(&demo_misc_dev);
	if (ret < 0)
	{
		printk("register demo_misc_gpio device failed!\n");
		return -1;
	}
	printk("demo_misc_gpio register successfully\n");
	return 0;
}

static void __exit demo_misc_ctl_exit(void)
{
	misc_deregister(&demo_misc_dev);
	printk("demo_misc_gpio exit successfully\n");
}

module_init(demo_misc_ctl_init);
module_exit(demo_misc_ctl_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR(DRV_AUTHOR);
MODULE_DESCRIPTION(DRV_DESC);

