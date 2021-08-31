#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "randname.h"

/*
char randName()
{
    
}
*/

enum nametype nameType;



char main()
{
    int i;
    int *sex;
    char* name;
    srand((unsigned) time( NULL));
    printf("%s\t\t%s\t%s\t%s\n", "姓名", "性别", "学号", "成绩");
    for(i = 0; i < 1000; ++i)
    {
        name = getRandomName(getRandomValue, sex);
        if(*sex == 0)
        {
            if(strlen(name) == 12)
            {
                printf("%s\t%s\t%04d\t%d\n", name, "男", i, getRandomValue() % 100);
            }
            else
            {
                printf("%s\t\t%s\t%04d\t%d\n", name, "男", i, getRandomValue() % 100);
            }
        }
        else
        { 
            if(strlen(name) == 12)
            {
                printf("%s\t%s\t%04d\t%d\n", name, "女", i, getRandomValue() % 100);
            }
            else
            {
                printf("%s\t\t%s\t%04d\t%d\n", name, "女", i, getRandomValue() % 100);
            }
        }
        free(name);
    }
    return 0;
}

int getRandomValue(void)
{
    int randomValue;
    randomValue = rand();
    return randomValue;
}


char *getRandomName(int (*getRandom)(void), int *sex)
{
    int randomRoll;
    int lastNameRoll;
    int firstNameRoll;
    char* name;    
    
    name = (char *)malloc(sizeof(char)*50);
    randomRoll = getRandom() % 7;
    nameType   = (enum nametype) randomRoll;
    
    switch(nameType)
    {
        case man_shan_single:                           //男性，单名单姓
            lastNameRoll   = getRandom() % 444;
            firstNameRoll  = getRandom() % 160;
            strcpy(name, NA1[lastNameRoll]);
            strcat(name, ME1m[firstNameRoll]);

            *sex = 0;
            break;

        case man_shan_double:                           //男性，双名单姓
            lastNameRoll   = getRandom() % 444;
            firstNameRoll  = getRandom() % 160;
            strcpy(name, NA1[lastNameRoll]);
            strcat(name, ME1m[firstNameRoll]);
            delay_ms(5);
            firstNameRoll  = getRandom() % 160;
            strcat(name, ME1m[firstNameRoll]); 

            *sex = 0;
            break;

        case man_comp_single:                           //男性，单名复姓
            lastNameRoll   = getRandom() % 59;
            firstNameRoll  = getRandom() % 160;
            strcpy(name, NA2[lastNameRoll]);
            strcat(name, ME1m[firstNameRoll]);

            *sex = 0;
            break;

        case man_comp_double:                           //男性，双名复姓
            lastNameRoll   = getRandom() % 59;
            firstNameRoll  = getRandom() % 160;
            strcpy(name, NA2[lastNameRoll]);
            strcat(name, ME1m[firstNameRoll]);
            delay_ms(5);
            firstNameRoll  = getRandom() % 160;
            strcat(name, ME1m[firstNameRoll]);

            *sex = 0; 
            break;

        case woman_shan_single:                           //女性，单名单姓
            lastNameRoll   = getRandom() % 444;
            firstNameRoll  = getRandom() % 165;
            strcpy(name, NA1[lastNameRoll]);
            strcat(name, ME1f[firstNameRoll]);

            *sex = 1;
            break;

        case woman_shan_double:                           //女性，双名单姓
            lastNameRoll   = getRandom() % 444;
            firstNameRoll  = getRandom() % 165;
            strcpy(name, NA1[lastNameRoll]);
            strcat(name, ME1f[firstNameRoll]);
            delay_ms(5);
            firstNameRoll  = getRandom() % 165;
            strcat(name, ME1f[firstNameRoll]);

            *sex = 1;
            break;
        case woman_comp_single:                           //女性，单名复姓
            lastNameRoll   = getRandom() % 59;
            firstNameRoll  = getRandom() % 165;
            strcpy(name, NA2[lastNameRoll]);
            strcat(name, ME1f[firstNameRoll]);

            *sex = 1;
            break;

        case woman_comp_double:                           //女性，双名复姓
            lastNameRoll   = getRandom() % 59;
            firstNameRoll  = getRandom() % 165;
            strcpy(name, NA2[lastNameRoll]);
            strcat(name, ME1f[firstNameRoll]);
            delay_ms(5);
            firstNameRoll  = getRandom() % 165;
            strcat(name, ME1f[firstNameRoll]); 

            *sex = 1;
            break;

        default:
            break;
    }
    //printf("%s\n", name);

    return name;
    
}

float delay_ms(int num)
{
    int i;
    int j;
    for(i = 0; i < num; ++i)
    {
         for(j = 0; j < num; ++j)
         {
         }
    }
}

