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



int main(int argc, char** argv)
{
	printf("hello world\n");
	char *pData = "this is a test string";
	
	int fd;
	int size;
	int i;
	int len;
	char buf[10];
	
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
	write(fd, pData, sizeof(*pData));
	size = read(fd, buf, sizeof(buf));
	close(fd);
	printf("read return %d\n", size);
	len = sizeof(buf);
	for(i = 0; i < len; i++)
		printf("%d \n", buf[i]);
	
	return 0;
}
