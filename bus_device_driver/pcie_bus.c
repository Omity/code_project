/* 该函数是对总线驱动的一个简单例子 
 * 因为是对PCIe的总线驱动,所以命名以PCIe为主
 */

/** 修改记录                         修改时间                        修改人
 *  本次只是对总线驱动的简单框架    2021年11月2日14时53分20秒       sn03955
 * 搭建,并没有关于PCIe的任何协议
 * 内容
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>


/** 编码步骤
 * 定义总线
 * 总线的注册与注销
 * 匹配函数
 */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sn03955");

//用于device和driver匹配
static int pcie_bus_match(struct device *dev, struct device_driver *drv)
{

	return !strncmp(dev_name(dev), drv->name, strlen(drv->name));
	//网上另一种写法
#if 0
	if(!strncmp(drv->name, dev->kobj.name, strlen(drv->name)))
	{
		printk("match ok\n");
		return 1;
	}
	else
	{
		printk("match failed\n");
		return 0;
	}
	
	return 0;
#endif
}

struct bus_type pcie_bus_type = {
	.name = "pci-express",
	.match = pcie_bus_match,
};

EXPORT_SYMBOL(pcie_bus_type);   // 外部声明, 使其在内核代码里可见,其他地方调用只需声明extern就可以使用.

static int __init pcie_bus_init(void)
{
	int ret;
	printk("---%s---- working ...\n", __FUNCTION__);
	
	ret = bus_register(&pcie_bus_type);
	if(0 != ret)
	{
		printk("pcie regist failed\n");
		return ret;
	}
	
	return 0;
}

static void __exit pcie_bus_exit(void)
{
	printk("---%s--- working...\n", __FUNCTION__);
	
	bus_unregister(&pcie_bus_type);
}

module_init(pcie_bus_init);
module_exit(pcie_bus_exit);




