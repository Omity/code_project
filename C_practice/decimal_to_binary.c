#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*******************************************************************************

日期：2021年7月29日
文件描述：整数转化为二进制
主要思路：对输入数字进行分类，正数，负数和0,以补码呈现,显示为32位，以此为基础。



*******************************************************************************/


int transDecimalToBinary(int num);


int main()
{
    int num;
    printf("请输入要转化的整数: \n");
    scanf("%d", &num);
    transDecimalToBinary(num);

    return 0;
}


int transDecimalToBinary(int num)
{
    int i;
    int tmp[32];
    int count;
    int temp;
    
    memset(tmp, 0, sizeof(int)*32);
    count = 31;
    temp = abs(num);
    
    printf("输入数字为: \n%d\n", num);
    printf("转化为32位二进制为:\n");

    do
    {
        tmp[count] = temp % 2;
        count -= 1;
        temp /= 2;
    }while(temp != 0);

    if(num >= 0)
    {
        for(i = 0; i < 32; ++i)
        {
            printf("%d", tmp[i]);
        }
        printf("\n");
    }
    else
    {
        for(i = 0; i < 32; ++i)
        {
	    if(tmp[i] == 0)
            {
		tmp[i] = 1;
            }
	    else
            {
		tmp[i] = 0;
            }
	}  //取反
        //加1
        tmp[31] += 1;
        for(i = 31; i > 0; --i)
        {
            if(tmp[i] == 2)
            {
                tmp[i] = 0;
                tmp[i - 1] += 1;
            }
            else
            {
            }
        }
        for(i = 0; i < 32; ++i)
        {
            printf("%d", tmp[i]);
        }
        printf("\n");
    }
    
    return 0;
}




