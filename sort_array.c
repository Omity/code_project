#include <stdio.h>


/*******************************************************************************

日期：2021年7月28日
文件描述：对输入数字进行升序排列
主要思路：输入数字为浮点型，范围较广一点。



*******************************************************************************/


void sort(float *arr, size_t size);


float main()
{
    int     i;
    size_t  size;
    float   array[10];

    size = sizeof(array) / sizeof(float);
    for(i = 0; i < 10; ++i)
    {
        scanf("%f", &array[i]);
    }
    sort(array, size);
    for(i = 0; i < size; ++i)
    {
        printf("%.2f ", array[i]);    //精度输出小数点后2位
    }
    printf("\n");
    return 0;
}

//升序排列
void sort(float *arr, size_t size)
{
    int   i;
    int   j;
    float tmp;
    for(i = 0; i < size - 1; ++i)
    {
        for(j = i + 1; j < size; ++j)
        {
            if(0 < (arr[j] - arr[i]) < 1E-6)
	    {
		tmp = arr[i];
		arr[i] = arr[j];
		arr[j] = tmp;
	    }
	    else
	    { 
	    }
        }
    }
}
