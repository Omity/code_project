/*
 * neon_instr.c
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
#include <sys/time.h>
#include <stdlib.h>
#include <arm_neon.h>

#define PIXEL      256
#define RGB_SIZE   1024

void rgb_c(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *rgb, int lenColor);
void rgb_neon(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *rgb, int lenColor);

int main(int argc, char **argv)
{
	uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t rgb;
    struct timeval startTime;
    struct timeval endTime;
    double timeUsed;
    
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



void rgb_c(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *rgb, int lenColor)
{
    for(i = 0;i < lenColor; i++)
    {
        r[i] = rgb[3 * i];
        g[i] = rgb[3 * i + 1];
        b[i] = rgb[3 * i + 2];
    }
}

void rgb_neon(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *rgb, int lenColor)
{
    int num8x16 = lenColor / 16;
    uint8x16x3_t intlv_rgb;
    for (int i=0; i < num8x16; i++) 
    {
        intlv_rgb = vld3q_u8(rgb+3*16*i);   //将8位数据每16个一个寄存器，放到连续的3个寄存器
        vst1q_u8(r+16*i, intlv_rgb.val[0]); //将一维向量写入CPU内存中
        vst1q_u8(g+16*i, intlv_rgb.val[1]);
        vst1q_u8(b+16*i, intlv_rgb.val[2]);
    }
}

int random_num(uint8_t n, uint8_t *arr, int count)
{
    int i;
    srand((unsigned)time(NULL));
    for(i = 0; i < count; i++)
    {
        uint8_t rand = rand() % n;
        rgb[i] = rand;
    }
    
    return 0;
}