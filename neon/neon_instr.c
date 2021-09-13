/*******************************************************************************
                普源精电科技技术有限公司版权所有
********************************************************************************
  源文件名: neon_instr.c
  功能描述: neon API test
  作    者: sn03955
  版    本: Ver1.0
  创建日期: 2021.9.10
    
  修改历史:
  修改日期：
  版 本 号：
  修 改 人：
  修改内容：
*******************************************************************************/


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <arm_neon.h>

#define PIXEL            256
#define RGB_SIZE         307200
#define RGB_ARRAY_SIZE   102400
#define ADD_ARRAY_SIZE   16

void api_test(uint8_t *arrA, uint8_t *arrB, uint8_t *arrC);
void rgb_c(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *rgb, int lenColor);
void rgb_neon(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *rgb, int lenColor);
int random_num(uint16_t n, uint8_t *arr, int count);

int main(int argc, char **argv)
{
	int i;
	uint8_t r[RGB_ARRAY_SIZE]       = { 0 };
    uint8_t g[RGB_ARRAY_SIZE]       = { 0 };
    uint8_t b[RGB_ARRAY_SIZE]       = { 0 };
    uint8_t rgb[RGB_ARRAY_SIZE]     = { 0 };
	uint8_t addA[ADD_ARRAY_SIZE]    = { 0 };
	uint8_t addB[ADD_ARRAY_SIZE]    = { 0 };
	uint8_t result[ADD_ARRAY_SIZE]  = { 0 };
    clock_t startTime;
    clock_t endTime;
    double timeUsed;
    
    random_num(PIXEL, rgb, RGB_SIZE);
	random_num(PIXEL, addA, ADD_ARRAY_SIZE);
	random_num(PIXEL, addB, ADD_ARRAY_SIZE);    
	

    startTime = clock();
    rgb_c(r, g, b, rgb, RGB_SIZE / 9);
    endTime = clock();
    timeUsed = (double)(endTime - startTime) / CLOCKS_PER_SEC;
    printf("rgb_c use time: %lf\n", timeUsed);
    
    startTime = clock();
    rgb_neon(r, g, b, rgb, RGB_SIZE / 9);
    endTime = clock();
    timeUsed = (double)(endTime - startTime) / CLOCKS_PER_SEC;
    printf("rgb_neon use time: %lf\n", timeUsed);
    

	//vqaddq test 
	api_test(addA, addB, result);
	for(i = 0; i < ADD_ARRAY_SIZE; i++)
		printf("%d ", addA[i]);
	printf("\n");	
	for(i = 0; i < ADD_ARRAY_SIZE; i++)
		printf("%d ", addB[i]);	
	printf("\n");
	for(i = 0; i < ADD_ARRAY_SIZE; i++)
		printf("%d ", result[i]);
	printf("\n");

	return 0;
}


void api_test(uint8_t *arrA, uint8_t *arrB, uint8_t *arrC)
{
	uint8x16_t a = vld1q_u8(arrA);
	uint8x16_t b = vld1q_u8(arrB);

	vst1q_u8(arrC, vqaddq_u8(a, b));
}


void rgb_c(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *rgb, int lenColor)
{
    int i;
    for(i = 0;i < lenColor; i++)
    {
        r[i] = rgb[3 * i];
        g[i] = rgb[3 * i + 1];
        b[i] = rgb[3 * i + 2];
    }
}

void rgb_neon(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *rgb, int lenColor)
{
    int i;
    int num8x16 = lenColor / 16;
    uint8x16x3_t intlv_rgb;
    for(i = 0; i < num8x16; i++) 
    {
        intlv_rgb = vld3q_u8(rgb + 3 * 16 * i);            //将8位数据每16个一个寄存器，放到连续的3个FPU寄存器
        vst1q_u8(r + 16 * i, intlv_rgb.val[0]);            //将一维向量写入CPU内存中
        vst1q_u8(g + 16 * i, intlv_rgb.val[1]);
        vst1q_u8(b + 16 * i, intlv_rgb.val[2]);
    }
}

int random_num(uint16_t n, uint8_t *arr, int count)
{
    int i;
    uint16_t tmp;
    srand((unsigned)time(NULL));
    for(i = 0; i < count / 3; i++)
    {
	tmp= rand() % n;
        arr[i] = tmp;
    }
    
    return 0;
}
