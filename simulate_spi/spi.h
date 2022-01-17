#ifndef __SPI_H__
#define __SPI_H__


#include <linux/module.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/miscdevice.h>
#include <linux/ioctl.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/spinlock.h>


#define GPIO_OUTPUT(pin, level)   gpio_direction_output(pin, level)
#define GPIO_REQUEST(pin, label)  gpio_request(pin, label)
#define GPIO_FREE(pin) 		      gpio_free(pin)
#define GPIO_SET_VALUE(pin, val)  gpio_set_value(pin, val)
#define GPIO_GET_VALUE(pin) 	  gpio_get_value(pin)
#define GPIO_INPUT(pin)           gpio_direction_input(pin)

typedef unsigned char           byte;
typedef unsigned short          word;

#define GPIO2_C3                (64+16+3)        //83
#define GPIO2_C4                (64+16+4)        //84
#define GPIO2_C5                (64+16+5)        //85
#define GPIO2_C6                (64+16+6)        //86     
#define GPIO2_C7                (64+16+7)        //87
#define GPIO2_D0                (64+24+0)        //88
#define GPIO2_D1                (64+24+1)        //89


// 定义SPI四个管脚(需对应主控gpio)
#define CS_0                    GPIO2_C7
#define CS_1                    GPIO2_C3
#define CS_2                    GPIO2_D0
#define CS_3                    GPIO2_D1
#define SCLK                    GPIO2_C6
#define MOSI                    GPIO2_C5
#define MISO                    GPIO2_C4

//SPI
#define SPI_CS_ENABLE           1
#define SPI_CS_DISABLE          0
#define SPI_TRANS_DATA_LENGTH   256

//gpio相关设定
#define GPIO_DIRECTION_INPUT    0
#define GPIO_DIRECTION_OUTPUT   1
#define GPIO_OUT_LOW            0
#define GPIO_OUT_HIGH           1

//IOCTL
#define SPI_CHOOSE_CS         0      //选择控制的从设备
#define SPI_CHANGE_MODE       1      //切换spi通信模式

#define SPIDelay  udelay(5)

unsigned int SPI_CS_gpio[] = {
	CS_0,
	CS_1,
	CS_2,
	CS_3,
};

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
    spinlock_t lock;
}Spi_t;

// 函数声明
#define spi_delay(delay)  udelay(delay)

static int GpioSetDirection(unsigned int gpio, const char *label, int dir);
static void SPISetGpioHigh(unsigned int gpio);
static void SPISetGpioLow(unsigned int gpio);
static int SPIConfigure(struct miscdevice *pdev, Spi_t* pstuSpi);
static int SPICSIsEnable(Spi_t* pstuSpi, int s32isEnable);
static int SPIWriteByte(Spi_t* pstuSpi, byte u8Byte);
static int SPIWrite(Spi_t* pstuSpi, byte* pu8Data, int s32dataLength);
static unsigned char SPIReadByte(Spi_t* pstuSpi);
static void SPIRead(Spi_t* pstuSpi, byte* pu8Data, int s32dataLength);





#endif
