#ifndef _SPI_BUS_H_
#define _SPI_BUS_H_

#ifndef _SPI_CPLD_DEV
#define _SPI_CPLD_DEV

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))


#define SPI_IOC_SELECT_CS		_IOW(SPI_IOC_MAGIC, 100, __u8)


#define DEV_ADC 	0x0001
#define DEV_SADC	0x0002
#define DEV_PLL_5G	0x0004
#define DEV_PLL1_4_4G   0x0008
#define DEV_AFE_SRL	0x0010
#define DEV_VGA_CH4	0x0020
#define DEV_VGA_CH3	0x0040
#define DEV_VGA_CH2	0x0080
#define DEV_VGA_CH1	0x0100


#define SPI_SEL_CPLD()	{int cs_sel = 0;ioctl(fd, SPI_IOC_SELECT_CS, &cs_sel);}
#define SPI_SEL_DEV()   {int cs_sel = 1;ioctl(fd, SPI_IOC_SELECT_CS, &cs_sel);}


#define SPI_NAME  	"/dev/spidev2.0"
#define SPI_MODE	0
#define SPI_BITS	8
#define SPI_RATE	10000000
#define SPI_DELY	0
#define SPI2K7_DRIVER_NAME "/dev/spi2k7_ctl"

#define GPIO_A_1    0
#define GPIO_A_2    1
#define GPIO_A_3    2

#define ACTIVE_HIGH   1
#define ACTIVE_LOW    0

struct stPLL
{
	unsigned char reg;
	unsigned int  val;
};

struct stPLL_lmk0482
{
	unsigned int  reg;
	unsigned char val;
};
#endif
#endif
