/*
 * process_test.c
 * 
 * Copyright 2021 rigol <SJ@ubuntu>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	pid_t pid;
	int result = -1;
	int fd[2], nbytes;
	char string[] = "你好，管道";//对string数组初始化
	char readBuf[80];
	int *write_fd = &fd[1];
	int *read_fd = &fd[0];
	printf("Please input data:");
	scanf("%s",string);
	result = pipe(fd);
	if(-1 == result)
	{
		printf("建立管道失败！\n");
		return -1;
	}

	pid = fork();
	if(-1 == pid) //此处为了验证父子进程是否创建成功，如果未创建成功，则返回-1
	{
		printf("进程创建失败!\n");
		return -1;
		}
	else if(0 == pid)
	{
		close(*read_fd);
		result = write(*write_fd,string,strlen(string));
		return 0;
	}
	else
	{
		close(*write_fd);
		nbytes = read(*read_fd,readBuf,sizeof(readBuf)-1);
		printf("接收了%d个数据，内容为：\"%s\"\n",nbytes,readBuf);
	}
	return 0;
}

