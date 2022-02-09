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

#define DEVICE_NAME   "fpga_to_sdio"


static int fpga_remove(struct platform_device *pdev)
{
	return 0;
}

static int fpga_probe(struct platform_device *pdev)
{
	return 0;
}

static void fpga_shutdown(struct platform_device *pdev)
{
	
}

static struct platform_device_id fpga_driver_ids[] = {
	.name = "fpga_to_sdio",     //设备数里的匹配字段
	{},
}

static struct platform_driver fpga_driver = {
	.driver = {
		.name  = "fpga_to_sdio",
		.owner = THIS_MODULE,
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
