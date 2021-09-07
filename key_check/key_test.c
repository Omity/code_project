/*******************************************************************************
                普源精电科技技术有限公司版权所有
********************************************************************************
  源文件名: main.c
  功能描述: 主程序
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
#include <string.h>

#include "key_phare.h"
#include "uart_data_deal.h"
#include "uart_queue.h"


/**************************** 宏定义 ***************/
#define UART_MAIN_ERR    -1
#define UART_MAIN_NO_ERR  0
 

int main()
{
    // 函数返回值
    int s32RetValue = 0;
    // 接收数据的线程
    pthread_t  stResvDataThread;
    // 解析数据的线程
    pthread_t  stPhareDataThread;


    // 申请按键处理的结构体空间
    g_pstkeyDeal = (keyDealStru *)malloc( sizeof(keyDealStru) );
    if(NULL == g_pstkeyDeal)
    {
       perror("Unable to malloc key deal space ");
	   return UART_MAIN_ERR;
    }


    // 申请接受数据的结构体空间
    g_pstResvDataQueue = (ResvDataQueueStru *)malloc( sizeof(ResvDataQueueStru) );
    if(NULL == g_pstResvDataQueue)
    {
       perror("Unable to malloc Resv data queue ");
	   return UART_MAIN_ERR;
    }


    //初始化信号量
	sem_init(&g_pstkeyDeal->semKeyDeal, 0, 0);
    //先运行resv
    sem_init(&g_pstkeyDeal->semUartResv, 0, 1); 


    // 变量初始化
    g_pstkeyDeal->s32KeyDealFlag = 0;
    // 初始化队列
    queueInit( g_pstResvDataQueue, MAX_ITEM_NUM );
    
    // 运行接收数据的线程
    s32RetValue = pthread_create(&stResvDataThread,NULL, ResvDataThread, NULL);
    if(s32RetValue != 0)
    {
        perror("Unable to cread resv data thread ");
        pthread_exit(NULL);
		return UART_MAIN_ERR;
    }    

    // 运行解析按键值的线程
    s32RetValue = pthread_create(&stPhareDataThread, NULL, PhareDataThread, NULL);
    if(s32RetValue != 0)
    {
        perror("Unable to cread phare data thread ");
        pthread_exit(NULL);
		return UART_MAIN_ERR;
    }


    printf("==============  key Thread  start============== \n");
    // 等待线程完成
    pthread_join(stResvDataThread, NULL);
	pthread_join(stPhareDataThread, NULL); 
 
    // 销毁信号量
	sem_destroy(&g_pstkeyDeal->semKeyDeal);
	sem_destroy(&g_pstkeyDeal->semUartResv);

    // 清除数据队列
    queueDeInit( g_pstResvDataQueue);
    while(1); 
    
}




