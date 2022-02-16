/**
 * IIC下的键盘驱动
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/of.h>
#include <linux/of_device.h>

#include "i2c_keyboard.h"


#define DEVICE_NAME         "rigol_kerboard"
#define DEVICE_BASE_ADDR    0x45                 //值需要对应设备
#define DRIVER_NAME         "rigol_kbd_driver"

#define KBD_DEBUG            1

/*
//I2C设备的信息, 必须有设备名和设备号, 其他的如中断,私有数据可选
static struct i2c_board_info rigol_kbd_info[] = {
	I2C_BOARD_INFO(DEVICE_NAME, DEVICE_BASE_ADDR),
};
*/

static int i2c_kbd_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	
	struct device *dev = &client->dev;
	struct input_dev *input;
	
	int error;
	
	KBD_INFO("I2C KEYBOARD driver probe...\n");
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {    //检测i2c适配器
		KBD_ERROR("I2C not supported");
		return -ENODEV;
	}
	
	
	//配置输入设备
	input = devm_input_allocate_device(dev);
	if (!input)
		return -ENOMEM;
	
	input->name = client->name;
	input->id.bustype = BUS_I2C;
	input->id.vendor  = 0x0001;
	input->id.product = 0x001;
	input->id.version = 0x0001;
	
	return 0;
}

static int i2c_kbd_remove(struct i2c_client *client)
{
	return 0;
}


static const struct i2c_device_id rigol_kbd_id[] = {
	{DEVICE_NAME, 0},   //目前固定一个设备
	{},
};

MODULE_DEVICE_TABLE(i2c, rigol_kbd_id);

static const struct of_device_id of_rigol_kbd[] = {          //设备树匹配
	{.compatible = "rigol, kerboard"},
	{},
};

MODULE_DEVICE_TABLE(of, of_rigol_kbd);

static struct i2c_driver rigol_kbd_driver = {
	.driver = {
		.name = DRIVER_NAME, 
		.of_match_table = of_rigol_kbd,
		.owner = THIS_MODULE,
	},
	.probe = i2c_kbd_probe,
	.remove = i2c_kbd_remove,
	.id_table = rigol_kbd_id,
};

module_i2c_driver(&rigol_kbd_driver);


MODULE_AUTHOR("shujie@rigol.com");
MODULE_DESCRIPTION("kerboard driver use i2c");
MODULE_LICENSE("GPL");






