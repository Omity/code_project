#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

int main()
{
    int pipefd[2];
    int tmp;
    pid_t pid;
    char bread;
    
    if(pipe(pipefd) == -1)
    {
        printf("创建管道失败!\n");
        exit(1);
    }
    
    bread = '4';
    pid = fork();
    
    if(pid < 0)
    {
         printf("fork error for %d\n", errno);
    }
    else if(pid == 0)  //消费者
    {
        //usleep(10);
        consume:
        if(read(pipefd[0], &bread, 1) < 0)
        {
            printf("消费者读取失败!\n");
        }
        else
        {
            if(bread == 'A')
            {
                bread = '9';
                printf("消费者拿走了一个面包,现在剩下了%c个面包!\n", bread);
                
            }
            else if(atoi(&bread) > 0)
            {
                tmp = bread - '1';
                bread = tmp + '0';
                printf("消费者拿走了一个面包,现在剩下了%c个面包!\n", bread);
            }
            else if(atoi(&bread) == 0)
            {
                printf("面包不足!\n");
            }/* end of if(bread == 'A')) */
            
            if(write(pipefd[1], &bread, 1) < 0)
            {
                printf("消费者写入失败!\n");
            }            
        }/* end of if(read(pipefd[0], &bread, 1) < 0) */
        
        sleep(1);
        goto consume;
    }/* end of else if(pid == 0) */
    else              //生产者
    {
        if(write(pipefd[1], &bread, 1) < 0)
        {
             printf("生产者初始化失败!\n");
        }
        product:
        if(read(pipefd[0], &bread, 1) < 0)
        {
            printf("生产者读入失败!\n");
        }
        else
        {
            if(atoi(&bread) < 10)
            {
                if(bread == '9')
                {
                    bread = 'A';
                }
                else
                {
                    tmp = bread - '1';
                    bread = tmp + '0';
                }/* end of if(bread == '9') */
            }
            if(write(pipefd[1], &bread, 1) < 0)
            {
                printf("生产者写入失败!\n");
            }
        }/* end of if(read(pipefd[0], &bread, 1) < 0) */
        sleep(1);
        goto product;
    }/* end of pid > 0 */
    
    return 0;
}
