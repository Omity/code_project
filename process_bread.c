#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <wait.h>
#include <signal.h>

/*******************************************************************************

日期：2021年8月11日
文件名: process_bread.c
文件描述：生产者-消费者:有一个可以存放10个面包的盘子，厨师用来放面包。客人取，盘子空
         了，客人等待;盘子满了，厨师暂时不要生产
主要思路：



*******************************************************************************/



int main()
{
    int count;
    pid_t consumePid;
    int pipefd[2];
    char bread;


    //创建管道
    if(pipe(pipefd) == -1)
    {
        perror("error!");
        exit(0);
    }
    
    bread = '9';
    count = 0;
    consumePid = fork();
  
    if(consumePid < 0)
    {
        printf("vfork error for %d\n", errno);
    }
    else if(consumePid == 0)   //消费者
    {
        usleep(10);
        consume:
        if(read(pipefd[0], &bread, 1) < 0)
        {
            printf("消费者read错误!\n");
        }
        else
        {
            if(atoi(&bread) > 0)
            {
                //bread--;
                count++;
                printf("拿走了一个面包!\n");
                if(write(pipefd[1], "5", 1) < 0)
                { 
                    printf("消费者写入错误!\n");
                }
            }
            else
            {
                printf("面包不足!!!\n");
            } /*end of if(bread > 0) */
        }
        if(count == 1000)
        {
            exit(0);
        }
        else
        {
            sleep(1);
            goto consume;
        } /* end of if(count == 1000) */
 
    }
    else   //生产者
    {
        if(write(pipefd[1], &bread, 1) < 0)
        {
            printf("呵呵好\n");
        }
        product:
        if(read(pipefd[0], &bread, 1) < 0)
        {
            printf("生产者read错误!\n");
        }
        else
        {
            if(atoi(&bread) < 10)
            {
                bread = bread + '1' - '0';
                count++;
                printf("现在有%个面包!\n",bread);
                if(write(pipefd[1], &bread, 1) < 0)
                {
                    printf("生产者写入错误!\n");
                }
            }
        }
        if(count == 1500)
        {
            exit(0);
        }
        else
        {
            sleep(1);
            goto product;
        }
    }
 
    return 0;
    
}


