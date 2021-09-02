#include <stdio.h>


double bonusSorted(double profit, double *bonus);


int main()
{
    double profit;
    double *pDouble;
    
    printf("请输入本月利润:\n");
    scanf("%lf[^\n]",  &profit);

    bonusSorted(profit, pDouble);
    
    printf("奖金为: %lf\n", *pDouble);
    
    return 0;
}

double bonusSorted(double profit, double *bonus)
{
    double bonusTen;
    double bonusTwenty;
    double bonusForty;
    double bonusSixty;
    double bonusOneMillion;
    
    bonusTen        = 100000 * 0.1;
    bonusTwenty     = bonusTen + 100000 * 0.075;
    bonusForty      = bonusTwenty + 200000 * 0.05;
    bonusSixty      = bonusForty + 200000 * 0.03;
    bonusOneMillion = bonusSixty + 400000 * 0.015;
    
    if(profit <= 100000)
    {
        *bonus = profit * 0.1;
    }
    else if(profit <= 200000)
    {
        *bonus = bonusTen + (profit - 100000) * 0.075;
    }
    else if(profit <= 400000)
    {
        *bonus = bonusTwenty + (profit - 200000) * 0.05;
    }
    else if(profit <= 600000)
    {
        *bonus = bonusForty + (profit - 400000) * 0.03;
    }
    else if(profit <= 1000000)
    {
        *bonus = bonusSixty + (profit - 600000) * 0.075;
    }
    else
    {
        *bonus = bonusOneMillion + (profit- 1000000) * 0.015;
    }
    
    return 0;
}
