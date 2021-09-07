/*******************************************************************************
                普源精电科技技术有限公司版权所有
********************************************************************************
  源文件名: uart_data_resv.h
  功能描述: 接收串口的头文件
  作    者: YangYuLiang
  版    本: Ver1.0
  创建日期: 2020.03.12
    
  修改历史:
  修改日期：
  版 本 号：
  修 改 人：
  修改内容：
*******************************************************************************/


#ifndef UART_DATA_DEAL_H
#define UART_DATA_DEAL_H

#include "uart_phy.h"


/**************************** uart相关设备信息 ***************/
#define UART_FILE_NAME "/dev/ttyS7"
#define UART_TEST_BAUND_RATE   750000   // 波特率(750K)
#define UART_TEST_DATA_BIT     8        // 数据位
#define UART_TEST_CHECK_BIT   'N'       // 校验位
#define UART_TEST_STOP_BIT     1        // 停止位


#define UART_RESV_ERR     -1
#define UART_RESV_NO_ERR   1


#define LED_TEST  1

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
  * 函    数：keybRESV名称                  参数类型           参数说明
  *           ps8PathName               char*           设备节点路径  
  *           pstUartDepolyInfo    uartDepolyInfoStru*    配置uart的结构体     
  * 返 回 值：无
  * 说    明：外部函数
 ******************************************************************************/
int keyboardInit(const char* ps8PathName, uartDepolyInfoStru  stUartDepolyInfo);


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
void keyboardResv( const int s32ResvDataLen, int s32UartFileFd);




/*******************************************************************************
  * 函    数：keyboardSeed
  * 描    述：从键盘板串口发送数据
  * 输入参数：
  *           参数名称              参数类型        参数说明
  *           s32UartFileFd extern       s32           文件描述符  
  *           s32WriteReg          s32           发送逻辑的寄存器
  *           s32WriteData         s32           发送数据
  
  * 返 回 值：无
  * 说    明：外部函数
 ******************************************************************************/
void keyboardSeed(int s32UartFileFd, const int s32WriteReg, int s32WriteData);

/****************************extern***************************************************
 函 数 名: ResvDataThread
 描    述: 接收数据线程
 输入参数:
          参数名称              参数类型           参数说明
          pvResvData           void*            串口接收的数据  
 输出参数： 无
 返 回 值: 无
 说    明：
*******************************************************************************/
extern void *ResvDataThread(void*  pvResvData);

#ifdef __cplusplus

}
#endif

#endif //UART_DATA_RESV_H

