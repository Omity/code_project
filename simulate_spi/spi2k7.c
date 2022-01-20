#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "spi_bus.h"
#include <errno.h>

static uint32_t BUFSZ = 4096;
static uint32_t BITSZ = 0x1000000;//16m

static void pabort(const char *s)
{	
	printf(s);
	exit(1);
}


//static int transfer(int fd, unsigned char *ts_buf, unsigned int len)
//{
	//int rx = 0;
	//int ret;
	//struct spi_ioc_transfer tr = {
             //.tx_buf = (unsigned long)ts_buf,   //定义发送缓冲区指针
             //.rx_buf = (unsigned long)&rx,   //定义接收缓冲区指针
             //.len = len,
             //.delay_usecs = SPI_DELY};

    //ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);//执行spidev.c中ioctl的default进行数据传输

	//if (ret == 1)
	//{
        //pabort("can't send spi message");
	//}
	//rx  =  rx >> 8;
	//ret =  rx & 0xff;
	//rx  =  rx >> 8;
	//rx  =  rx | (ret << 8);
	//return rx;
//}

static int set_gpio_value(int fd, unsigned int gpio, int val)
{
	int ret;
	ret = ioctl(fd, gpio, val);
	if(ret == -1)
	{
		printf("can\'t set %u to %d\n", gpio, val);
		return ret;
	}
	printf("set value to: %d\n", val);
	
	return 0;
}

void set_spi_mode(int fd, uint8_t mode)
{
	int ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		 pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		 pabort("can't get spi mode");

}

void set_spi_bits(int fd, uint8_t bits)
{
	int ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		 pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		 pabort("can't get bits per word");
}

void set_spi_speed(int fd, uint32_t speed)
{
	int ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		 pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		 pabort("can't get max speed hz");	
}


//sending k7 bit stream
int down_K7(int fd, const char* bit_name)
{
	int   nFileSize = 0;
	unsigned char* pDataBuf = NULL;

	FILE * pFile = fopen(bit_name, "r");
	if(pFile)
	{
		pDataBuf = (unsigned char*)malloc(BITSZ);
		if(pDataBuf)
		{
			int step;
			//int i;
			int nLeftSize;
			unsigned char* pBits = NULL;
			
			SPI_SEL_DEV();

			nFileSize = fread(pDataBuf, 1, BITSZ, pFile);

			//printf("Sending :0x%x\n", nFileSize);
			pBits = pDataBuf;
			step = nFileSize / BUFSZ;
			while(step)
			{
				write(fd, pBits, BUFSZ);
				pBits += BUFSZ;
				step--;
			}
			nLeftSize = pDataBuf + nFileSize - pBits;			
			write(fd, pBits, nLeftSize);

			usleep(10000);//10ms

			free(pDataBuf);
			//wait for done
			// SPI_SEL_CPLD();
		}
		fclose(pFile);
		return 0;			
	}

	return -1;
}

int main(int argc, char *argv[])
{
	int fd;
	//int i;
	//int ret;
	int fd_drv;
	int value = 2;
	char* pK160S = "/rigol/SPU_BIT.bit";

	printf("start new part\n");
	if(argc > 1 )
	{
		pK160S = argv[1];
	}

	
	if(argc == 0 )
	{
		printf("spi2k7 K7_Master [K7_Slave]----Build:%s\n", __DATE__);
		printf("Down default %s\n", pK160S);
	}

	if ((fd = open(SPI_NAME, O_RDWR)) < 0)
		pabort("Can't open device\n");
	if((fd_drv = open(SPI2K7_DRIVER_NAME, O_RDWR)) < 0)
		pabort("Can't open device\n");


//!TODO 在此将 PIN_PROGRAMB 脚拉低，1us后拉高

	//ioctl(fd_drv, IOCTL_INITB, &value);
	//fprintf(stdout, "return initb voltage: %d\n", value);
	set_gpio_value(fd_drv, IOCTL_PROGRAMB, ACTIVE_LOW);
	usleep(1);
	set_gpio_value(fd_drv, IOCTL_PROGRAMB, ACTIVE_HIGH);
	
	//ioctl(fd_drv, IOCTL_INITB, &value);
	//fprintf(stdout, "return initb voltage: %d\n", value);
	while(value != 1)
	{
		ioctl(fd_drv, IOCTL_INITB, &value);
		//fprintf(stdout, "return initb voltage: %d\n", value);
	}
	
	set_spi_bits(fd, SPI_BITS);
	set_spi_mode(fd, SPI_MODE);
	set_spi_speed(fd, 10000000);	
	//master
	printf("Down K7 master\n");
	if(down_K7(fd, pK160S) != 0)
	{
		close(fd);
		pabort("!!!Down K7 slave failed\n");
	}
//!TODO 在此检测 PIN_DONE是否拉高，否则重试下载
	ioctl(fd_drv, IOCTL_DONE, &value);
	if(!value)
	{
		if(down_K7(fd, pK160S) != 0)
		{
			close(fd);
			pabort("!!!Down K7 slave failed\n");
		}
		ioctl(fd_drv, IOCTL_DONE, &value);
	}
	fprintf(stdout, "return value:%d and done\n", value);
	close(fd);
	close(fd_drv);
	return 0;
}
