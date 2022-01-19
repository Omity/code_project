#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/types.h>

#define DEVICE_NAME      "/dev/demo_misc_ctl"


int main(int argc, char **argv)
{
	int fd;
	int ret = -1;
	int i;
	
	if((fd = open(DEVICE_NAME, O_RDWR)) < 0)
	{
		printf("open device %s failed\n", DEVICE_NAME);
		exit(1);
	}
	for(i = 0; i < 5; i++)
	{
		ret = ioctl(fd, i, 1);
		if(ret < 0)
		{
			printf("ioctl failed at %d\n", i);
		}
	}
	
	return 0;
}
