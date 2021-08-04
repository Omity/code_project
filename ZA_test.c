#include <stdio.h>


int strcmp (const char *str1,const char *str2)
{
    while (*str1 && *str2 && (*str1 == *str2))
    {
        str1++;
	str2++;
    }
    return *str1 - *str2;
}

char trans(char *p[], int num)
{

    int i, j;
    char* tmp;

    for(i = 0; i < num - 1; ++i)
    {
        for(j = i + 1; j < num; ++j)
        {
            if(strcmp(p[i], p[j]) > 0)
            {
                tmp = p[i];
                p[i] = p[j];
                p[j] = tmp;
            }
            else
            {
                printf("%s\n", "不变");
            }
        }
    }

    return 0;
}


int main()
{
    int i;
    char aa[2][10];
    char* a[10];
    for(i = 0; i < 2; i++)
    {
        scanf("%s", aa[i]);
    }
    for(i = 0; i < 2; i++)
    {
        a[i] = aa[i];
    }
    trans(a, 2);
    for(i = 0; i < 2; ++i)
        printf("%s", a[i]);
}
