/*
 * neon_sin.c
 * 
 * Copyright 2021 Ubuntu-64-Develop <sn02241@ubuntu>
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
#include <arm_neon.h>
#include <sys/time.h>

#define EPSILON     1e-6


int cSin(float x)
{
    int i;
    int s;
    float sum = 0;
    float mole;   //分子
    float deno;   // 分母
    
    mole = x;
    deno = 1;
    s = 1;
    for(i = 1; (mole / deno) >= EPSILON; i++)
    {
        sum  = sum + s * (mole / deno);
        mole = mole * x * x;
        deno = deno * 2 * i * (2 * i + 1);
        s *= -1;
    }
    
    printf("sin(%f) ≈ %f\n", x, sum);
    return 0;
}


int neonSin(float x)
{
        int i;
    int s;
    float sum = 0;
    float mole;   //分子
    float deno;   // 分母
    
    mole = x;
    deno = 1;
    s = 1;
    kernel_neon_begin();
    for(i = 1; (mole / deno) >= EPSILON; i++)
    {
        sum  = sum + s * (mole / deno);
        mole = mole * x * x;
        deno = deno * 2 * i * (2 * i + 1);
        s *= -1;
    }
    kernel_neon_end();
    
    printf("sin(%f) ≈ %f\n", x, sum);
}


int main(int argc, char **argv)
{
	float data;
    struct timeval startTime;
    struct timeval endTime;
    double timeUsed;
    scanf("x waiting to calc: %f", &data);
    gettimeofday(&startTime, NULL);
    cSin(data);
    gettimeofday(&endTime, NULL);
    timUsed = (startTime.tv_sec - endTime.tv_sec) + 
              (double)(startTime.tv_usec - endTime.tv_usec) / 1000000.0;
    printf("cSin use time: lf\n", timeUsed);
    
    gettimeofday(&startTime, NULL);
    neonSin(data);
    gettimeofday(&endTime, NULL);
    timUsed = (startTime.tv_sec - endTime.tv_sec) + 
              (double)(startTime.tv_usec - endTime.tv_usec) / 1000000.0;
    printf("neonSin use time: lf\n", timeUsed);
	return 0;
}




