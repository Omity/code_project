#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>//read,write等等
#include <sys/ioctl.h>


#define DEVICE_NAME      "/dev/spi_simulate"

//ioctl
#define SPI_CHOOSE_CS    0
#define SPI_CHANGE_MODE  1

#define SPI_CS_0         0
#define SPI_CS_1         1
#define SPI_CS_2         2
#define SPI_CS_3         3

#define SPI_MODE_0       0
#define SPI_MODE_1       1
#define SPI_MODE_2       2
#define SPI_MODE_3       3

#define WRITE_MAX_LENGTH  256
typedef unsigned char Byte; 

struct Venus_SPI_config
{
	unsigned short reg;
	unsigned int val;
};

struct SPI_msg
{
	struct Venus_SPI_config spi_msg;
	unsigned short flags; //操作标志位
#define SPI_M_RD       0x01  //读取标志位
	unsigned int len;
	Byte *buf;
};

struct Venus_SPI_config low_input_max_gain[] = {
	{0x2F, 0x0001},
	{0x36, 0x4000},
	{0x32, 0xD300},
	{0x2C, 0x800E},
	{0x2D, 0x0000},
	{0x2E, 0x0EF9},
	{0x2B, 0x0400},
};

static int spi_transfer(int fd, struct SPI_msg *msgs)
{
	Byte b[WRITE_MAX_LENGTH];
	if(msgs->len > WRITE_MAX_LENGTH) 
	{
		printf("data length too long!\n");
		exit(1);
	}
	b[0] = msgs->flags & SPI_M_RD;
	b[1] = msgs->spi_msg.reg;
	if(msgs->flags)
	{
		b[2] = msgs->spi_msg.val;
		//msgs->buf = b;
		//write(fd, msgs->buf, msgs->len);
		write(fd, b, msgs->len);
	}
	else
	{
		//msgs->buf = b;
		write(fd, b, 3);
		read(fd, msgs->buf, msgs->len);
	}
	
	return 0;
}


int main(int argc, char **argv)
{
	printf("start to config AFE\n");
	//char *pData = (char *)(&(low_input_max_gain[0].reg));
	struct SPI_msg msg;
	int fd;
	int i;
	int len;
	
	len = sizeof(low_input_max_gain) / sizeof(struct Venus_SPI_config);
	//printf("length: %d\n", len);
	
	if((fd = open(DEVICE_NAME, O_RDWR)) < 0)
	{
		printf("Open Device failed.\r\n");
		exit(1);
	}
	if(ioctl(fd, SPI_CHOOSE_CS, SPI_CS_0) < 0)
	{
		printf("ioctl err!!\n");
		exit(1);
	}
	
	for(i = 0; i < len; i++)
	{
		msg.spi_msg = low_input_max_gain[i];
		msg.flags   = !SPI_M_RD;
		msg.len     = 4;
		spi_transfer(fd, &msg);
	}
	close(fd);
	return 0;
}
