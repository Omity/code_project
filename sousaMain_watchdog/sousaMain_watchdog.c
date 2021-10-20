/******************************************************************************
                      普源精电科技有限公司版权所有(2021-2031)
*******************************************************************************
  源文件名: sousaMain-watchdog.c
  功能描述: 针对SousaMain进程进行监测，进程死掉之后重启应用

  作    者: sn03955
  版    本: V1.00
  完成日期: 2021年10月20日

  修改历史：


******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <time.h>


#include "watchdog.h"

#define  	ZERO        0
#define  	SUCCESS     0
#define  	FAIL        1
#define  	MAX_LOOP    3
#define     FINAL_LOOP  MAX_LOOP-1
#define     SLEEP_S     5
#define  	SOUSAMAIN_PROC     "SousaMain"
#define  	SOUSAMAIN_PATH     "./mnt/app/SousaMain"
#define     LOG_PATH           "/mnt/UDISK/"
#define     LOG_NAME           "sousaMain_restart.log"

#define     DDBUG                                 1
#undef      DDBUG
#if DDBUG
    #define DEBUG_PRINT(fmt, args...) fprintf(stdout, fmt, ##args)
#else
    #define DEBUG_PRINT(fmt, args...)
#endif


int main(int argv, char **argc)
{
	int i;
	int count = 0;
	int ret = SUCCESS;
	
	while(1)
	{
		for(i = 0; i < MAX_LOOP; i++)
		{
			count = i;
			if (SUCCESS == checkProcessExist(SOUSAMAIN_PROC))
			{
				DEBUG_PRINT("%s is working normaolly!\n", SOUSAMAIN_PROC);
				count = i - 1;
				break;     // One success and quit
			}
			sleep(SLEEP_S);
		}
		// if count is FINAL_LOOP, then restart the process
		if(FINAL_LOOP == count)
		{
			ret = restartApp(SOUSAMAIN_PATH);
			while(ret)
			{
				DEBUG_PRINT("restart %s unsuccessfully!\n", SOUSAMAIN_PROC);
				ret = restartApp(SOUSAMAIN_PATH);
			}
			DEBUG_PRINT("restart %s successfully!\n", SOUSAMAIN_PROC);
			logMake(LOG_NAME);
		}
		sleep(1);
	}
}

//查询进程是否存在
int checkProcessExist(char *name)
{
    FILE *pstr;
    char cmd[100];
    char number[10];

    int ret = SUCCESS;

    memset(cmd, 0 , sizeof(cmd));
    sprintf(cmd,"ps | grep %s | grep -v grep | wc -l",name);
    pstr=popen(cmd,"r");

    fgets(number,9, pstr);
    DEBUG_PRINT("number is %d\n",atoi(number));

    if( ZERO == atoi(number))
    {
        DEBUG_PRINT("process - %s not exist!\n",name);
        ret = FAIL;
    }
    else
    {
        DEBUG_PRINT("process - %s exist!\n",name);
        ret = SUCCESS;
    }

    pclose(pstr);
    return ret;

}

//重启进程
int restartApp(char *name)
{
	FILE *fp = NULL;
	char cmd[50];
	
	int ret = SUCCESS;
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "%s &", name);
	fp = popen(cmd, "r");
	if(NULL == fp)
	{
		printf("popen error!\n");
		// try again
		fp = popen(cmd, "r");
		if (NULL == fp)
		{
			// fail again, return fail
			ret = FAIL;
		}
	}
	else
	{
		DEBUG_PRINT("popen success!\n");
		ret = SUCCESS;
	}
	return ret;
}


//log生成
void logMake(char *name)
{
	char msg[] = "SousaMain App restart at "; 
	time_t timep;
	FILE *pFile = NULL;
	char filePath[1024];
	char content[40960];
	
	time(&timep);
	strcpy(filePath, LOG_PATH);
	strcat(filePath, name);
	printf("%s", filePath);
	strcpy(content, msg);
	strcat(content, ctime(&timep));
	printf("%s", content);
	pFile = fopen(filePath, "a+");
	if(NULL == pFile)
	{
		printf("fopen failed!");
		exit(1);
	}
	fseek(pFile, 0, SEEK_END);
	fprintf(pFile,"%s\n", content);
	fflush(pFile);
	
	fclose(pFile);
}




