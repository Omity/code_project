#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <stdlib.h>

/***********************************************************
     功能说明：进程等待wait()方法的应用
    author: linux.sir@qq.com 
***********************************************************/

void parentWaitProcess(int pidNum);

int main(int argc, char * argv[])
{
    int i;
    int flag;
    pid_t pid;

    pid = fork();

    if(pid < 0)
    {
        printf("fork error for %m\n",errno);
    }
    else if(pid > 0)
    { 
        pid = fork();
	if(pid == 0)
        {
            printf("I\'m C. My pid = %d , my parent ppid = %d\n",getpid(),getppid());
	    exit(1);
        }
        sleep(2);
    }
    else
    {
        flag = 1;
        printf("I\'m B. My pid = %d , my parent ppid = %d\n",getpid(),getppid());
    }

    if(flag == 1)
    {
        sleep(1);
        parentWaitProcess(4);
    }
    
    printf("I\'m A. My pid = %d, my ppid = %d\n", getpid(), getppid());

    exit(0);
  
}



void parentWaitProcess(int pidNum)
{
 
    int count = 0; 

    pid_t pid  = fork();
    int status = -1;
   
    if(pid < 0)
    {
        printf("fork error for %m\n",errno);
    }
    else if(pid > 0)
    { 
        wait(&status);//父进程执行到此，马上阻塞自己，直到有子进程结束。当发现有子进程结束时，就会回收它的资源。
    }
    else
    {
        printf("I\'m D. My pid = %d , my parent ppid = %d\n", getpid(), getppid());
	
        exit(pidNum);
     
    } 
    
    exit(0);
    
}




