
#include "spi.h"
/**
 * GPIObank   基值   GPIO组   基值
    GPIO0      0       A       0
    GPIO1      32      B       8
    GPIO2      64      C       16
    GPIO3      96      D       24
    GPIO4     128
  */


#if 0
#define DBG(x...)   printk(x)
#define DBG_PRINT
#else
#define DBG(x...)   do {} while (0)
#endif


#define DEVICE_NAME            "spi_simulate"
#define DRV_AUTHOR             "sn03955@rigol"
#define DRV_DESC               "use gpio to simulate spi prtocol"


// SPI 主设备结构体
Spi_t spi_master = {
	.u32CS   = CS_0,
    .u32SCLK = SCLK,
    .u32MOSI = MOSI,
    .u32MISO = MISO,
    .spiMode = Mode_0,
    .spiType = SPIMaster,
};

static byte* read_write_data;

/******************************************************************************
**函数名称：GpioSetDirection
**函数功能：设置GPIO方向
**输入参数：gpio——GPIO号
**         label——申请管脚名字
**         dir——管脚方向
**输出参数：无
******************************************************************************/
static int GpioSetDirection(unsigned int gpio, const char *label, int dir)
{
	int ret;
	if (gpio_is_valid(gpio))
	{
		ret = GPIO_REQUEST(gpio, label);
		if (ret)
		{
			printk("request gpio:%u failed!\n", gpio);
			goto err_req;
		}
		if (dir)   //1表示输出
			GPIO_OUTPUT(gpio, GPIO_OUT_LOW);  //默认设置低电平
		else
			GPIO_INPUT(gpio);
	}
	
	return 0;
	
err_req:
	GPIO_FREE(gpio);
	
	return ret;
}


/******************************************************************************
**函数名称：SPISetGpioHigh
**函数功能：设置管脚电平为高
**输入参数：gpio----管脚号
**         
**         
**输出参数：0
******************************************************************************/
static void SPISetGpioHigh(unsigned int gpio)
{
	GPIO_SET_VALUE(gpio, GPIO_OUT_HIGH);
}


/******************************************************************************
**函数名称：SPISetGpioLow
**函数功能：设置管脚电平为低
**输入参数：gpio----管脚号
**         
**         
**输出参数：0
******************************************************************************/
static void SPISetGpioLow(unsigned int gpio)
{
	GPIO_SET_VALUE(gpio, GPIO_OUT_LOW);
}


/******************************************************************************
**函数名称：SPIConfigure
**函数功能：初始化SPI管脚
**输入参数：pstuSpi---spi结构体
**         
**         
**输出参数：无
******************************************************************************/
static int SPIConfigure(struct miscdevice *pdev, Spi_t* pstuSpi)
{
    int ret = 0;
    if(!pstuSpi)
    {
		printk("error in %s parameter!\n", __func__);
		return -EINVAL;
	}

	read_write_data = (byte*)devm_kzalloc(pdev->this_device, SPI_TRANS_DATA_LENGTH, GFP_KERNEL);
	
    if(pstuSpi->spiType == SPIMaster)
    {
		DBG("set type to master\n");
		GpioSetDirection(CS_0, "spi_cs_0", GPIO_DIRECTION_OUTPUT);
		GpioSetDirection(CS_1, "spi_cs_1", GPIO_DIRECTION_OUTPUT);
		GpioSetDirection(CS_2, "spi_cs_2", GPIO_DIRECTION_OUTPUT);
		GpioSetDirection(CS_3, "spi_cs_3", GPIO_DIRECTION_OUTPUT);
		SPISetGpioHigh(CS_0);
		SPISetGpioHigh(CS_1);
		SPISetGpioHigh(CS_2);
		SPISetGpioHigh(CS_3);
		GpioSetDirection(pstuSpi->u32SCLK, "spi_sclk", GPIO_DIRECTION_OUTPUT);
		GpioSetDirection(pstuSpi->u32MOSI, "spi_mosi", GPIO_DIRECTION_OUTPUT);
		GpioSetDirection(pstuSpi->u32MISO, "spi_miso", GPIO_DIRECTION_INPUT);
    }
    else
    {
		GpioSetDirection(CS_0, "spi_cs_0", GPIO_DIRECTION_INPUT);
		GpioSetDirection(CS_1, "spi_cs_1", GPIO_DIRECTION_INPUT);
		GpioSetDirection(CS_2, "spi_cs_2", GPIO_DIRECTION_INPUT);
		GpioSetDirection(CS_3, "spi_cs_3", GPIO_DIRECTION_INPUT);
		GpioSetDirection(pstuSpi->u32SCLK, "spi_sclk", GPIO_DIRECTION_INPUT);
		GpioSetDirection(pstuSpi->u32MOSI, "spi_mosi", GPIO_DIRECTION_INPUT);
		GpioSetDirection(pstuSpi->u32MISO, "spi_miso", GPIO_DIRECTION_OUTPUT);
    }
    
    spin_lock_init(&(pstuSpi->lock));
    ret = pstuSpi->spiMode;
    switch(pstuSpi->spiMode)
    {
    case Mode_0:
		break;
    case Mode_1:
		break;
    case Mode_2:
    case Mode_3:
        SPISetGpioHigh(pstuSpi->u32SCLK);
        break;
    default:
		return ret;
    }
    
    return 0;
}


/******************************************************************************
**函数名称：SPICSIsEnable
**函数功能：CS管脚状态设定
**输入参数：pstuSpi---spi结构体
**          s32isEnable---是否使能
**         
**输出参数：0
******************************************************************************/
static int SPICSIsEnable(Spi_t* pstuSpi, int s32isEnable)
{
	if(s32isEnable)
		SPISetGpioLow(pstuSpi->u32CS);  //片选使能拉低
	else
		SPISetGpioHigh(pstuSpi->u32CS);
		
	return 0;
	
}


/******************************************************************************
**函数名称：SPIWriteByte
**函数功能：写一个byte数据
**输入参数：pstuSpi---spi结构体
**          u8Byte---待发送数据
**         
**输出参数：0
******************************************************************************/
static int SPIWriteByte(Spi_t* pstuSpi, byte u8Byte)
{
    int i = 0;
    if(!pstuSpi)
		return -EINVAL;

    switch(pstuSpi->spiMode)
    {
    case Mode_0: /* Clock Polarity is 0 and Clock Phase is 0 */
		SPISetGpioLow(pstuSpi->u32SCLK);
        for(i = 7; i >= 0; i--)
        {
            if(u8Byte & (1 << i))
            {
                SPISetGpioHigh(pstuSpi->u32MOSI);
            }
            else
            {
                SPISetGpioLow(pstuSpi->u32MOSI);
            }
            SPISetGpioLow(pstuSpi->u32SCLK);
            SPIDelay;
            SPISetGpioHigh(pstuSpi->u32SCLK);
            SPIDelay;
        }
        SPISetGpioLow(pstuSpi->u32SCLK);
        break;

    case Mode_1: /* Clock Polarity is 0 and Clock Phase is 1 */
        SPISetGpioLow(pstuSpi->u32SCLK);
        for(i = 7; i >= 0; i--)
        {
            if(u8Byte & (1 << i))
            {
                SPISetGpioHigh(pstuSpi->u32MOSI);
            }
            else
            {
                SPISetGpioLow(pstuSpi->u32MOSI);
            }
            SPISetGpioHigh(pstuSpi->u32SCLK);
            SPIDelay;
            SPISetGpioLow(pstuSpi->u32SCLK);
            SPIDelay;
        }
        SPISetGpioLow(pstuSpi->u32SCLK);
        break;

    case Mode_2: /* Clock Polarity is 1 and Clock Phase is 0 */
        SPISetGpioHigh(pstuSpi->u32SCLK);
        for(i = 7; i >= 0; i--)
        {
            SPISetGpioHigh(pstuSpi->u32SCLK);
            if(u8Byte & (1 << i))
            {
                SPISetGpioHigh(pstuSpi->u32MOSI);
            }
            else
            {
                SPISetGpioLow(pstuSpi->u32MOSI);
            }
            SPIDelay;
            SPISetGpioLow(pstuSpi->u32SCLK);
            SPIDelay;
        }
        SPISetGpioHigh(pstuSpi->u32SCLK);
        break;

    case Mode_3: /* Clock Polarity is 1 and Clock Phase is 1 */
        SPISetGpioHigh(pstuSpi->u32SCLK);
        for(i = 7; i >= 0; i--)
        {
            if(u8Byte & (1 << i))
            {
                SPISetGpioHigh(pstuSpi->u32MOSI);
            }
            else
            {
                SPISetGpioLow(pstuSpi->u32MOSI);
            }
            SPISetGpioLow(pstuSpi->u32SCLK);
            SPIDelay;
            SPISetGpioHigh(pstuSpi->u32SCLK);
            SPIDelay;
        }
        SPISetGpioHigh(pstuSpi->u32SCLK);
        break;

    default:
        break;
    }
    
    return 0;
}


/******************************************************************************
**函数名称：__SPIWrite
**函数功能：发送数据
**输入参数：pstuSpi---spi结构体
**          pu8Data---待发送数据指针
**          s32dataLength---数据长度
**         
**输出参数：0
******************************************************************************/
static int __SPIWrite(Spi_t* pstuSpi, byte* pu8Data, int s32dataLength)
{
    int i = 0;
    int ret = 0;

	DBG("%s start to write data...\n", __func__);
	// Write data
    for(i = 0; i < s32dataLength; i++)
    {
        ret = SPIWriteByte(pstuSpi, pu8Data[i]);
    }
	DBG("%s finish to write data...\n", __func__);
    return ret;
}

/******************************************************************************
**函数名称：SPIWrite
**函数功能：发送数据
**输入参数：pstuSpi---spi结构体
**          pu8Data---待发送数据指针
**          s32dataLength---数据长度
**         
**输出参数：0
******************************************************************************/

static int SPIWrite(Spi_t* pstuSpi, byte* pu8Data, int s32dataLength)
{
	int ret;
	int clk_change = GPIO_OUT_LOW;
	switch(pstuSpi->spiMode) {
		case Mode_0:
		case Mode_1:
			break;
		case Mode_2:
		case Mode_3:
			clk_change = GPIO_OUT_HIGH;
			break;
		default:
			break;
	}
	
	spin_lock(&(pstuSpi->lock));
	SPICSIsEnable(pstuSpi, SPI_CS_ENABLE);
	SPIDelay;
	ret = __SPIWrite(pstuSpi, pu8Data, s32dataLength);
	SPIDelay;
	SPICSIsEnable(pstuSpi, SPI_CS_DISABLE);
	SPIDelay;
	clk_change ? SPISetGpioLow(pstuSpi->u32SCLK) : SPISetGpioHigh(pstuSpi->u32SCLK);
	SPIDelay;
	!clk_change ? SPISetGpioLow(pstuSpi->u32SCLK) : SPISetGpioHigh(pstuSpi->u32SCLK);
	spin_unlock(&(pstuSpi->lock));
	
	if(ret) {
		printk("[%s]: write failed \n", __func__);
		return ret;
	}
	return 0;
}

/******************************************************************************
**函数名称：SPIReadByte
**函数功能：读取一个字节
**输入参数：pstuSpi---spi结构体
**         
**          
**         
**输出参数：0
******************************************************************************/
static unsigned char SPIReadByte(Spi_t* pstuSpi)
{
    int i = 0;
    byte u8readData = 0;
 

    switch(pstuSpi->spiMode)
    {
    case Mode_0: /* Clock Polarity is 0 and Clock Phase is 0 */
        SPISetGpioLow(pstuSpi->u32SCLK);
        for(i = 0; i < 8; i++)
        {
            SPISetGpioLow(pstuSpi->u32SCLK);
            SPIDelay;
            SPISetGpioHigh(pstuSpi->u32SCLK);
            u8readData = u8readData << 1;
            u8readData |= GPIO_GET_VALUE(pstuSpi->u32MISO);
            SPIDelay;
        }
        SPISetGpioLow(pstuSpi->u32SCLK);
        break;

    case Mode_1: /* Clock Polarity is 0 and Clock Phase is 1 */
        SPISetGpioLow(pstuSpi->u32SCLK);
        for(i = 0; i < 8; i++)
        {
            SPISetGpioHigh(pstuSpi->u32SCLK);
            SPIDelay;
            SPISetGpioLow(pstuSpi->u32SCLK);
            u8readData = u8readData << 1;
            u8readData |= GPIO_GET_VALUE(pstuSpi->u32MISO);
            SPIDelay;
        }
        SPISetGpioLow(pstuSpi->u32SCLK);
        break;

    case Mode_2: /* Clock Polarity is 1 and Clock Phase is 0 */
        SPISetGpioHigh(pstuSpi->u32SCLK);
        for(i = 0; i < 8; i++)
        {
            SPISetGpioHigh(pstuSpi->u32SCLK);
            SPIDelay;
            SPISetGpioLow(pstuSpi->u32SCLK);
            u8readData = u8readData << 1;
            u8readData |= GPIO_GET_VALUE(pstuSpi->u32MISO);
            SPIDelay;
        }
        SPISetGpioHigh(pstuSpi->u32SCLK);
        break;

    case Mode_3:  /* Clock Polarity is 1 and Clock Phase is 1 */
        SPISetGpioHigh(pstuSpi->u32SCLK);
        for(i = 0; i < 8; i++)
        {
            SPISetGpioLow(pstuSpi->u32SCLK);
            SPIDelay;
            SPISetGpioHigh(pstuSpi->u32SCLK);
            u8readData = u8readData << 1;
            u8readData |= GPIO_GET_VALUE(pstuSpi->u32MISO);
            SPIDelay;
        }
        SPISetGpioHigh(pstuSpi->u32SCLK);
        break;

    default:
        break;
    }

    return u8readData;
}


/******************************************************************************
**函数名称：__SPIRead
**函数功能：读取数据
**输入参数：pstuSpi---spi结构体
**          pu8Data---数据指针
**          s32dataLength---数据长度
**         
**输出参数：0
******************************************************************************/
static void __SPIRead(Spi_t* pstuSpi, byte* pu8Data, int s32dataLength)
{
    int i = 0;

	DBG("%s start to read data...\n", __func__);
    // Read data
    for(i = 0; i < s32dataLength; i++)
    {
        pu8Data[i] = SPIReadByte(pstuSpi);
    }
    DBG("%s finish to read data...\n", __func__);
}

/******************************************************************************
**函数名称：SPIRead
**函数功能：读取数据
**输入参数：pstuSpi---spi结构体
**          pu8Data---数据指针
**          s32dataLength---数据长度
**         
**输出参数：0
******************************************************************************/
static void SPIRead(Spi_t* pstuSpi, byte* pu8Data, int s32dataLength)
{
	int ret;
	int clk_change = GPIO_OUT_LOW;
	byte u8Data;
	u8Data = pu8Data[0];
	switch(pstuSpi->spiMode) {
		case Mode_0:
		case Mode_1:
			break;
		case Mode_2:
		case Mode_3:
			clk_change = GPIO_OUT_HIGH;
			break;
		default:
			break;
	}
	
	spin_lock(&(pstuSpi->lock));
	SPICSIsEnable(pstuSpi, SPI_CS_ENABLE);
	SPIDelay;
	ret = __SPIWrite(pstuSpi, &u8Data, 1);
	__SPIRead(pstuSpi, pu8Data, s32dataLength);
	SPIDelay;
	SPICSIsEnable(pstuSpi, SPI_CS_DISABLE);
	SPIDelay;
	clk_change ? SPISetGpioLow(pstuSpi->u32SCLK) : SPISetGpioHigh(pstuSpi->u32SCLK);
	SPIDelay;
	!clk_change ? SPISetGpioLow(pstuSpi->u32SCLK) : SPISetGpioHigh(pstuSpi->u32SCLK);
	spin_unlock(&(pstuSpi->lock));
	
	if(ret)
		printk("[%s]: read failed\n", __func__);
	
}

static long spi_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
		case SPI_CHOOSE_CS:
		    DBG("SPI_COMMAND: %d-->SPI_CHOOSE_CS:  CS_%lu \n", cmd, arg);
			spi_master.u32CS = SPI_CS_gpio[arg];
			break;
		case SPI_CHANGE_MODE:
			DBG("SPI_COMMAND: %d-->SPI_CHANGE_MODE:  mode_%lu \n", cmd, arg);
			spi_master.spiMode = arg;
			break;
		/**
		 * to be continued
		 */
		default:
			return -EINVAL;
	}
	
	return 0;
}

static int spi_open(struct inode * inode, struct file * file)
{
	return 0;
}
 
static int spi_close(struct inode * inode, struct file * file)
{
	return 0;
}

static ssize_t spi_read(struct file *file, char __user *buf, size_t len, loff_t *f_pos)
{
	int i;
	int retval;
	loff_t pos = *f_pos;
	
	if(pos >= SPI_TRANS_DATA_LENGTH)
	{
		retval = 0;
		goto exit;
	}
	if(len > (SPI_TRANS_DATA_LENGTH - pos))
		len = SPI_TRANS_DATA_LENGTH - pos;
	pos += len;
	
	if(copy_from_user(read_write_data, buf, len))
	{
		retval = -EFAULT;
		goto exit;
	}
	DBG("read addr from user: %02x\n", *read_write_data);
	
	SPIRead(&spi_master, read_write_data, len);
	
	if(!read_write_data)
		goto exit;
	
	if(copy_to_user(buf, read_write_data, len))
	{
		retval = -EFAULT;
		goto exit;
	}
	for(i = 0; i < len; i++) {
		DBG("read from MISO:%02x\n", read_write_data[i]);
	}
	*f_pos = pos;
	
exit:
	return len;
	
}

static ssize_t spi_write(struct file *file, const char __user *buf, size_t len, loff_t *f_pos)
{
	int i;
	ssize_t retval = -ENOMEM;
	loff_t pos = *f_pos;
	DBG("from user write length: %lu\n", len);
	if(pos > SPI_TRANS_DATA_LENGTH)
		goto exit;
	
	if(len > (SPI_TRANS_DATA_LENGTH - pos))
		len = SPI_TRANS_DATA_LENGTH - pos;
	
	pos += len;
	
	if(copy_from_user(read_write_data, buf, len))
	{
		retval = -EFAULT;
		goto exit;
	}
	for(i = 0; i < len; i++) {
		DBG("from user write: %02x\n", read_write_data[i]);
	}
	
	retval = SPIWrite(&spi_master, read_write_data, len);
	
	if (retval)
	{
		printk("failed to write data...\n");
		goto exit;
	}
	
	*f_pos = pos;
	retval = len;
	
exit:
	return retval;
}


static struct file_operations spi_fops = {
	.owner          = THIS_MODULE,
	.write          = spi_write, 
	.read           = spi_read, 
	.open           = spi_open,
	.release        = spi_close,
	.unlocked_ioctl = spi_ioctl, 
};
static struct miscdevice spi_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = DEVICE_NAME,
	.fops  = &spi_fops,
};

static int __init spi_init(void)
{
	int ret;
	

	
	ret = misc_register(&spi_dev);
	if (ret < 0)
	{
		printk("registerspi_gpio device failed!\n");
		return -1;
	}
	//初始化GPIO
	SPIConfigure(&spi_dev, &spi_master);
	DBG("SPI driver initialize successful!\n");

	return 0;
}
 
static void __exit spi_exit(void)
{
	GPIO_FREE(CS_0);
	GPIO_FREE(CS_1);
	GPIO_FREE(CS_2);
	GPIO_FREE(CS_3);
	GPIO_FREE(SCLK);
	GPIO_FREE(MISO);
	GPIO_FREE(MOSI);
	misc_deregister(&spi_dev);

	printk("SPI driver exit!\n");
}
module_init(spi_init);
module_exit(spi_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRV_AUTHOR);
MODULE_DESCRIPTION(DRV_DESC);

