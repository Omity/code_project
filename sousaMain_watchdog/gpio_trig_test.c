#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>//read,write等等
#include <fcntl.h>

#define DEVICE_NAME  "/dev/gpio_trig"

// ioctl 的输入命令
#define TRIG_GPIO_OUT_LOW 0
#define TRIG_GPIO_OUT_HIGH 1
#define TRIG_GPIO_SET_PID 2
#define TRIG_GPIO_SET_TRIG_RISING 3
#define TRIG_GPIO_SET_TRIG_FALLING 4
#define TRIG_GPIO_SET_TRIG_LOW 5
#define TRIG_GPIO_SET_TRIG_HIGH 6
#define GPIO_IN_READ_STATE      7
#define TRIG_IN_1               0
#define TRIG_IN_2               1
#define TRIG_IN_3               2
#define TRIG_IN_4               3


struct trigInDataStu{
	char trigInName[4][20];
	int trigInState[4];
};

int main(int argv, char **argc)
{
	int fd;
	int i;
	struct trigInDataStu trigInData;

	if((fd = open(DEVICE_NAME, O_RDWR)) < 0)
	{
		printf("Open Device failed.\r\n");
		exit(1);
	}
	if(ioctl(fd, TRIG_GPIO_SET_TRIG_FALLING, TRIG_IN_2) < 0)
	{
		printf("ioctl err!!\n");
	}
	if(ioctl(fd, TRIG_GPIO_SET_TRIG_RISING, TRIG_IN_4) < 0)
	{
		printf("ioctl err!!\n");
	}
	if(ioctl(fd, GPIO_IN_READ_STATE, &trigInData) < 0)
	{
		printf("ioctl err!!\n");
	}
	for(i = 0; i < 4; i++)
	{
	    printf("%s state: %d\n", trigInData.trigInName[i], trigInData.trigInState[i]);
	}
	close(fd);
	
	return 0;
}
