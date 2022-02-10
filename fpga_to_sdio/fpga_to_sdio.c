#include <linux/module.h>
#include <linux/dma-mapping.h>
#include <linux/clk.h>
#include <linux/mmc/host.h>
#include <linux/platform_device.h>
#include <linux/cpufreq.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/io.h>

#include <mach/dma.h>

#include "fpga_to_sdio.h"

#define DEVICE_NAME   "fpga_to_sdio"

/**
 * 这是模仿host的主机端
 * 目前代码不完整,缺失很大,并且流程不明确
 */


struct fpga_host {
	struct platform_device *pdev;
	struct fpga_to_sdio_pdata *pdata;
	struct mmc_host		*mmc;
	struct resource		*mem;
	struct clk		*clk;         //当前的总线时钟
	u8 pwr;                       //当前电压
	
	void __iomem   *base;
	
	
	spinlock_t lock;
	int        irq;         //中断  
	int        irq_cd;
	
	int        dma;
	int        do_dma;
	int        dma_togo;
	int        dma_complete;
	
	
	struct mmc_request    *mrq;             //当前request
	struct mmc_command    *cmd;             //当前命令
	
	int 	   isfpga;
	
	struct tasklet_struct	pio_tasklet;
};

static int fpga_remove(struct platform_device *pdev)
{
	return 0;
}

static int fpga_probe(struct platform_device *pdev)
{
	struct mmc_host *mmc;
	struct fpga_host *host;
	int ret;
	int isfpga;
	
	isfpga = platform_get_device_id(pdev)->driver_data;
	
	mmc = mmc_alloc_host(sizeof(struct fpga_host), &pdev->dev);   //分配mmc_host结构体内存
	if(!mmc) {
		ret = -ENOMEM;
		goto out;
	}
	
	host = mmc_priv(mmc); //将mmc_host和fpga_host关联
	host->mmc = mmc;
	host->pdev = pdev;
	host->isfpga = isfpga;
	
	spin_lock_init(&host->lock);
	
	return 0;
	
out:
	return ret;
}

static void fpga_shutdown(struct platform_device *pdev)
{
	
}


#ifdef CONFIG_PM

static int fpga_suspend(struct device *dev)
{
	struct mmc_host *mmc = platform_get_drvdata(to_platform_device(dev));

	return mmc_suspend_host(mmc);
}

static int fpga_resume(struct device *dev)
{
	struct mmc_host *mmc = platform_get_drvdata(to_platform_device(dev));

	return mmc_resume_host(mmc);
}

static const struct dev_pm_ops fpga_pm = {
	.suspend	= fpga_suspend,
	.resume		= fpga_resume,
};

#define fpga_pm_ops &fpga_pm
#else /* CONFIG_PM */
#define fpga_pm_ops NULL
#endif /* CONFIG_PM */



static struct platform_device_id fpga_driver_ids[] = {
	.name = "fpga_to_sdio",     //匹配字段
	.driver_data = 1,
	{},
}

MODULE_DEVICE_TABLE(platform, fpga_driver_ids);

static struct platform_driver fpga_driver = {
	.driver = {
		.name  = "fpga_to_sdio",
		.owner = THIS_MODULE,
		.pm    = fpga_pm_ops,
	}
	.id_table = fpga_driver_ids,
	.probe    = fpga_probe,
	.remove   = fpga_remove,
	.shutdown = fpga_shutdown,
};

module_platform_driver(fpga_driver);

MODULE_DESCRIPTION("FPGA to SD card Interface driver");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("shujie@rigol.com");
