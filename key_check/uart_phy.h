/*******************************************************************************
                普源精电科技技术有限公司版权所有
********************************************************************************
  源文件名: uart_phy.h
  功能描述: 型号为8250的uart设备测试的头文件
  作    者: YangYuLiang
  版    本: Ver1.0
  创建日期: 2020.03.12
    
  修改历史:
  修改日期：
  版 本 号：
  修 改 人：
  修改内容：
*******************************************************************************/


#ifndef UART_PHY_H
#define UART_PHY_H

#include <pthread.h>
#include <semaphore.h>





#define KEY_DEBUG        1
#define PRINT_KEY_VALUE  1

/* print keyboard value */
#if KEY_DEBUG
#    define key_debug(fmt,...)                       \
        do                                           \
        {                                            \
            printf(fmt,##__VA_ARGS__);               \
        }while(0)
#else
#    define key_debug(fmt,...)
#endif


// 接收串口数据长度
#define UART_RESV_DATA_LEN 10
// 接收数据队列最大个数
#define MAX_ITEM_NUM 255


/* uart串口的配置信息 */
typedef struct uartDepolyInfo
{
	int s32UartBaundBate;  // 波特率
	int s32UartDateBit;    // 数据位 
	char  s8UartCheckBit;  // 校验位
	int s32UartStopBit;    // 停止位
}uartDepolyInfoStru;


typedef struct ResvDataQueue
{
  unsigned char  stResvDataItem[MAX_ITEM_NUM];       // 接受数据队列
  unsigned char   u8QueueLen;                        // 接收队列长度
  unsigned short  u16ItemHead;                       // 数据队列第一个 
  unsigned short  u16ItemTail;                       // 数据队列最后一个

}ResvDataQueueStru;
extern ResvDataQueueStru *g_pstResvDataQueue;

typedef struct keyDeal
{    
    unsigned int  s32KeyDealFlag;   // 是否处理按键的判别值
    sem_t semKeyDeal;               // 处理按键处理的信号量
    sem_t semUartResv;              // 处理数据处理的信号量
}keyDealStru;

extern keyDealStru *g_pstkeyDeal;


#define UART_PHY_ERR     -1
#define UART_PHY_NO_ERR   0
//#define PTHREAD_MUX 

#ifdef __cplusplus
extern "C" {
#endif

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
int uartOpen(const char* ps8PathName);

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
int uartSpecialSetup(int s32UartFileFd, uartDepolyInfoStru  stUartDepolyInfo);




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
int uartSetup(int s32UartFileFd, uartDepolyInfoStru  stUartDepolyInfo);



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
int uartDataRecv(int s32UartFileFd,  const int s32ReadLen, unsigned char *pu8ReadDataBuf);




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
void uartDataSeed(int s32UartFileFd, const int s32WriteLen, unsigned char *pu8WriteDataBuf);

#ifdef __cplusplus

}
#endif

#endif // UART_PHY_H

