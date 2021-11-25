#ifndef _DRAW_PROCESSBAR_H
#define _DRAW_PROCESSBAR_H

//#include <common.h>
//#include <malloc.h>
//#include <sunxi_display2.h>
//#include <sunxi_board.h>

#include <linux/delay.h>
#include <linux/init.h>
#include <linux/compiler.h>
#include <asm/cacheflush.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <asm/uaccess.h>
#include <linux/fb.h>

typedef struct
{
	int          screen_width;
	int          screen_height;
	int          screen_size;
	unsigned int color;
	char         *screen_buf;
}screen_info;


typedef struct 
{
	int  x1;					//左上角x坐标
	int  y1;					//左上角y坐标
	int  x2;					//右下角x坐标
	int  y2;					//右下角y坐标
	int  st_x;					//内部起始点的x坐标
	int  st_y;					//内部起始点的y坐标
	int  pr_x;					//当前进度所在的x坐标
	int  pr_y;					//无效
	int  thick;					//框的厚度，边缘厚度
	int  width;					//整体宽度
	int  height;				//整体高度
	int  frame_color;			//边框颜色
	int  progress_color;		//内部颜色
	int  progress_ratio;		//当前进度百分比
	/*0:left to right*/
	/*1:right to left*/
	/*2:up to down*/
	/*3:down to up*/
	int direction_option;
	
	void *kmalloc_addr;
}processbar_t;

#define RIGOL_COLOR_GREEN         0xff00FF00
#define RIGOL_COLOR_RED           0xffFF0000
#define RIGOL_COLOR_BLUE		  0xff0000ff
#define RIGOL_COLOR_CYAN          0xff00FFFF
#define RIGOL_COLOR_MAGENTA       0xffFF00FF
#define RIGOL_COLOR_YELLOW        0xffFFFF00
#define RIGOL_COLOR_LIGHTBLUE     0xff8080FF
#define RIGOL_COLOR_LIGHTGREEN    0xff80FF80
#define RIGOL_COLOR_LIGHTRED      0xffFF8080
#define RIGOL_COLOR_LIGHTCYAN     0xff80FFFF
#define RIGOL_COLOR_LIGHTMAGENTA  0xffFF80FF
#define RIGOL_COLOR_LIGHTYELLOW   0xffFFFF80
#define RIGOL_COLOR_DARKBLUE      0xff000080
#define RIGOL_COLOR_DARKGREEN     0xff008000
#define RIGOL_COLOR_DARKRED       0xff800000
#define RIGOL_COLOR_DARKCYAN      0xff008080
#define RIGOL_COLOR_DARKMAGENTA   0xff800080
#define RIGOL_COLOR_DARKYELLOW    0xff808000
#define RIGOL_COLOR_WHITE         0xffFFFFFF
#define RIGOL_COLOR_LIGHTGRAY     0xffD3D3D3
#define RIGOL_COLOR_GRAY          0xff808080
#define RIGOL_COLOR_DARKGRAY      0xff404040
#define RIGOL_COLOR_BLACK         0xff000000


void ui_set_color(unsigned int color);
int ui_get_color(void);
int draw_hollowrectangle(int x1, int y1, int x2, int y2);
int draw_solidrectangle(int x1, int y1, int x2, int y2);
int progressbar_create(int x1, int y1, int x2, int y2, int op);
int progressbar_config(int frame_color, int progress_color, int thickness);
int progressbar_active(void);
int progressbar_upgrate(int rate);
int screen_set(screen_info *screen_info_stu, char *framebuffer);
static void progressbar_run(struct work_struct *work);
static void progressbar_exit(void);

#endif
