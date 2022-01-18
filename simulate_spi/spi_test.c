#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>//read,write等等
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

//#include "version.h"

#define DEVICE_NAME      "/dev/spi_simulate"
#define TOOL_NAME        "spi_config"
#define VERSION          "1.0.0"

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

typedef unsigned char byte; 

struct Venus_SPI_config
{
	unsigned short reg;
	unsigned int val;
};

struct SPI_msg
{
	struct Venus_SPI_config spi_msg;
	unsigned short flags; //操作标志位
#define SPI_M_RD       0x80  //读取标志位
	unsigned int len;
	byte *buf;
};

struct Venus_SPI_config low_max_input_gain[] = {
	{0x2F, 0x0001},
	{0x36, 0x4000},
	{0x32, 0xD300},
	{0x2C, 0x800E},
	{0x2D, 0x0000},
	{0x2E, 0x0EF9},
	{0x2B, 0x0400},
};

struct Venus_SPI_config low_min_input_gain[] = {
	
	{0x2F, 0x0001},
	{0x31, 0x00C0},
	{0x36, 0x0380},
	{0x2C, 0x841E},
	{0x2D, 0x0000},
	{0x2E, 0x0EF9},
	{0x2B, 0x0400},
};

struct Venus_SPI_config low_input[] = {
	{0x2F, 0x0001},
	{0x31, 0x0030},
	{0x36, 0x001C},
	{0x2D, 0x0000},
	{0x2C, 0x490C},
	{0x2E, 0x4EF9},
	{0x2B, 0x0400},
};

struct Venus_SPI_config high_in_phase_max_gain[] = {
	{0x2B, 0x4000},
	{0x23, 0x2800},
	{0x24, 0x0000},
	{0x2F, 0x000C},
	{0x35, 0x009C},
	{0x29, 0x0080},
	{0x2A, 0x0C00},
	{0x2E, 0x5EF9},
	{0x2B, 0x4400},
};

struct Venus_SPI_config high_in_phase_min_gain[] = {
	{0x2B, 0x4000},
	{0x23, 0x2800},
	{0x24, 0x0000},
	{0x33, 0xC000},
	{0x35, 0x0800},
	{0x29, 0x4D80},
	{0x2A, 0x0C00},
	{0x2E, 0x1EF9},
	{0x2B, 0x4400},
};

struct Venus_SPI_config high_opposite_max_gain[] = {
	{0x2B, 0x4000},
	{0x23, 0x1000},
	{0x37, 0xC000},
	{0x20, 0x0000},
	{0x22, 0x0078},
	{0x21, 0x0B11},
	{0x26, 0x1CF6},
	{0x2F, 0x4E00},
	{0x29, 0x0280},
	{0x2A, 0x3C00},
	{0x2E, 0x1EF9},
	{0x2B, 0x4400},
};

struct Venus_SPI_config test_config[] = {
	{0x07, 0x3FA5},
	{0x00, 0x07A0},
	{0x07, 0x0000},
	{0x00, 0x1111},
};

static int spi_transfer(int fd, struct SPI_msg *msgs)
{
	int i;
	byte b[WRITE_MAX_LENGTH];
	
	if(msgs->len > WRITE_MAX_LENGTH) 
	{
		printf("data length too long!\n");
		exit(1);
	}
	b[0] = (msgs->flags & SPI_M_RD) | msgs->spi_msg.reg;
	if(!msgs->flags)
	{
		b[1] = msgs->spi_msg.val >> 8;
		b[2] = msgs->spi_msg.val;
		write(fd, b, msgs->len);
		printf("data: %02x%02x%02x\n", b[0], b[1], b[2]);
	}
	else
	{
		msgs->buf = b;
		read(fd, msgs->buf, msgs->len);
		for(i = 0; i < msgs->len; i++)
		{
			printf("read data: %02x\n", msgs->buf[i]);
		}
	}
	return 0;
}


static unsigned char config_info[][256] = {
	"aaa", 
	"aaa", 
	"aaa", 
	"aaa", 
	"aaa", 
	"aaa", 
};

static void help(void)
{
	fprintf(stderr, 
		"Usage: %s [-c which_CS] [-m Mode] [-f function] ...\n", TOOL_NAME
	);
}

int main(int argc, char **argv)
{
	//char *pData = (char *)(&(low_input_max_gain[0].reg));
	struct SPI_msg msg;
	struct Venus_SPI_config *config_array;
	int fd;
	int config_idx = 0, cs_idx = 0, mode_idx = 0, help_mask = 0, version = 1, debug = 0;
	int arg_idx = 1;
	int i;
	int len;
	
	if(argc > 7)
	{
		fprintf(stdout, "invalid parameter list\n");
		help();
	}
	//获得配置选项
	while((arg_idx < argc ) && (argv[arg_idx][0] == '-'))
	{
		switch(argv[arg_idx][1])
		{
			case 'h': help_mask = 1;break;
			case 'v':
			case 'V': version = 1;break;
			case 'g':
			case 'G': debug = 1; break;
			case 'c':
			case 'C': 
				if(isdigit((int)*argv[arg_idx + 1]))
					cs_idx = atoi(argv[arg_idx + 1]);
				else
				{
					fprintf(stderr, "-c lack of parameter or parameter invalid\n");
					exit(1);
				}
				break;
			case 'm':
			case 'M': 
				if(isdigit((int)*argv[arg_idx + 1]))
					mode_idx = atoi(argv[arg_idx + 1]); 
				else
				{
					fprintf(stderr, "-m lack of parameter or parameter invalid\n");
					exit(1);
				}
			break;
			case 'f':
			case 'F': 
				if(isdigit((int)*argv[arg_idx + 1]))
					config_idx = atoi(argv[arg_idx + 1]); 
				else
				{
					fprintf(stderr, "-f lack of parameter or parameter invalid\n");
					exit(1);
				}
			break;
			default:
				fprintf(stderr, "Error: Unsupported option \"%s\"!\n",
				argv[arg_idx]);
			help();
			exit(1);
		}
		arg_idx++;
	}
	if(help_mask)
	{
		help();
		exit(0);
	}
	if(version)
	{
		fprintf(stdout, "%s version: %s\n", TOOL_NAME, VERSION);
		exit(0);
	}
	
	if((fd = open(DEVICE_NAME, O_RDWR)) < 0)
	{
		printf("Open Device failed.\r\n");
		exit(1);
	}
	//片选
	if(ioctl(fd, SPI_CHOOSE_CS, cs_idx) < 0)
	{
		printf("ioctl err!!\n");
		exit(1);
	}
	debug ? fprintf(stdout, "choose cs_%d\n", cs_idx) : 0;
	if(mode_idx)
	{
		if(ioctl(fd, SPI_CHANGE_MODE, mode_idx) < 0)
		{
			printf("ioctl err!!\n");
			exit(1);
		}
		debug ? fprintf(stdout, "choose cs_%d\n", cs_idx) : 0 ;
	}
	
	switch(config_idx)
	{
		case 0: 
			config_array = low_max_input_gain;
			len = sizeof(low_max_input_gain) / sizeof(struct Venus_SPI_config);
			break;
		case 1:
			config_array = low_min_input_gain;
			len = sizeof(low_min_input_gain) / sizeof(struct Venus_SPI_config);
			break;
		case 2:
			config_array = low_input;
			len = sizeof(low_input) / sizeof(struct Venus_SPI_config);
			break;
		case 3:
			config_array = high_in_phase_max_gain;
			len = sizeof(high_in_phase_max_gain) / sizeof(struct Venus_SPI_config);
			break;
		case 4:
			config_array = high_in_phase_min_gain;
			len = sizeof(high_in_phase_min_gain) / sizeof(struct Venus_SPI_config);
			break;
		case 5:
			config_array = high_opposite_max_gain;
			len = sizeof(high_opposite_max_gain) / sizeof(struct Venus_SPI_config);
			break;
		default:
			config_array = low_max_input_gain;
			len = sizeof(low_max_input_gain) / sizeof(struct Venus_SPI_config);
			break;
	}
	debug ? fprintf(stdout, "choose config to: %s\n", config_info[config_idx]) : 0;
	for(i = 0; i < len; i++)
	{
		msg.spi_msg = config_array[i];
		msg.flags   = !SPI_M_RD;
		msg.len     = 3;
		debug ? fprintf(stdout, "reg: %02x val: %04x\n", config_array[i].reg, config_array[i].val) : 0;
		spi_transfer(fd, &msg);
	}
	close(fd);
	return 0;
}
