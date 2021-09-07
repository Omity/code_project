#include <stdio.h>
#include <string.h>


typedef struct
{
    char  name[50];
    int   number;
    char  sex[2];
    int   grade;
} s32FinalGrade;

void printGrade(s32FinalGrade *para);


int main()
{
    s32FinalGrade  gradeList;

    strcpy(gradeList.name, "张三");
    gradeList.number  = 0001;
    strcpy(gradeList.sex, "男");
    gradeList.grade   = 95;

    printGrade( &gradeList);
    return 0;

}

void printGrade(s32FinalGrade *para)
{
    printf("姓名： %s\n", para->name);
    printf("学号： %04d\n", para->number);
    printf("性别： %s\n", para->sex);
    printf("成绩： %d\n", para->grade);

    return;
}
