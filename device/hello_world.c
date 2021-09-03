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
static struct kobject  *kob;
//static struct kset     *kst;

/* kobject下的文件和权限*/
struct attribute helloAttr = 
{
    .name = "kobj_config",
    .mode = 0666,
};

struct attribute helloLog = 
{
    .name = "outlog",
    .mode = 0666,
};


/* 数组，代表kobject下可以有多个文件 */
static struct attribute *defAttr[] =
{
    &helloAttr,
    NULL,
};

/* 文件的操作 */
struct sysfs_ops helloOps = 
{
    .show = helloShow,
    .store = helloStore,
};

/* 填充kobject结构体 
 * kobject 主要关注这个元素,也可以使用默认值
 */
struct kobj_type helloKtype =
{
    .release = helloRelease,
    .sysfs_ops = &helloOps,
    .default_attrs = defAttr,
};

/* kobject release */
void helloRelease(struct kobject *kobject)
{
    printk("release this kobject!\n");
}


/* sysfs_ops-> show */
static ssize_t helloShow(struct kobject *kobject, struct attribute *attr , char *buf)
{
    printk("A show operation start\n");
	return sprintf(buf, "%s\n", attr->name);
	
}

/* sysfs_ops->store */
static ssize_t helloStore(struct kobject *kobject, struct attribute *attr, const char *buf, size_t count)
{
	const char* p = buf;
	printk("A store operation start\n");
    if(NULL == buf)
	{
		printk("helloStore success!\n");
		attr->name = IS_EMPTY;
		//sprintf(attr->name, "%s\n", EMPTY);
	}
	else if((*buf < 'A') || (*buf > 'z'))
	{
		printk("helloStore failed!\n");
		attr->name = IS_INVALID;
		//sprintf(attr->name, "%s\n", "EOF");
	}
	else
	{
		attr->name = p;
	}
	
	return count;
}
	

/* 初始化函数 */
static int helloWorldInit(void)
{
	int ret;
    printk("driver loading ........\n");
	
	ret = kobject_init_and_add(kob, &helloKtype, kernel_kobj->parent, "hello_test");
	if(!ret)
	{
		printk("kobject init failed in %d\n", ret);
	}
    printk(KERN_INFO "hello world!\n");
    return 0;
}

/* 卸载函数 */
static void helloWorldExit(void)
{
    printk("driver unloading ......\n");
	kobject_del(kob);
    printk(KERN_INFO "goodbye world!\n");
}

module_init(helloWorldInit);
module_exit(helloWorldExit);
