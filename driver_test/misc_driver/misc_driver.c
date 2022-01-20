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

#define DEMO_MISC_MAGIC       'x' 
#define IOCTL_INITB           _IO(DEMO_MISC_MAGIC, 0x01)
#define IOCTL_PROGRAMB        _IO(DEMO_MISC_MAGIC, 0x02)
#define IOCTL_DONE	          _IO(DEMO_MISC_MAGIC, 0x03)



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
		case IOCTL_INITB:
		case IOCTL_DONE:
			printk("into case %u and arg=%lu\n", cmd, arg);
			break;
		case IOCTL_PROGRAMB:
			gpio_set_value(2, arg);
			break;
		default:
			printk("unsupport case %u and arg=%lu\n", cmd, arg);
			return -EINVAL;
	}
	
	return 0;
}

static int spi2k7_config(void)
{
	int ret;
	if(gpio_is_valid(1)
	{
		ret = gpio_request(1, "initb");
		if(ret)
		{
			printk("request gpio:%d\n", 1);
			goto initb_err;
		}
		gpio_direction_input(1);
	}
	printk("finish request gpio PIN_INITB\n");
	if(gpio_is_valid(2)
	{
		ret = gpio_request(2, "programb");
		if(ret)
		{
			printk("request gpio:%d\n", 2);
			goto programb_err;
		}
		gpio_direction_output(2, 1);
	}
	printk("finish request gpio PIN_PROGRAMB\n");
	if(gpio_is_valid(3)
	{
		ret = gpio_request(3, "done");
		if(ret)
		{
			printk("request gpio:%d\n", 3);
			goto done_err;
		}
		gpio_direction_input(3);
	}
	printk("finish request gpio PIN_DONE\n");
	return 0;
	
done_err:
	gpio_free(3);
programb_err:
	gpio_free(2);
initb_err:
	gpio_free(1);
	
	return ret;
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
	spi2k7_config();
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

