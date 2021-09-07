#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>


void waitProcess();


int main()
{
    signal(SIGINT, SIG_IGN);
   
    int i;
    int flag;
    pid_t pid;
    
    flag = 0;
    
    for(i = 0; i < 2; ++i)
    {
        pid = fork();
        
        if(pid < 0)
        {
            printf("fork error for %m\n", errno);
        }
        else if(pid == 0)     //子进程入口
        {
            flag = i + 1;
            
            if(flag == 1)         //B进程
            {
                printf("I\'m B. My pid = %d, my ppid = %d\n",getpid(), getppid());
                break;
            }
            else if(flag = 2)    //C进程
            {
                printf("I\'m C. My pid = %d, my ppid = %d\n",getpid(), getppid());
                exit(1);
            }
        }
    }
    if(flag == 1)
    {
        waitProcess();   //D进程
    }
    else
    {
        wait(NULL);
        printf("I\'m A. My pid = %d, my ppid = %d\n",getpid(), getppid());
        exit(0);
    }
}

void waitProcess()
{
    pid_t pid;
    
    pid = fork();
    
    if(pid < 0)
    {
        printf("fork error for %m\n",errno);
    }
    else if(pid > 0)
    {
        wait(NULL);
    }
    else
    {
        printf("I\'m D. My pid = %d, my ppid = %d\n",getpid(), getppid());
        exit(1);
    }
    
    exit(0);
}


