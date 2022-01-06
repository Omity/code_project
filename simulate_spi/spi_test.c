#include <linux/module.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <assert.h>

#if 1
#define DBG(x...)   printk(x)
#define DBG_PRINT
#else
#define DBG(x...)   do {} while (0)
#endif

#define GPIO_OUTPUT(pin, level)   gpio_direction_output(pin, level)
#define GPIO_REQUEST(pin, label)  gpio_request(pin, label)
#define GPIO_FREE(pin) 		      gpio_free(pin)
#define GPIO_SET_VALUE(pin, val)  gpio_set_value(pin, val)
#define GPIO_GET_VALUE(pin) 	  gpio_get_value(pin)
#define GPIO_INPUT(pin)           gpio_direction_input(pin)

typedef unsigned char           byte;
typedef unsigned short          word;


// 定义SPI四个管脚(需对应主控gpio)
#define CS                      12 
#define SCLK                    13
#define MOSI                    14
#define MISO                    15

//SPI
#define SPI_CS_ENABLE           1
#define SPI_CS_DISABLE          0

//gpio相关设定
#define GPIO_DIRECTION_INPUT    0
#define GPIO_DIRECTION_OUTPUT   1
#define GPIO_OUT_LOW            0
#define GPIO_OUT_HIGH           1


#define SPIDelay  delay_us(1)

// SPI 通信模式
typedef enum SPIMode
{
    Mode_0,   /* Clock Polarity is 0 and Clock Phase is 0 */
    Mode_1,   /* Clock Polarity is 0 and Clock Phase is 1 */
    Mode_2,   /* Clock Polarity is 1 and Clock Phase is 0 */
    Mode_3,   /* Clock Polarity is 1 and Clock Phase is 1 */
}SPIMode;

// SPI 模式
typedef enum SPIType
{
    SPIMaster,
    SPISlave,
}SPIType;

// SPI 结构体
typedef struct SpiStruct
{
    unsigned int u32CS;
    unsigned int u32SCLK;
    unsigned int u32MOSI;
    unsigned int u32MISO;
    SPIMode spiMode;
    SPIType spiType;
}Spi_t;

// 函数声明


// SPI 主设备结构体
Spi_t Spi_0 = 
{
    .u32CS = CS,
    .u32SCLK = SCLK,
    .u32MOSI = MOSI,
    .u32MISO = MISO,
    .spiMode = Mode_1,
    .spiType = SPIMaster,
};

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
static int SPISetGpioHigh(unsigned int gpio)
{
	return GPIO_SET_VALUE(gpio, GPIO_OUT_HIGH);
}


/******************************************************************************
**函数名称：SPISetGpioLow
**函数功能：设置管脚电平为低
**输入参数：gpio----管脚号
**         
**         
**输出参数：0
******************************************************************************/
static int SPISetGpioLow(unsigned int gpio)
{
	return GPIO_SET_VALUE(gpio, GPIO_OUT_LOW);
}


/******************************************************************************
**函数名称：SPIConfigure
**函数功能：初始化SPI管脚
**输入参数：pstuSpi---spi结构体
**         
**         
**输出参数：无
******************************************************************************/
static int SPIConfigure(Spi_t* pstuSpi)
{
    int ret = 0;
    if(!pstuSpi)
    {
		printk("error in %s parameter!\n", __func__);
		return -1;
	}

    if(pstuSpi->spiMode == SPIMaster)
    {
		GpioSetDirection(pstuSpi->u32CS, "spi_cs", GPIO_DIRECTION_OUTPUT);
		GpioSetDirection(pstuSpi->u32SCLK, "spi_sclk", GPIO_DIRECTION_OUTPUT);
		GpioSetDirection(pstuSpi->u32MOSI, "spi_mosi", GPIO_DIRECTION_OUTPUT);
		GpioSetDirection(pstuSpi->u32MISO, "spi_miso", GPIO_DIRECTION_INPUT);
    }
    else
    {
		GpioSetDirection(pstuSpi->u32CS, "spi_cs", GPIO_DIRECTION_INPUT);
		GpioSetDirection(pstuSpi->u32SCLK, "spi_sclk", GPIO_DIRECTION_INPUT);
		GpioSetDirection(pstuSpi->u32MOSI, "spi_mosi", GPIO_DIRECTION_INPUT);
		GpioSetDirection(pstuSpi->u32MISO, "spi_miso", GPIO_DIRECTION_OUTPUT);
    }

    SPISetGpioHigh(pstuSpi->u32CS);
    ret = pstuSpi->spiMode;
    switch(pstuSpi->spiMode)
    {
    case Mode_0:
    case Mode_1:
    case Mode_2:
		SPISetGpioHigh(pstuSpi->u32SCLK);
		break;
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
	int ret;
	if(s32isEnable)
		ret = SPISetGpioLow(pstuSpi->u32CS);  //片选使能拉低
	else
		ret = SPISetGpioHigh(pstuSpi->u32CS);
	if(ret)
		return ret;
		
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
    int ret;

    switch(pstuSpi->spiMode)
    {
    case Mode_0: /* Clock Polarity is 0 and Clock Phase is 0 */
		SPISetGpioLow(pstuSpi->u32SCLK);
        for(i = 7; i >= 0; i--)
        {
            SPISetGpioLow(pstuSpi->u32SCLK);
            SPIDelay;
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
        }
        SPISetGpioLow(pstuSpi->u32SCLK);
        break;

    case Mode_1: /* Clock Polarity is 0 and Clock Phase is 1 */
        SPISetGpioLow(pstuSpi->u32SCLK);
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
            SPISetGpioLow(pstuSpi->u32SCLK);
            if(u8Byte & (1 << i))
            {
                SPISetGpioHigh(pstuSpi->u32MOSI);
            }
            else
            {
                SPISetGpioLow(pstuSpi->u32MOSI);
            }
            SPIDelay;
            SPISetGpioHigh(pstuSpi->u32SCLK);
            SPIDelay;
        }
        SPISetGpioHigh(pstuSpi->u32SCLK);
        break;

    default:
        break;
    }
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
static int SPIWrite(Spi_t* pstuSpi, unsigned char* pu8Data, int s32dataLength)
{
    int i = 0;

    SPICSIsEnable(pstuSpi, SPI_CS_ENABLE);
    delay_us(8);

    // Write data
    for(i = 0; i < s32dataLength; i++)
    {
        SPIWriteByte(pstuSpi, pu8Data[i]);
    }

    delay_us(8);
    SPICSIsEnable(pstuSpi, SPI_CS_DISABLE);
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
    unsigned char u8readData = 0;

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
**函数名称：SPIRead
**函数功能：读取数据
**输入参数：pstuSpi---spi结构体
**          pu8Data---数据指针
**          s32dataLength---数据长度
**         
**输出参数：0
******************************************************************************/
void SPIRead(Spi_t* pstuSpi, unsigned char* pu8Data, int s32dataLength)
{
    int i = 0;

    SPICSIsEnable(pstuSpi, SPI_CS_ENABLE);
    delay_us(8);

    // Read data
    for(i = 0; i < s32dataLength; i++)
    {
        pu8Data[i] = SPIReadByte(pstuSpi);
    }

    delay_us(8);
    SPICSIsEnable(pstuSpi, SPI_CS_DISABLE);
}
