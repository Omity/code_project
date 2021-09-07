/*******************************************************************************
                普源精电科技技术有限公司版权所有
********************************************************************************
  源文件名: uart_queue.c
  功能描述: 串口数据队列
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
// 用户定义头文件
#include "uart_queue.h"

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
void queueInit( ResvDataQueueStru *pstResvDataQueue, unsigned char u8QueueLen )
{
    // 初始化队列值
    memset(pstResvDataQueue->stResvDataItem, 0 , u8QueueLen);
    // 使队列头尾相连
    pstResvDataQueue->u16ItemHead = 0;
    pstResvDataQueue->u16ItemTail = 0;
    // 设置队列长度
    pstResvDataQueue->u8QueueLen = u8QueueLen;
}


/*******************************************************************************
  * 函    数：queueIsEmpty
  * 描    述：查询队列是否为空
  * 输入参数：
  *           参数名称                  参数类型           参数说明
  *           pstResvDataQueue    ResvDataQueueStru    获取的数据队列 
  * 返 回 值：队列是否为空
  * 说    明：外部函数
 ******************************************************************************/
int queueIsEmpty( ResvDataQueueStru *pstResvDataQueue)
{
    // 判断队列头尾是否相连
    return pstResvDataQueue->u16ItemHead == pstResvDataQueue->u16ItemTail;
}


/*******************************************************************************
  * 函    数：queueIsFull
  * 描    述：查询队列是否已满
  * 输入参数：
  *           参数名称                  参数类型           参数说明
  *           pstResvDataQueue    ResvDataQueueStru    获取的数据队列 
  * 返 回 值：队列是否为满
  * 说    明：外部函数
 ******************************************************************************/
int queueIsFull( ResvDataQueueStru *pstResvDataQueue)
{
    // 如果一组数据的正好尾部的位置后面一个位置是首位置，表示数组填充满
    return ( ((pstResvDataQueue->u16ItemTail + 1) % pstResvDataQueue->u8QueueLen) == pstResvDataQueue->u16ItemHead );
}

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
int queueIn(ResvDataQueueStru *pstResvDataQueue, unsigned char u8ResvData)
{
    //如果队列已满，不插入数据
    if(1 == queueIsFull(pstResvDataQueue) )
    {
        return UART_QUEUE_ERR;
    }
    else
    {
       // 设置队列尾部数据
       pstResvDataQueue->stResvDataItem[pstResvDataQueue->u16ItemTail] =  u8ResvData;
       // 尾部位置+1
       pstResvDataQueue->u16ItemTail = (pstResvDataQueue->u16ItemTail + 1) % pstResvDataQueue->u8QueueLen;
    }
    return UART_QUEUE_NO_ERR;
}


/*******************************************************************************
  * 函    数：queueOut
  * 描    述：向数据队列外输出数据
  * 输入参数：
  *           参数名称                  参数类型           参数说明
  *           pstResvDataQueue    ResvDataQueueStru    获取的数据队列 
  *           pu8PutData             u8*               发送数据的指针
  * 返 回 值：无
  * 说    明：外部函数 
 ******************************************************************************/
int queueOut(ResvDataQueueStru *pstResvDataQueue, unsigned char *pu8PutData)
{
    // 如果队列是空，不取出数据
    if(1== queueIsEmpty(pstResvDataQueue) )
    {
        return UART_QUEUE_ERR;
    }
    else
    {
       *pu8PutData =  pstResvDataQueue->stResvDataItem[pstResvDataQueue->u16ItemHead];  
       pstResvDataQueue->u16ItemHead = (pstResvDataQueue->u16ItemHead + 1) % pstResvDataQueue->u8QueueLen;
    }
    return UART_QUEUE_NO_ERR;
}

/*******************************************************************************
  * 函    数：queueClear
  * 描    述：向数据队列外输出数据
  * 输入参数：
  *           参数名称                  参数类型           参数说明
  *           pstResvDataQueue    ResvDataQueueStru    获取的数据队列 
  * 返 回 值：无
  * 说    明：外部函数 
 ******************************************************************************/
void queueClear(ResvDataQueueStru *pstResvDataQueue)
{
    // 清空队列数据
    memset(pstResvDataQueue->stResvDataItem, 0 , pstResvDataQueue->u8QueueLen);
    pstResvDataQueue->u16ItemHead = 0;
    pstResvDataQueue->u16ItemTail = 0;
}


/*******************************************************************************
  * 函    数：queueLength
  * 描    述：获取队列长度
  * 输入参数：
  *           参数名称                  参数类型           参数说明
  *           pstResvDataQueue    ResvDataQueueStru    获取的数据队列 
  * 返 回 值：队列长度
  * 说    明：外部函数 
 ******************************************************************************/
int queueLength(ResvDataQueueStru *pstResvDataQueue)
{
    int s32DataLength;
    // 队列长度 = （队列尾 - 队列头位置 + 队列总长） % 队列总长
    s32DataLength = (pstResvDataQueue->u16ItemTail + pstResvDataQueue->u8QueueLen - pstResvDataQueue->u16ItemHead ) 
            % pstResvDataQueue->u8QueueLen;

    return s32DataLength;
}


/*******************************************************************************
  * 函    数：queueDeInit
  * 描    述：删除数据队列
  * 输入参数：
  *           参数名称                  参数类型           参数说明
  *           pstResvDataQueue    ResvDataQueueStru    获取的数据队列 
  * 返 回 值：无
  * 说    明：外部函数 
 ******************************************************************************/
void queueDeInit( ResvDataQueueStru *pstResvDataQueue)
{
    // 清空队列数据
    memset(pstResvDataQueue->stResvDataItem, 0 , pstResvDataQueue->u8QueueLen);
    pstResvDataQueue->u16ItemHead = 0;
    pstResvDataQueue->u16ItemTail = 0;
    pstResvDataQueue->u8QueueLen = 0;
}








