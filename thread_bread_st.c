#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <wait.h>
#include <signal.h>
#include <pthread.h>


/*******************************************************************************

日期：2021年8月12日
文件名: thread_bread_st.c
文件描述：生产者-消费者:有一个可以存放10个面包的盘子，厨师用来放面包。客人取，盘子空
         了，客人等待;盘子满了，厨师暂时不要生产
主要思路：



*******************************************************************************/


#define BREAD_FULL    10
#define BREAD_EMPTY    0
#define MAX           10

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t notFull = PTHREAD_COND_INITIALIZER;  //队满
pthread_cond_t notEmpty = PTHREAD_COND_INITIALIZER; //队空

int bread = 0;

//函数声明
void* produce(void* arg);
void* consume(void* arg);

int main(int argc, char *argv[])
{
    int i;
    pthread_t proThread;
    pthread_t conThread;
    int proRet;
    int conRet;
    
    while(1)
    {
        pthread_create(&proThread, NULL, produce, NULL);
        pthread_create(&conThread, NULL, consume, NULL);

        pthread_join(proThread, (void**)&proRet);
        pthread_join(conThread, (void**)&conRet);
    }

    return 0;
}

//生产者
void* produce(void* arg)
{
    int tmp;
    pthread_mutex_lock(&mutex);

    if(bread == BREAD_FULL)
    {
        printf("盘子已满,可以休息!\n");
        //pthread_cond_wait(&notFull, &mutex);
    }
    tmp = rand() % (11 - bread);
    bread += tmp;  //product
    printf("已生产%d个面包,现在面包有%d个\n", tmp, bread);
    //发出队非空的消息
    //pthread_cond_signal(&notEmpty);
    pthread_mutex_unlock(&mutex);
   

    return (void*)1;
}
void* consume(void* arg)
{
    int tmp;

    pthread_mutex_lock(&mutex);
    if(bread == BREAD_EMPTY)
    {
        printf("没有面包，赶紧做!\n");
        //等待队不空
        //pthread_cond_wait(&notEmpty, &mutex);
    }
    tmp = rand() % (bread + 1);
    bread -= tmp; //consume
    printf("已消费%d个面包,现在面包有%d个\n", tmp, bread);
    //发出队不满的消息
    //pthread_cond_signal(&notFull);
    pthread_mutex_unlock(&mutex);

    sleep(1);

    return (void*)2;
}
