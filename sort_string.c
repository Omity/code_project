#include <stdio.h>
#include <stdlib.h>

/*******************************************************************************

日期：2021年7月28日
文件描述：利用指针实现字符串排序
主要思路：字符串存入二维数组中，然后以排序的思维对字符串进行排列，不使用strcmp函数



*******************************************************************************/

int myStrCmp (const char *str1,const char *str2);
int stringSorted(char *p[], int num);


int main()
{
    int    count;
    int    i;
    char   string[100][50];
    char*  arrPointer[50];

    printf("请输入单词的数目(不超过20):");
    scanf("%d[^\n]", &count);
    printf("请输入%d个单词:\n", count);
    for(i = 0; i < count; ++i)
    {
        scanf("%s[^\n]", string[i]);
 
    }
    for(i = 0; i < count; ++i)
    {
        arrPointer[i] = string[i];
    }
    
    //打印输入
    printf("输入的单词顺序为:\n");
    for(i = 0; i < count; ++i)
    {
        printf("%s\n", arrPointer[i]);
    }

    //排序并打印输出
    stringSorted(arrPointer, count);
    printf("排序后单词顺序为:\n");
    for(i = 0; i < count; ++i)
    {
        printf("%s\n", arrPointer[i]);
    }

    return 0;
}

//字符串比较
int myStrCmp (const char *str1,const char *str2)
{
    while (*str1 && *str2 && (*str1 == *str2))
    {
        str1++;
	str2++;
    }
    return *str1 - *str2;
}

//字符串排序
int stringSorted(char *p[], int num)
{

    int i, j;
    char* tmp;

    for(i = 0; i < num - 1; ++i)
    {
        for(j = i + 1; j < num; ++j)
        {
            if(myStrCmp(p[i], p[j]) > 0)
            {
                tmp = p[i];
                p[i] = p[j];
                p[j] = tmp;
            }
            else
            {
            }
        }
    }

    return 0;
}

