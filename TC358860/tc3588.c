
#include "tc3588.h"

#define TC3588_I2C_NAME    "tc358860XBG"

#define DEBUG    1
#if DEBUG
#define DBG(x...)   printk(x)
#else
#define DBG(x...)   do {} while (0)
#endif


static struct of_device_id tc3588_of_match[] = {
	{.compatible = "rockchip,tc3588"},
	{}
};
MODULE_DEVICE_TABLE(of, tc3588_of_match);

static const struct i2c_device_id tc3588_id[] = {
	{TC3588_I2C_NAME, 0}, 
	{}
};

MODULE_DEVICE_TABLE(i2c, tc3588_id);

static int tc3588_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	printk("TC358860XBG I2C device start to probe!\n");
	
	tc3588_configure(client);
	return 0;
}

static s32 tc3588_i2c_read(struct i2c_client *client, s32 len, u8 *buf)
{
	struct i2c_msg msgs[2];
    s32 ret = -1;
    s32 retries = 0;

    //printk("<<-GTP-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);

    msgs[0].flags = !I2C_M_RD;
    msgs[0].addr  = client->addr;
    msgs[0].len   = TC3588_ADDR_LENGTH;   //表示寄存器地址字节,以byte为单位
    msgs[0].buf   = &buf[0];
    //msgs[0].scl_rate = 300 * 1000;    // for Rockchip, etc.
    
    msgs[1].flags = I2C_M_RD;
    msgs[1].addr  = client->addr;
    msgs[1].len   = len - TC3588_ADDR_LENGTH;
    msgs[1].buf   = &buf[TC3588_ADDR_LENGTH];
    //msgs[1].scl_rate = 300 * 1000;

    while(retries < 5)
    {
        ret = i2c_transfer(client->adapter, msgs, 2);
        if(ret == 2)
			break;
        retries++;
        msleep(5);
    }
    if((retries >= 5))
    {
		printk("I2C Write: 0x%04X, %d bytes failed, errcode: %d! Process reset.", (((u16)(buf[0] << 8)) | buf[1]), len-2, ret);
	}
	
	return ret;
}

static s32 tc3588_i2c_write(struct i2c_client *client, s32 len, u8 *buf)
{
	struct i2c_msg msg;
	s32 ret = -1;
    s32 retries = 0;
    
    msg.flags = !I2C_M_RD;
    msg.addr  = client->addr;
    msg.len   = len;
    msg.buf   = buf;
    
    while(retries < 5)
    {
        ret = i2c_transfer(client->adapter, &msg, 1);
        if (ret == 1)
			break;
        retries++;
    }
    if((retries >= 5))
    {
		printk("I2C Write: 0x%04X, %d bytes failed, errcode: %d! Process reset.", (((u16)(buf[0] << 8)) | buf[1]), len-2, ret);
	}
	
	return ret;
}

static int tc3588_reg_value(u8 *buf, s32 len)
{

	switch(len) {
	case 1:
		printk("read addr: %02x%02x  value:%02x\n", buf[0], buf[1], buf[2]);
		break;
	case 2:
	printk("read addr: %02x%02x  value:%02x%02x\n", buf[0], buf[1], buf[3], buf[2]);
	break;
	case 3:
		printk("read addr: %02x%02x  value:%02x%02x%02x\n", buf[0], buf[1], buf[4], buf[3], buf[2]);
		break;
	case 4:
		printk("read addr: %02x%02x  value:%02x%02x%02x%02x\n", buf[0], buf[1], buf[5], buf[4], buf[3], buf[2]);
		break;
	default:
		break;
	}
	
	return 0;
}

static int tc3588_write_value(u8 *buf, s32 len)
{
	switch(len) {
	case 1:
		printk("write addr: %02x%02x  value:%02x\n", buf[0], buf[1], buf[2]);
		break;
	case 2:
	printk("write addr: %02x%02x  value:%02x%02x\n", buf[0], buf[1], buf[3], buf[2]);
	break;
	case 3:
		printk("write addr: %02x%02x  value:%02x%02x%02x\n", buf[0], buf[1], buf[4], buf[3], buf[2]);
		break;
	case 4:
		printk("write addr: %02x%02x  value:%02x%02x%02x%02x\n", buf[0], buf[1], buf[5], buf[4], buf[3], buf[2]);
		break;
	default:
		break;
	}
	
	return 0;
}

//static int simple_strtoul_test(void)
//{
	//unsigned char *p = "0x101213141101";
	//unsigned long a;
	
	//a = simple_strtoul((char *)p, NULL, 16);
	//printk("%s value: %lx\n", __func__, a);
	
	//return 0;
//}

static int tc3588_configure(struct i2c_client *client)
{
	//初始化tc3588
	int i;
	int data_length;
	unsigned long return_data;
	unsigned long temp_data;
	int ret = -1;
	struct i2c_client *i2c_connecter = client;
	u8 *buf = NULL;
	//u8 *p_data = kmalloc(sizeof(unsigned long), GFP_KERNEL);
	unsigned long temp_addr = 0;
	int list_length = sizeof(tc3588_init_cmd) / sizeof(unsigned long) / 4;
	
	//simple_strtoul_test();
	
	/** 开始进行初始化动作 */
	DBG("%s start in this part =====\n", __func__);
	DBG("tc3588_init_cmd length: %d\n", list_length);
	for(i = 0 ; i < list_length; i++)
	{
		switch(tc3588_init_cmd[i][2])  //检测命令类型
		{
			//case IIC_CMD_CHECK:
			case IIC_CMD_DELAY:
				if(tc3588_init_cmd[i][0] > DELAY_S)
					msleep(tc3588_init_cmd[i][0] / 1000);
				else
					udelay(tc3588_init_cmd[i][0]);
				DBG("delay or sleep cmd: %lu us\n", tc3588_init_cmd[i][0]);
				break;
			case IIC_CMD_WRITE:
				tc3588_i2c_write(i2c_connecter, tc3588_init_cmd[i][1], (u8 *)(&tc3588_init_cmd[i][0]));
				//DBG("write cmd: %lx\n", tc3588_init_cmd[i][0]);
				tc3588_write_value((u8 *)(&tc3588_init_cmd[i][0]), tc3588_init_cmd[i][1] - 2);
				break;
			case IIC_CMD_READ:
				temp_addr = tc3588_init_cmd[i][0];
				data_length = tc3588_init_cmd[i][1];
				return_data = tc3588_init_cmd[i][3];
				//while(1)
				//{
					buf = (u8 *)(&temp_addr);
					ret = tc3588_i2c_read(i2c_connecter, data_length, buf);
					if(ret != 2)
					{
						printk("tc3588_i2c_read falied =====\n");
						return ret;
					}
					if(DEBUG)
						tc3588_reg_value(buf, data_length - 2);
					temp_data = simple_strtoul((char*)buf, NULL, 16);
					//DBG("return value: %012lx\n", temp_data);
					//if(temp_data == return_data)
					//{
						//DBG("check value successfully");
						//break;
					//}
				//}
				break;
			default:
				break;
		}
	}
	
	return 0;

}



static int tc3588_remove(struct i2c_client *client)
{
	printk("TC358860XBG I2C device remove!\n");
	return 0;
}

static struct i2c_driver tc3588_driver = {
	.driver = {
		.name = TC3588_I2C_NAME,
		.owner = THIS_MODULE,
		.of_match_table = tc3588_of_match,
	},
	.probe = tc3588_probe,
	.remove = tc3588_remove,
	.id_table = tc3588_id,
};

module_i2c_driver(tc3588_driver);

MODULE_DESCRIPTION("TC358860 Driver");
MODULE_LICENSE("GPL");



