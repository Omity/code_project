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


#ifndef UART_QUEUE_H
#define UART_QUEUE_H


#include "uart_phy.h"



#define UART_QUEUE_ERR     -1
#define UART_QUEUE_NO_ERR   0

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  * 函    数：queueInit
  * 描    述：消息队列初始化
  * 输入参数：
  *           参数名称                  参数类型           参数说明
  *           pstResvDataQueue    ResvDataQueueStru    获取的数据队列 
  *           u8QueueLen            u8                 数据队列长度
  * 返 回 值：无
  * 说    明：外部函数
 ******************************************************************************/
void queueInit( ResvDataQueueStru *pstResvDataQueue, unsigned char u8QueueLen );



/*******************************************************************************
  * 函    数：queueIsEmpty
  * 描    述：查询队列是否为空
  * 输入参数：
  *           参数名称                  参数类型           参数说明
  *           pstResvDataQueue    ResvDataQueueStru    获取的数据队列 
  * 返 回 值：队列是否为空
  * 说    明：外部函数
 ******************************************************************************/
int queueIsEmpty( ResvDataQueueStru *pstResvDataQueue);


/*******************************************************************************
  * 函    数：queueIsFull
  * 描    述：查询队列是否已满
  * 输入参数：
  *           参数名称                  参数类型           参数说明
  *           pstResvDataQueue    ResvDataQueueStru    获取的数据队列 
  * 返 回 值：队列是否为满
  * 说    明：外部函数
 ******************************************************************************/
int queueIsFull( ResvDataQueueStru *pstResvDataQueue);


/*******************************************************************************
  * 函    数：queueIn
  * 描    述：向数据队列中添加数据
  * 输入参数：
  *           参数名称                  参数类型           参数说明
  *           pstResvDataQueue    ResvDataQueueStru    获取的数据队列 
  *           u8ResvData               u8             读取数据的指针 
  * 返 回 值：无
  * 说    明：外部函数
 ******************************************************************************/
int queueIn(ResvDataQueueStru *pstResvDataQueue, unsigned char u8ResvData);




/*******************************************************************************
  * 函    数：queueOut
  * 描    述：向数据队列外输出数据
  * 输入参数：
  *           参数名称                  参数类型           参数说明
  *           pstResvDataQueue    ResvDataQueueStru    获取的数据队列 
  *           u8PutData             u8                发送数据的指针
  * 返 回 值：无
  * 说    明：外部函数 
 ******************************************************************************/
int queueOut(ResvDataQueueStru *pstResvDataQueue, unsigned char *pu8PutData);

/*******************************************************************************
  * 函    数：queueClear
  * 描    述：向数据队列外输出数据
  * 输入参数：
  *           参数名称                  参数类型           参数说明
  *           pstResvDataQueue    ResvDataQueueStru    获取的数据队列 
  * 返 回 值：无
  * 说    明：外部函数 
 ******************************************************************************/
void queueClear(ResvDataQueueStru *pstResvDataQueue);

/*******************************************************************************
  * 函    数：queueLength
  * 描    述：获取队列长度
  * 输入参数：
  *           参数名称                  参数类型           参数说明
  *           pstResvDataQueue    ResvDataQueueStru    获取的数据队列 
  * 返 回 值：队列长度
  * 说    明：外部函数 
 ******************************************************************************/
int queueLength(ResvDataQueueStru *pstResvDataQueue);


/*******************************************************************************
  * 函    数：queueDeInit
  * 描    述：删除数据队列
  * 输入参数：
  *           参数名称                  参数类型           参数说明
  *           pstResvDataQueue    ResvDataQueueStru    获取的数据队列 
  * 返 回 值：无
  * 说    明：外部函数 
 ******************************************************************************/
void queueDeInit( ResvDataQueueStru *pstResvDataQueue);

#ifdef __cplusplus

}
#endif

#endif // UART_QUEUE_H

