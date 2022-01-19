#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>//read,write等等
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "version.h"

#define DEVICE_NAME      "/dev/spi_simulate"
#define TOOL_NAME        "spi_config"


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
	{0x07, 0x6000},
};

struct Venus_SPI_config low_min_input_gain[] = {
	
	{0x2F, 0x0001},
	{0x31, 0x00C0},
	{0x36, 0x0380},
	{0x2C, 0x841E},
	{0x2D, 0x0000},
	{0x2E, 0x0EF9},
	{0x2B, 0x0400},
	{0x07, 0x6000},
};

struct Venus_SPI_config low_input[] = {
	{0x2F, 0x0001},
	{0x31, 0x0030},
	{0x36, 0x001C},
	{0x2D, 0x0000},
	{0x2C, 0x490C},
	{0x2E, 0x4EF9},
	{0x2B, 0x0400},
	{0x07, 0x2000},
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
	{0x07, 0x4000},
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
	{0x07, 0x4000},
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
	{0x07, 0x0000},
};

struct Venus_SPI_config read_config[] = {
	{0x00, 0xFFFF},
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
	}
	else
	{
		msgs->buf = b;
		read(fd, msgs->buf, msgs->len);
		for(i = 0; i < msgs->len; i++)
		{
			fprintf(stdout, "read data: %02x\n", msgs->buf[i]);
		}
	}
	return 0;
}


static unsigned char config_info[][256] = {
		"0--> 低阻1mv/div",
		"1--> 低阻10mv/div",
		"2--> 低阻200mv/div",
		"3--> 高阻1mv/div",
		"4--> 高阻10mv/div",
		"5--> 高阻100mv/div",
};

static void help(void)
{
	fprintf(stderr, 
		"Usage: %s [-s which_CS] [-m Mode] [-f function] [-r width] ...\n" 
		"parameter:可支持大小写\n"
		"参数带[]可省略, 不使用则以默认参数进行\n"
		"  -s ----选择片选哪个从设备,默认选择CS_0, 可选参数为0, 1. 2, 3\n"
		"  -m ----传输模式,默认mode0,可选参数为0, 1, 2, 3\n"
		"  -f ----配置选项,针对哪一种配置参数,可选参数:\n"
		"      0: 低阻1mv/div\n"
		"      1: 低阻10mv/div\n"
		"      2: 低阻200mv/div\n"
		"      3: 高阻1mv/div\n"
		"      4: 高阻10mv/div\n"
		"      5: 高阻100mv/div\n"
		"  -r ----寄存器查询,仅可查0x00, width宽度最大256字节\n"
		"  -h ----帮助查询\n"
		"  -v ----版本查询\n"
		"  注: -h -v 为单选项\n"
		"  -g ----debug, 可获取一些打印日志\n"
		"  Example: 全部以默认参数,即片选0,mode0,配置选项0\n"
		"    %s\n"
		"  Example: 读CS_1 以mode2模式2字节\n"
		"    %s -r 2 -m 2\n"
		"  Example: 写CS_2 以默认模式, 配置选项3\n"
		"    %s -s 2 -f 3\n"
		,TOOL_NAME, TOOL_NAME, TOOL_NAME, TOOL_NAME
	);
}

int main(int argc, char **argv)
{

	struct SPI_msg msg;
	struct Venus_SPI_config *config_array;
	int fd;
	int config_idx = 0, cs_idx = 0, mode_idx = 0, help_mask = 0, version = 0, debug = 0, read_idx = 0, arg_idx = 1;
	int read_width;
	int i;
	int len;
	
	if((argc < 2) | (argc > 7))
	{
		fprintf(stdout, "invalid parameter, read help menu for more infomation\n");
		help();
		exit(1);
	}
	//获得配置选项
	while(arg_idx < argc )
	{
		if(!isdigit(*argv[arg_idx]) && (argv[arg_idx][0] == '-'))
		{
			switch(argv[arg_idx][1])
			{
				case 'h': help_mask = 1;break;
				case 'v':
				case 'V': version = 1;break;
				case 'g':
				case 'G': debug = 1; break;
				case 'r':
				case 'R':
					if(isdigit((int)*argv[arg_idx + 1]))
					{
						read_idx = 1;
						read_width = atoi(argv[arg_idx + 1]);
					}
					else
					{
						fprintf(stderr, "-r lack of parameter or parameter invalid\n");
						exit(1);
					}
					break;
				case 's':
				case 'S': 
					if(isdigit((int)*argv[arg_idx + 1]))
					{
						cs_idx = atoi(argv[arg_idx + 1]);
						if(cs_idx > 3)
						{
							fprintf(stderr, "CS only support 0,1,2,3\n");
							exit(1);
						}
					}
					else
					{
						fprintf(stderr, "-s lack of parameter or parameter invalid\n");
						exit(1);
					}
					break;
				case 'm':
				case 'M': 
					if(isdigit((int)*argv[arg_idx + 1]))
					{
						mode_idx = atoi(argv[arg_idx + 1]);
						if(mode_idx > 3)
						{
							fprintf(stderr, "mode only support 0,1,2,3\n");
							exit(1);
						} 
					}
					else
					{
						fprintf(stderr, "-m lack of parameter or parameter invalid\n");
						exit(1);
					}
					break;
				case 'f':
				case 'F': 
					if(isdigit((int)*argv[arg_idx + 1]))
					{
						config_idx = atoi(argv[arg_idx + 1]);
						if(config_idx > 5)
						{
							fprintf(stderr, "config only support 0,1,2,3,4,5\n");
							exit(1);
						}
					} 
					else
					{
						fprintf(stderr, "-f lack of parameter or parameter invalid\n");
						exit(1);
					}
					break;
				default:
					break;
			}
		}
		arg_idx++;
	}
	//fprintf(stdout, "help:%d\nversion:%d\ndebug:%d\narg_dix:%d\n", help_mask, version, debug, arg_idx);
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
		fprintf(stdout, 
				"Open Device failed.\r\n"
				"try insmod rigol/driver/spi.ko to load spi driver\n"
				);
		exit(1);
	}
	//片选
	if(cs_idx)
	{
		if(ioctl(fd, SPI_CHOOSE_CS, cs_idx) < 0)
		{
			printf("ioctl err!!\n");
			exit(1);
		}
	}
	debug ? fprintf(stdout, "choose: cs_%d\n", cs_idx) : 0;
	//模式切换
	if(mode_idx)
	{
		if(ioctl(fd, SPI_CHANGE_MODE, mode_idx) < 0)
		{
			printf("ioctl err!!\n");
			exit(1);
		}
	}
	debug ? fprintf(stdout, "choose: mode_%d\n", mode_idx) : 0 ;
	//读寄存器,只能读0x00
	if(read_idx)
	{
		msg.spi_msg = read_config[0];
		msg.flags   = SPI_M_RD;
		msg.len     = read_width;
		spi_transfer(fd, &msg);
		exit(0);
	}
	//写配置
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
	debug ? fprintf(stdout, "choose config: %s\n", config_info[config_idx]) : 0;
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
