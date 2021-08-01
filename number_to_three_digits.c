#include <stdio.h>


int main()
{
    int i;
    int j;
    int k;
    int arr[4];
    int tmp;
    
    printf("请输入你要使用的四个数字 (0~9):\n");
    for(i = 0; i < 4; ++i)
    {
        scanf("%d[^\n]", &arr[i]);
        if((arr[i] > 9) || (arr[i] < 0))
        {
            printf("无效数字，请重新输入!!!\n");
            scanf("%d[^\n]", &arr[i]);
        }
        for(j = 0; j < i; ++j)
        {
            if(arr[i] == arr[j])
            {
                printf("数字重复，请重新输入!!!\n");
                scanf("%d[^\n]", &arr[i]);
            }
        }
    }
    
    for(i = 0; i < 4; ++i)
    {
        for(j = 0; j < 4; ++j)
        {
            for(k = 0; k < 4; ++k)
            {
                if((j != i) && (k != i) && (k != j))
                {
                    tmp = arr[i] * 100 + arr[j] * 10 + arr[k];
                    printf("%d\n", tmp);
                }/* end of if((j != i) || (k != i) || (k != j)) */
            }/* end of for(k = 0; k < 4; ++k) */
        }/* end of for(j = 0; j < 4; ++j) */
    } /*end of for(i = 0; i < 4; ++i) */
    
    return 0;
}

