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

#define GPIO_A_1    0
#define GPIO_A_2    1
#define GPIO_A_3    2

#define PIN_INITB 		GPIO_A_1
#define PIN_PROGRAMB 	GPIO_A_2
#define PIN_DONE	 	GPIO_A_3

#define ACTIVE_HIGH     1
#define ACTIVE_LOW      0

#define SPI2K7_GPIO_CTL_NAME   "spi2k7_ctl"
#define DRV_AUTHOR             "sn03955@rigol"
#define DRV_DESC               "gpio used by spi2k7"

typedef struct spi2k7_gpio 
{
	unsigned int initb;
	unsigned int programb;
	unsigned int done;
}spi2k7_t;

spi2k7_t spi2k7 = {
	.initb    = PIN_INITB,
	.programb = PIN_PROGRAMB, 
	.done     = PIN_DONE,
};

static unsigned char *read_data;

static int spi2k7_open(struct inode * inode, struct file * file)
{
	printk("open file successfully\n");
	return 0;
}
 
static int spi2k7_close(struct inode * inode, struct file * file)
{
	return 0;
}

static ssize_t spi2k7_write(struct file *file, const char __user *buf, size_t len, loff_t *f_pos)
{
	int retval;
	if(copy_from_user(read_data, buf, len))
	{
		printk("exit this part\n");
		retval = -EFAULT;
		goto exit;
	}
	printk("copy from user: %s\n", read_data);
	
	return 0;

exit:
	return retval;
}

static ssize_t spi2k7_read(struct file *file, char __user *buf, size_t len, loff_t *f_pos)
{
	int retval = 0;
	char *p = DRV_AUTHOR;
	if(copy_to_user(buf, p, len))
	{
		retval = -EFAULT;
		printk("copy failed\n");
		goto exit;
	}
	printk("transfer %s to user\n", DRV_AUTHOR);
	
	return retval;
	
exit:
	return len;
}

static long spi2k7_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret;
	printk("into spi2k7_ioctl\n");
	switch(cmd)
	{
		case PIN_INITB:
			if(arg < 2)
			{
				printk("into set PIN_INITB\n");
				gpio_set_value(cmd, arg);
			}
			else
			{
				printk("invalid value\n");
				return -EINVAL;
			}
			break;
		case PIN_PROGRAMB:
			if(arg < 2)
			{
				printk("into set PIN_PROGRAMB\n");
				gpio_set_value(cmd, arg);
			}
			else
			{
				printk("invalid value\n");
				return -EINVAL;
			}
			break;
		case PIN_DONE:
			if(arg < 2)
			{
				ret = gpio_get_value(cmd);
				printk("read PIN_DONE value:%d\n", ret);
			}
			else
			{
				printk("invalid value\n");
				return -EINVAL;
			}
			break;
		default:
			printk("invalid gpio set\n");
			return -EINVAL;
	}
	
	return 0;
}

static int spi2k7_config(spi2k7_t *pSpi2k7)
{
	int ret;
	if(gpio_is_valid(pSpi2k7->initb))
	{
		ret = gpio_request(pSpi2k7->initb, "initb");
		if(ret)
		{
			printk("request gpio:%d\n", pSpi2k7->initb);
			goto initb_err;
		}
		gpio_direction_input(pSpi2k7->initb);
	}
	printk("finish request gpio PIN_INITB\n");
	if(gpio_is_valid(pSpi2k7->programb))
	{
		ret = gpio_request(pSpi2k7->programb, "programb");
		if(ret)
		{
			printk("request gpio:%d\n", pSpi2k7->programb);
			goto programb_err;
		}
		gpio_direction_output(pSpi2k7->programb, ACTIVE_HIGH);
	}
	printk("finish request gpio PIN_PROGRAMB\n");
	if(gpio_is_valid(pSpi2k7->done))
	{
		ret = gpio_request(pSpi2k7->done, "done");
		if(ret)
		{
			printk("request gpio:%d\n", pSpi2k7->done);
			goto done_err;
		}
		gpio_direction_input(pSpi2k7->done);
	}
	printk("finish request gpio PIN_DONE\n");
	return 0;
	
done_err:
	gpio_free(pSpi2k7->done);
programb_err:
	gpio_free(pSpi2k7->programb);
initb_err:
	gpio_free(pSpi2k7->initb);
	
	return ret;
}

static struct file_operations spi2k7_fops = {
	.owner          = THIS_MODULE,
	.open           = spi2k7_open,
	.release        = spi2k7_close,
	.read           = spi2k7_read, 
	.write          = spi2k7_write,
	.unlocked_ioctl = spi2k7_ioctl, 
	.compat_ioctl   = spi2k7_ioctl, 
};

static struct miscdevice spi2k7_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = SPI2K7_GPIO_CTL_NAME,
	.fops  = &spi2k7_fops,
};

static int __init spi2k7_ctl_init(void)
{
	int ret;
	ret = misc_register(&spi2k7_dev);
	if (ret < 0)
	{
		printk("register spi2k7_gpio device failed!\n");
		return -1;
	}
	read_data = (unsigned char*)devm_kzalloc(spi2k7_dev.this_device, 256, GFP_KERNEL);
	ret = spi2k7_config(&spi2k7);
	if(ret)
	{
		printk("config spi2k7_gpio failed!\n");
		return ret;
	}
	printk("spi2k7_gpio register successfully\n");
	return 0;
}

static void __exit spi2k7_ctl_exit(void)
{
	gpio_free(spi2k7.initb);
	gpio_free(spi2k7.programb);
	gpio_free(spi2k7.done);
	misc_deregister(&spi2k7_dev);
	printk("spi2k7_gpio exit successfully\n");
}

module_init(spi2k7_ctl_init);
module_exit(spi2k7_ctl_exit);



MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRV_AUTHOR);
MODULE_DESCRIPTION(DRV_DESC);


