/*******************************************************************************
                普源精电科技技术有限公司版权所有
********************************************************************************
  源文件名: uart_phy.c
  功能描述: 型号为8250的uart设备测试的主程序
  作    者: YangYuLiang
  版    本: Ver1.0
  创建日期: 2020.07.08
    
  修改历史:
  修改日期：
  版 本 号：
  修 改 人：
  修改内容：
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <sys/ioctl.h> 
#include <linux/serial.h>
#include "uart_phy.h"

keyDealStru *g_pstkeyDeal = NULL;
ResvDataQueueStru *g_pstResvDataQueue = NULL;

/*******************************************************************************
  * 函    数：uartOpen
  * 描    述：打开设备节点
  * 输入参数：
  *           参数名称                  参数类型           参数说明
  *           ps8PathName               char*           设备节点路径  
  *           stUartDepolyInfo    uartDepolyInfoStru    配置uart的结构体           
  * 返 回 值：错误码
  * 说    明：外部函数
 ******************************************************************************/
int uartOpen(const char* ps8PathName)
{
    // 
    int s32UartFileFd = 0;
    /** 以非阻塞方式打开 串口 */
    s32UartFileFd = open(ps8PathName, O_RDWR | O_NONBLOCK);

    if ( s32UartFileFd < 0 )
    {
        perror("Unable to open uart_dev file");
		return UART_PHY_ERR;
    }

    return s32UartFileFd;
}

/*******************************************************************************
  * 函    数：uartSpecialSetup
  * 描    述：uart接口的非标准配置
  * 输入参数：
  *           参数名称              参数类型           参数说明
  *           s32UartFileFd         s32              文件描述符    
  *        	  TuartDepolyInfoStru   uartDepolyInfo   配置uart的结构体
  * 返 回 值：错误代码
  * 说    明：外部函数
 ******************************************************************************/
int uartSpecialSetup(int s32UartFileFd, uartDepolyInfoStru  stUartDepolyInfo)
{
	int s32RetValue = 0;

    // 串口的配置结构体
	struct termios TuartOptStru;  
    struct serial_struct stSerialOpt;  

	/* Get current options */
	tcgetattr(s32UartFileFd , &TuartOptStru);
    /* Flush the buffer */
    tcflush(s32UartFileFd, TCIOFLUSH);

    // 设置非标准波特率
	cfsetispeed(&TuartOptStru, B38400);
    cfsetospeed(&TuartOptStru, B38400);
	
    
	// 设置uart接口的波特率 
    s32RetValue = tcsetattr(s32UartFileFd, TCSANOW, &TuartOptStru);
    if(s32RetValue != 0)
    {
        perror("Unable to set uart_dev opt config");
        return UART_PHY_ERR;
    }
    /* Get configurations vim IOCTL */
    if((ioctl(s32UartFileFd, TIOCGSERIAL,&stSerialOpt)) < 0)
    {
        printf("Fail to get Serial!\n");
        return UART_PHY_ERR;
    }

    stSerialOpt.flags = ASYNC_SPD_CUST;  
    //custom_divisor  =  baud_base / baud； 其中 baud_base = 系统时钟（24 M） / 16, baud为波特率 
    stSerialOpt.custom_divisor = stSerialOpt.baud_base / stUartDepolyInfo.s32UartBaundBate; 

    /* Set it */
    if((ioctl(s32UartFileFd, TIOCSSERIAL, &stSerialOpt)) < 0)
    {
        printf("Fail to set Serial\n");
        return UART_PHY_ERR;
    }


    TuartOptStru.c_cflag |= (CREAD | CLOCAL);
    TuartOptStru.c_cflag &= ~CSIZE;
    /* 设置数据位 */
    switch(stUartDepolyInfo.s32UartDateBit)
    {
        case 7:
            TuartOptStru.c_cflag |= CS7;
            break;
        case 8:
            TuartOptStru.c_cflag |= CS8;
            break;
    }
    
    /* 设置校验位 */
    switch(stUartDepolyInfo.s8UartCheckBit)
    {
        case 'O':
            TuartOptStru.c_cflag |= PARENB;
            TuartOptStru.c_cflag |= PARODD;
            TuartOptStru.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'E':
            TuartOptStru.c_iflag |= (INPCK | ISTRIP);
            TuartOptStru.c_cflag |= PARENB;
            TuartOptStru.c_cflag &= ~PARODD;
            break;
        case 'N':
            TuartOptStru.c_cflag &= ~PARENB;
			TuartOptStru.c_iflag &= ~INPCK;
			break;
    }

    /* 设置停止位 */
    if(stUartDepolyInfo.s32UartStopBit == 1)
    {
        TuartOptStru.c_cflag &= ~CSTOPB;
    }
    else if(stUartDepolyInfo.s32UartStopBit == 2)
    {
        TuartOptStru.c_cflag |= CSTOPB;
    }
    TuartOptStru.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /* Raw mode */
    TuartOptStru.c_oflag &= ~OPOST; /* raw output */
    TuartOptStru.c_oflag &= ~(ONLCR | OCRNL);
    TuartOptStru.c_iflag &= ~(ICRNL | INLCR | IGNCR);
    TuartOptStru.c_iflag &= ~(IXON | IXOFF | IXANY);

    /* 设置超时时间 */
    TuartOptStru.c_cc[VTIME] = 10;   // 设置超时为15sec
    TuartOptStru.c_cc[VMIN] = 1;     // Update the Opt and do it now

    /* 处理未接收字符 */
    tcflush(s32UartFileFd, TCIFLUSH);
    s32RetValue = tcsetattr(s32UartFileFd, TCSANOW, &TuartOptStru);
    if(s32RetValue != 0)
    {
        perror("Unable to set uart_dev attr config");
        return UART_PHY_ERR;
    }

    return UART_PHY_NO_ERR;
}



/*******************************************************************************
  * 函    数：uartSetup
  * 描    述：uart接口的配置
  * 输入参数：
  *           参数名称              参数类型           参数说明
  *           s32UartFileFd         s32              文件描述符    
  *        	  TuartDepolyInfoStru   uartDepolyInfo   配置uart的结构体
  * 返 回 值：错误代码
  * 说    明：外部函数
 ******************************************************************************/
int uartSetup(int s32UartFileFd, uartDepolyInfoStru  stUartDepolyInfo)
{
	int s32RetValue = 0;

    // 串口的配置结构体
	struct termios TuartOptStru;  

	/* 配置串口 */
	tcgetattr(s32UartFileFd , &TuartOptStru);

	/* 设置配置结构体的波特率 */
	switch(stUartDepolyInfo.s32UartBaundBate)
	{
        case 2400:
			cfsetispeed(&TuartOptStru, B2400);
			cfsetospeed(&TuartOptStru, B2400);
            break;
        case 4800:
			cfsetispeed(&TuartOptStru, B4800);
			cfsetospeed(&TuartOptStru, B4800);
            break;
        case 9600:
			cfsetispeed(&TuartOptStru, B9600);
			cfsetospeed(&TuartOptStru, B9600);
            break;
        case 115200:
			cfsetispeed(&TuartOptStru, B115200);
			cfsetospeed(&TuartOptStru, B115200);
            break;
        case 500000:
			cfsetispeed(&TuartOptStru, B500000);
			cfsetospeed(&TuartOptStru, B500000);
            break;
        case 576000:
			cfsetispeed(&TuartOptStru, B576000);
			cfsetospeed(&TuartOptStru, B576000);
            break;    
        case 921600:
			cfsetispeed(&TuartOptStru, B921600);
			cfsetospeed(&TuartOptStru, B921600);
            break;
        case 1000000:
			cfsetispeed(&TuartOptStru, B1000000);
			cfsetospeed(&TuartOptStru, B1000000);
            break;
        case 1500000:
			cfsetispeed(&TuartOptStru, B1500000);
			cfsetospeed(&TuartOptStru, B1500000);
            break;
    }
	

	// 设置uart接口的波特率 
    s32RetValue = tcsetattr(s32UartFileFd, TCSANOW, &TuartOptStru);
    if(s32RetValue != 0)
    {
        perror("Unable to set uart_dev opt config");
        return UART_PHY_ERR;
    }

    TuartOptStru.c_cflag |= (CREAD | CLOCAL);
    TuartOptStru.c_cflag &= ~CSIZE;
    /* 设置数据位 */
    switch(stUartDepolyInfo.s32UartDateBit)
    {
        case 7:
            TuartOptStru.c_cflag |= CS7;
            break;
        case 8:
            TuartOptStru.c_cflag |= CS8;
            break;
    }
    
    /* 设置校验位 */
    switch(stUartDepolyInfo.s8UartCheckBit)
    {
        case 'O':
            TuartOptStru.c_cflag |= PARENB;
            TuartOptStru.c_cflag |= PARODD;
            TuartOptStru.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'E':
            TuartOptStru.c_iflag |= (INPCK | ISTRIP);
            TuartOptStru.c_cflag |= PARENB;
            TuartOptStru.c_cflag &= ~PARODD;
            break;
        case 'N':
            TuartOptStru.c_cflag &= ~PARENB;
			TuartOptStru.c_iflag &= ~INPCK;
			break;
    }

    /* 设置停止位 */
    if(stUartDepolyInfo.s32UartStopBit == 1)
    {
        TuartOptStru.c_cflag &= ~CSTOPB;
    }
    else if(stUartDepolyInfo.s32UartStopBit == 2)
    {
        TuartOptStru.c_cflag |= CSTOPB;
    }
    TuartOptStru.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /* Raw mode */
    TuartOptStru.c_oflag &= ~OPOST; /* raw output */
    TuartOptStru.c_oflag &= ~(ONLCR | OCRNL);
    TuartOptStru.c_iflag &= ~(ICRNL | INLCR | IGNCR);
    TuartOptStru.c_iflag &= ~(IXON | IXOFF | IXANY);

    /* 设置超时时间 */
    TuartOptStru.c_cc[VTIME] = 10;   //设置超时为15sec
    TuartOptStru.c_cc[VMIN] = 1;     //Update the Opt and do it now

    /* 处理未接收字符 */
    tcflush(s32UartFileFd, TCIFLUSH);
    s32RetValue = tcsetattr(s32UartFileFd, TCSANOW, &TuartOptStru);
    if(s32RetValue != 0)
    {
        perror("Unable to set uart_dev attr config");
        return UART_PHY_ERR;
    }

    return UART_PHY_NO_ERR;
}


/*******************************************************************************
  * 函    数：uartDataRecv
  * 描    述：从uart接口接收数据
  * 输入参数：
  *           参数名称             参数类型      参数说明
  *           s32UartFileFd        s32         文件描述符  
  *           s32ReadLen           s32         读取数据的长度
  * 输出参数：
  *           参数名称            参数类型        参数说明
  *           pu8ReadDataBuf       u8*        存放读取数据的缓冲区指针
  * 返 回 值：无
  * 说    明：外部函数
 ******************************************************************************/
int uartDataRecv(int s32UartFileFd,  const int s32ReadLen, unsigned char *pu8ReadDataBuf)
{
	int s32TmpReadLen = 0;
	
    // 读取uart接口数据
    s32TmpReadLen = read(s32UartFileFd, (void*)pu8ReadDataBuf, s32ReadLen);

    return s32TmpReadLen;
}

/*******************************************************************************
  * 函    数：uartDataSeed
  * 描    述：从uart接口发送数据
  * 输入参数：
  *           参数名称              参数类型      参数说明
  *           s32UartFileFd        s32           文件描述符  
  *           s32WriteLen          s32           发送数据的长度
  * 输出参数：
  *           参数名称             参数类型       参数说明
  *           pu8WriteDataBuf      u8*            存放发送数据的缓冲区指针
  * 返 回 值：无
  * 说    明：外部函数
 ******************************************************************************/
void uartDataSeed(int s32UartFileFd, const int s32WriteLen, unsigned char *pu8WriteDataBuf)
{
	int s32TmpWriteLen = 0;
	int i = 0;
	
    // 发送uart接口数据
    s32TmpWriteLen = write(s32UartFileFd, (void*)pu8WriteDataBuf, s32WriteLen);
	if (s32TmpWriteLen <= 0)
	{
		perror("Unable to seed data");
		exit(0);
	}
}




 

