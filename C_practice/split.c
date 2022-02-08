#include <stdio.h>
#include <string.h>

//该函数用于实现python里面对字符串的拆分

/*
 * 
 * name: split
 * @param   src 源字符串的首地址(buf的地址) 
 *		    separator 指定的分割字符
 *		    dest 接收子字符串的数组
 *		    num 分割后子字符串的个数
 * @return  无
 * 
 */

void split(char *src,const char *separator,char **dest,int *num) 
{
     char *pNext;
     int count = 0;
     if (NULL == src || strlen(src) == 0) //如果传入的地址为空或长度为0，直接终止 
        return;
     if (NULL == separator || strlen(separator) == 0) //如未指定分割的字符串，直接终止 
        return;
     pNext = (char *)strtok(src,separator); //必须使用(char *)进行强制类型转换(虽然不写有的编译器中不会出现指针错误)
     while(NULL != pNext ) {
          *dest++ = pNext;
          ++count;
         pNext = (char *)strtok(NULL,separator);  //必须使用(char *)进行强制类型转换
    }  
    *num = count;
}
