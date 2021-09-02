#include <linux/module.h>
#include <linux/init.h>



//module install

static int __init helloWorldInit(void)        
{
    printk(KERN_INFO "hello world!\n");
}


static void __exit helloWorldExit(void)
{
    printk(KERN_INFO "goodbye world!\n");
}







module_init(helloWorldInit);          //insmod
module_exit(helloWorldExit);          //rmmod


MODULE_LICENSE("GPL");
MODULE_AUTHOR("ShuJie");
MODULE_DECRIPTION("这是测试驱动!\n");
MODULE_ALIAS("alias test_ko");
