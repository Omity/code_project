#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/types.h>
#include <time.h>


#define DEVICE_NAME      "/dev/demo_misc_ctl"

#define DEMO_MISC_MAGIC       'x' 
#define IOCTL_INITB           _IO(DEMO_MISC_MAGIC, 0x01)
#define IOCTL_PROGRAMB        _IO(DEMO_MISC_MAGIC, 0x02)
#define IOCTL_DONE	          _IO(DEMO_MISC_MAGIC, 0x03)

static unsigned int cmd[] = {
	IOCTL_INITB,
	IOCTL_PROGRAMB,
	IOCTL_DONE,
};

int main(int argc, char **argv)
{
	int fd;
	int ret = -1;
	int i = 1;
	
	if((fd = open(DEVICE_NAME, O_RDWR)) < 0)
	{
		printf("open device %s failed\n", DEVICE_NAME);
		exit(1);
	}
	for(i = 0; i < 2; i++)
	{
		ret = ioctl(fd, cmd[1], i);
		if(ret < 0)
		{
			printf("ioctl failed\n");
		}
		usleep(1);
	}
	
	return 0;
}
