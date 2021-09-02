#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>


static int helloWorldInit(void)
{
    printk(KERN_INFO "hello world!\n");
    return 0;
}

static void helloWorldExit(void)
{
    printk(KERN_INFO "goodbye world!\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shujie");
MODULE_ALIAS("alias hello");


module_init(helloWorldInit);
module_exit(helloWorldExit);
