/*******************************************************************************
                普源精电科技技术有限公司版权所有
********************************************************************************
  源文件名: key_phare.c
  功能描述: 按键键值解析
  作    者: YangYuLiang
  版    本: Ver1.0
  创建日期: 2020.03.12
    
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

#include "uart_data_deal.h"
#include "uart_queue.h"


/*******************************************************************************
  * 函    数：keyboardInit
  * 描    述：初始化键盘板（包括打开设备节点，配置uart设备节点，创建串口数据链表）
  * 输入参数：
  *           参数名称                  参数类型           参数说明
  *           ps8PathName               char*           设备节点路径  
  *           stUartDepolyInfo    uartDepolyInfoStru    配置uart的结构体     
  * 返 回 值：无
  * 说    明：外部函数
 ******************************************************************************/
int keyboardInit(const char* ps8PathName, uartDepolyInfoStru  stUartDepolyInfo)
{
    int s32RetValue =    0;

    // 文件描述符
    int s32UartFileFd = -1;

    // 打开设备节点
    s32UartFileFd = uartOpen(ps8PathName);
    if ( s32UartFileFd < 0 )
    { 
        perror("uart open failed");
        exit(0);
    }

    // 配置uart设备节点
    s32RetValue = uartSpecialSetup(s32UartFileFd, stUartDepolyInfo);
    if ( s32RetValue < 0 )
    {   
        perror("uart uartSetup failed");
        exit(0); 
    }
		    
		 
    return s32UartFileFd;
}

/*******************************************************************************
  * 函    数：keyboardSeed
  * 描    述：从键盘板串口发送数据
  * 输入参数：
  *           参数名称              参数类型        参数说明
  *           s32UartFileFd        s32           文件描述符  
  *           s32WriteReg          s32           发送逻辑的寄存器
  *           s32WriteData         s32           发送数据
  
  * 返 回 值：无
  * 说    明：外部函数
 ******************************************************************************/
void keyboardSeed(int s32UartFileFd, const int s32WriteReg, int s32WriteData)
{

    unsigned char au8WriteDataBuf[2] = { 0 };
    //寄存器地址（高4位）
    //当寄存器数据（低4位）
	au8WriteDataBuf[1] = 0xaa;
	au8WriteDataBuf[0] = (s32WriteReg << 4) & 0xff;
	au8WriteDataBuf[0] += s32WriteData & 0xff;
    // 读取数据
    uartDataSeed(s32UartFileFd, 2 , au8WriteDataBuf);

#if PRINT_KEY_VALUE
    key_debug("uart Send(0x%x) 0x%x\n", s32WriteReg, s32WriteData);
#endif
    return ;

}


/*******************************************************************************
  * 函    数：keyboardResv
  * 描    述：键盘板串口接受数据
  * 输入参数：
  *             参数名称                  参数类型              参数说明   
  *           s32UartFileFd                 s32         文件描述符  
  *           s32ResvDataLen                s32         读取数据的长度
  * 返 回 值：无
  * 说    明：外部函数
 ******************************************************************************/
void keyboardResv(const int s32ResvDataLen, int s32UartFileFd)
{ 
    int i = 0;
    unsigned char au8ResvDataBuf[UART_RESV_DATA_LEN] = {0};
    // 接收数据长度
    int s32TempDataLen = 0; 
     
    do
    {
        // 读取数据
        s32TempDataLen  = uartDataRecv(s32UartFileFd, s32ResvDataLen , au8ResvDataBuf); 
        if(s32TempDataLen > 0)
        {
            for ( i = 0; i < s32ResvDataLen; i++ )
            {
                queueIn( g_pstResvDataQueue, au8ResvDataBuf[i]);
#if PRINT_KEY_VALUE
                key_debug(" 0x%x", au8ResvDataBuf[i]);
#endif            
            }
            key_debug("\n");
        }
        else// 未接收到数据时，退出
        {
            break;
        }
    

    }while(1);
}
     

/*******************************************************************************
 函 数 名: ResvDataThread
 描    述: 接收数据线程
 输入参数: 
            参数名称                  参数类型           参数说明
           pvResvData                void*            串口接收的数据  
 输出参数： 无
 返 回 值: 无
 说    明：
*******************************************************************************/
void *ResvDataThread(void*  pvResvData)
{
    // 文件描述符
    static int s32UartFileFd = 0;
    // 接收数据长度
    unsigned int u32RcvBytes = 0;

    static int i = 0; 
    
    /* 初始化uart设备的基本信息 */
    static uartDepolyInfoStru   stUartDepolyInfo = 
    {
	    .s32UartBaundBate = UART_TEST_BAUND_RATE,   // 波特率
	    .s32UartDateBit   = UART_TEST_DATA_BIT,     // 数据位
	    .s8UartCheckBit   = UART_TEST_CHECK_BIT,    // 校验位
	    .s32UartStopBit	  = UART_TEST_STOP_BIT,     // 停止位 
    };


    // 按键板初始化
    s32UartFileFd = keyboardInit(UART_FILE_NAME, stUartDepolyInfo);
     
    while(1)
    {
 
        // 等待信号 > 0
        sem_wait(&g_pstkeyDeal->semUartResv);

        if(0 == g_pstkeyDeal->s32KeyDealFlag)
        {           
            keyboardResv(UART_RESV_DATA_LEN, s32UartFileFd);
            u32RcvBytes = queueLength( g_pstResvDataQueue );           
            if( u32RcvBytes >= UART_RESV_DATA_LEN)
            {
                g_pstkeyDeal->s32KeyDealFlag = 1;
            }

        }  

        // led控制和测试(流水灯)
	
    #if LED_TEST  

    if(i == 0)
    {
        for(int s32Reg = 1; s32Reg < 3; ++s32Reg)
        {
            for(int s32Data = 1; s32Data < 9; s32Data = s32Data * 2 )
            {
                 keyboardSeed(s32UartFileFd, s32Reg, s32Data);
                 sleep(1);
            }
        }

	keyboardSeed(s32UartFileFd, 3, 1);
	sleep(1);
	i = 1;
    }

    #endif
	
        //信号量+1
	    sem_post(&g_pstkeyDeal->semKeyDeal);

	
    }
    
                         
}

