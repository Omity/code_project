#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>//read,write等等
#include <fcntl.h>

#define DEVICE_NAME  "/dev/gpio_trig"

// ioctl 的out命令
#define TRIG_GPIO_OUT_LOW            0
#define TRIG_GPIO_OUT_HIGH           1
// ioctl 的in命令
#define TRIG_GPIO_SET_PID            2
#define TRIG_GPIO_SET_TRIG_RISING    3
#define TRIG_GPIO_SET_TRIG_FALLING   4
#define TRIG_GPIO_SET_TRIG_LOW       5
#define TRIG_GPIO_SET_TRIG_HIGH      6
#define GPIO_IN_READ_STATE           7
//pin脚
#define TRIG_IN_1                    0
#define TRIG_IN_2                    1
#define TRIG_IN_3                    2
#define TRIG_IN_4                    3
#define TRIG_OUT_1                   0
#define TRIG_OUT_2                   1
#define TRIG_OUT_3                   2
#define TRIG_OUT_4                   3


static int gpioTrigPin[] = {
	TRIG_IN_1,
	TRIG_IN_2,
	TRIG_IN_3,
	TRIG_IN_4,
	TRIG_OUT_1,
	TRIG_OUT_2,
	TRIG_OUT_3,
	TRIG_OUT_4
};

static int gpioTrigIOctl[] = {
	TRIG_GPIO_OUT_LOW,
	TRIG_GPIO_OUT_HIGH,
	GPIO_IN_READ_STATE,
	TRIG_GPIO_SET_TRIG_RISING
};
	
struct trigInDataStu{
	char trigInName[4][20];
	int trigInState[4];
	
};

static int gpioOpen(char *deviceName, mode_t mode)
{
	int fd;
	if((fd = open(deviceName, mode)) < 0)
	{
		printf("Open Device failed.\r\n");
		exit(1);
	}
	
	return fd;
}

static int gpioCheckIOctl(int fd, int cmd, struct trigInDataStu *pData)
{
	int i;
	if(ioctl(fd, cmd, pData) < 0)
	{
		printf("ioctl err!!\n");
		exit(1);
	}
	for(i = 0; i < 4; i++)
	{
		printf("%s state: %d\n", pData->trigInName[i], pData->trigInState[i]);
	}
	return 0;
}

static int gpioSetIOctl(int fd, int cmd, int pin)
{
	if(ioctl(fd, cmd, pin) < 0)
	{
		printf("ioctl err!\n");
		exit(1);
	}
	return 0;
}

int main(int argv, char **argc)
{
	int fd;
	int i;
	struct trigInDataStu trigInData;
	
	fd = gpioOpen(DEVICE_NAME, O_RDWR);
	for(i = 0; i < 2; i++)
	{
		gpioCheckIOctl(fd, gpioTrigIOctl[2], &trigInData);
		sleep(1);
		gpioSetIOctl(fd, gpioTrigIOctl[1], gpioTrigPin[i + 4]);
		sleep(2);
		gpioCheckIOctl(fd, gpioTrigIOctl[2], &trigInData);
		sleep(1);
		gpioCheckIOctl(fd, gpioTrigIOctl[2], &trigInData);
		sleep(1);
		gpioSetIOctl(fd, gpioTrigIOctl[3], gpioTrigPin[i]);
		sleep(1);
		gpioCheckIOctl(fd, gpioTrigIOctl[2], &trigInData);
	}
	
	close(fd);
	
	return 0;
}
