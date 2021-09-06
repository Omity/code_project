#include <linux/device.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/ctype.h>


/*******************************************************************************
 * 文件名称: hello_world.c
 * 文件作者: ShuJie
 * 创建时间: 2021年09月02日
 * 文件描述: 一个简单的hello world驱动，当insmod时，dmesg输出"hello world";
 *           当rmmod时，dmesg输出"goodbye world";
 *           提供一个sysfs接口，把用户输入的小写字母转换为大写字母;
 *           提供一个sysfs接口，启动一个workqueue任务.
 ******************************************************************************/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shujie");
MODULE_ALIAS("alias hello");
MODULE_VERSION("1.0");


#define IS_EMPTY    "empty"
#define IS_INVALID  "invalid"


void helloRelease(struct kobject *kobject);
static ssize_t helloShow(struct kobject *kobject, struct attribute *attr, char *buf);
static ssize_t helloStore(struct kobject *kobject, struct attribute *attr, const char *buf, size_t count);


// 设备模型的数据结构
/************************************kobject***********************************/

static DEVICE_ATTR(kobject_device, 0666, helloShow, helloStore);         /* 建立设备节点 */

/* 数组，属性添加 */
static struct attribute *defAttr[] =
{
    /* #define DEVICE_ATTR(_name, _mode, _show, _store) /
     * struct device_attribute dev_attr_##_name = __ATTR(_name, _mode, _show, _store)
     */
    &dev_attr_kobject_device.attr,   //DEVICE_ATTR 函数创建的前缀就是dev_attr_ + name 
    NULL,
};

static struct attrbute_group attr_group = 
{
    .attrs = defAttr,
}

/* kobject release */
void helloRelease(struct kobject *kobject)
{
    printk("release this kobject!\n");
}


/* sysfs_ops-> show */
static ssize_t helloShow(struct kobject *kobject, struct attribute *attr , char *buf)
{
    printk("A show operation start\n");
    printk("read from attr->name :%s\n", attr->name);
	return sprintf(buf, "%s\n", attr->name);
	
}

/* sysfs_ops->store */
static ssize_t helloStore(struct kobject *kobject, struct attribute *attr, const char *buf, size_t count)
{
	char* p = NULL;
	printk("A store operation start\n");
	
	sscanf(buf, "%s", p);
    if(NULL == p)
	{
		printk("helloStore success!\n");
		attr->name = IS_EMPTY;
		//sprintf(attr->name, "%s\n", EMPTY);
	}
	else if((*p < 'A') || (*p > 'z'))
	{
		printk("helloStore failed!\n");
		attr->name = IS_INVALID;
		//sprintf(attr->name, "%s\n", "EOF");
	}
	else
	{
		attr->name = "right";
	}
	 p = NULL;
	printk("write: %s\n", attr->name);
	
	return count;
}
/***********************************kobject************************************/

static struct class *thisClass;
static struct device *thisDevice;

/* 初始化函数 */
static int helloWorldInit(void)
{
    void* ptr_err;
	int ret;
    printk("driver loading ........\n");
	thisClass = class_create(THIS_MODULE, "hello_class");
    ret = IS_ERR(ptr_err = thisClass);
    
    thisDevice = device_create(thisClass, NULL, MKDEV(0, 0), NULL, "hello_device");
    ret = IS_ERR(ptr_err = thisDevice);

    ret = sys_create_group(&thisDevice->kobj, &attr_group);
    printk(KERN_INFO "hello world!\n");
    return ret;
}

/* 卸载函数 */
static void helloWorldExit(void)
{
    printk("driver unloading ......\n");
	sys_remove_group(&thisDevice->kobj, &attr_group);
    device_destroy(thisClass, MKDEV(0, 0));
    class_destroy(thisClass);
    printk(KERN_INFO "goodbye world!\n");
}

module_init(helloWorldInit);
module_exit(helloWorldExit);
