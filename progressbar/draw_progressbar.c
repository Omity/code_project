/** 文件描述: draw_progressbar.c
 *  功能描述: 用于实现kernel阶段的进度条, 目前未实现区分升级和正常开机
 */

/** 遗留问题: 目前关于颜色显示的配置存在问题 */

#include "draw_progressbar.h"


static screen_info g_screen_source_stu;
static processbar_t g_processbar_stu;
static struct delayed_work progressbar_work;
static struct workqueue_struct *mywork;

//设置颜色
static void ui_set_color(unsigned int color)
{
	g_screen_source_stu.color = color;
}

//获取颜色
static int ui_get_color(void)
{
	return g_screen_source_stu.color;
}

//画进度条边框
static int draw_hollowrectangle(screen_info *screen_stu, int x1, int y1, int x2, int y2)
{
	screen_info *p_screen_stu = screen_stu;
	int   end_x, end_y;
	int   start_x, start_y;
	char *base1, *base2;
	int   x, y, tmp;
	int   line_offset;
	uint  cache_start;

	end_x   = x1;
	end_y   = y1;
	start_x = x2;
	start_y = y2;

	//确保起点比较小，方便计算
	if(start_y > end_y)
	{
		tmp 	= start_y;
		start_y = end_y;
		end_y   = tmp;
	}
	if(start_x > end_x)
	{
		tmp 	= start_x;
		start_x = end_x;
		end_x   = tmp;
	}
	//开始绘线
	base1  = p_screen_stu->screen_buf + (p_screen_stu->screen_width * start_y + start_x)* 4;
	base2  = base1 + (end_y - start_y) * p_screen_stu->screen_width * 4;
	cache_start = (uint)base1;
	for(x=start_x;x<=end_x;x++)
	{
		*((int *)base1) = p_screen_stu->color;
		*((int *)base2) = p_screen_stu->color;
		base1 += 4;
		base2 += 4;
	}
	base1  = p_screen_stu->screen_buf + (p_screen_stu->screen_width * start_y + start_x)* 4;
	base2  = base1 + (end_x - start_x) * 4;
	line_offset = p_screen_stu->screen_width * 4;
	for(y=start_y;y<=end_y;y++)
	{
		*((int *)base1) = p_screen_stu->color;
		*((int *)base2) = p_screen_stu->color;
		base1 += line_offset;
		base2 += line_offset;
	}

	//flush_cache((uint)cache_start, p_screen_stu->screen_width * p_screen_stu->screen_height * 4);
	flush_cache_all();

	return 0;
}

//填充进度条
static int draw_solidrectangle(screen_info *screen_stu, int x1, int y1, int x2, int y2)
{
	screen_info *p_screen_stu = screen_stu;
	int   end_x, end_y;
	int   start_x, start_y;
	char *base1, *base2;
	char *base1_1, *base2_1;
	int   x, y, tmp;
	uint  cache_start;

	end_x   = x1;
	end_y   = y1;
	start_x = x2;
	start_y = y2;

	//确保起点比较小，方便计算
	if(start_y > end_y)
	{
		tmp 	= start_y;
		start_y = end_y;
		end_y   = tmp;
	}
	if(start_x > end_x)
	{
		tmp 	= start_x;
		start_x = end_x;
		end_x   = tmp;
	}

	base1 = p_screen_stu->screen_buf + (p_screen_stu->screen_width * start_y + start_x) * 4;
	base2 = base1 + p_screen_stu->screen_width * (end_y - start_y) * 4;
	base1_1 = base1;
	base2_1 = base2;
	cache_start = (uint)base1;
	for(x=end_x;x>=start_x;x--)
	{
		for(y=end_y;y>=start_y;y-=2)
		{
			*((int *)base1_1) = p_screen_stu->color;
			*((int *)base2_1) = p_screen_stu->color;
			base1_1 += p_screen_stu->screen_width * 4;
			base2_1 -= p_screen_stu->screen_width * 4;
		}
		base1 += 4;
		base2 += 4;
		base1_1 = base1;
		base2_1 = base2;
	}
	//flush_cache(cache_start, (uint)base2 - cache_start);
	flush_cache_all();

	return 0;
}

//初始化关于进度条的属性
static int progressbar_create(processbar_t *progressbar_stu, int x1, int y1, int x2, int y2, int op)
{
	processbar_t *progress = progressbar_stu;
	int tmp;
	//progress = kmalloc(sizeof(processbar_t), GFP_KERNEL);
	//progress->kmalloc_addr = progress;
	//if(!progress)
	//{
		//printk("sprite cartoon ui: create progressbar failed\n");

		//return 0;
	//}
	progress->direction_option = op;
	if(x1 > x2)
	{
		tmp = x1;
		x1 	= x2;
		x2  = tmp;
	}
	if(y1 > y2)
	{
		tmp = y1;
		y1 	= y2;
		y2  = tmp;
	}
	progress->x1 = x1;
	progress->x2 = x2;
	progress->y1 = y1;
	progress->y2 = y2;
	progress->width  = x2 - x1;
	progress->height = y2 - y1;
	//配置默认属性，防止没有调用配置函数而属性缺失
	progress->st_x	 = progress->pr_x = x1 + 1;
	progress->st_y	 = progress->pr_y = y1 + 1;
	progress->frame_color 	 = RIGOL_COLOR_WHITE;
	progress->progress_color = RIGOL_COLOR_GREEN;
	progress->progress_ratio = 0;
	progress->thick          = 1;
	progress->frame_color 	 = RIGOL_COLOR_GREEN;
	progress->progress_color = RIGOL_COLOR_RED;
	
	return 0;
}

//配置进度条
static int progressbar_config(processbar_t *progressbar_stu, int frame_color, int progress_color, int thickness)
{
	processbar_t *progress = progressbar_stu;

	//if(!p)
	//{
		//return -1;
	//}
	progress->frame_color 	 = frame_color;
	progress->progress_color = progress_color;
	progress->progress_ratio = 0;
	progress->thick          = thickness;
	switch (progress->direction_option) {
	case 0:
		progress->st_x = progress->pr_x = progress->x1 + thickness;
		progress->st_y = progress->pr_y = progress->y1 + thickness;
		break;
	case 1:
		progress->st_x = progress->pr_x = progress->x2 + thickness;
		progress->st_y = progress->pr_y = progress->y1 + thickness;
		break;
	case 2:
		progress->st_x = progress->pr_x = progress->x1 + thickness;
		progress->st_y = progress->pr_y = progress->y1 + thickness;
		break;
	case 3:
		progress->st_x = progress->pr_x = progress->x1 + thickness;
		progress->st_y = progress->pr_y = progress->y2 + thickness;
		break;
	default:
		break;
	}

	return 0;
}

//设置属性并画出边框
static int progressbar_active(screen_info *screen_stu, processbar_t *progressbar_stu)
{
	processbar_t *progress = progressbar_stu;
	screen_info *p_screen_stu = screen_stu;
	int base_color;
	int i;

	base_color = ui_get_color();
	ui_set_color(progress->frame_color);
	for(i=0;i<progress->thick;i++)
	{
	    draw_hollowrectangle(p_screen_stu, progress->x1+i, progress->y1+i, progress->x2-i, progress->y2-i);
	}
	ui_set_color(base_color);

	return 0;
}

static int progressbar_upgrate(screen_info *screen_stu, processbar_t *progressbar_stu, int rate)
{
	processbar_t *progress = progressbar_stu;
	screen_info *p_screen_stu = screen_stu;
	int base_color, progresscolor;
	int pixel;
	int x1 = 0, y1 = 0;
	int x2 = 0, y2 = 0;

	if((rate < 0) || (rate > 100))
	{
		printk("ui progressbar: invalid progressbar rate\n");
		return -1;
	}

	if (progress->direction_option <= 1)
		pixel = (rate * (progress->width - progress->thick*2)/100);
	else
		pixel = (rate * (progress->height - progress->thick*2)/100);
	if(rate == progress->progress_ratio)
	{
		return 0;
	}
	else
	{
		if (progress->direction_option <= 1) {
			x1 = progress->pr_x;
			if (progress->direction_option % 2)
				x2 = progress->st_x - pixel;
			else
				x2 = progress->st_x + pixel;
			progresscolor = (rate > progress->progress_ratio)
					    ? (progress->progress_color)
					    : (RIGOL_COLOR_BLACK);
			progress->pr_x  = x2;
			progress->progress_ratio = rate;
		} else {
			y1 = progress->pr_y;
			if (progress->direction_option % 2)
				y2 = progress->st_y - pixel;
			else
				y2 = progress->st_y + pixel;

			progresscolor = (rate > progress->progress_ratio)
					    ? (progress->progress_color)
					    : (RIGOL_COLOR_BLACK);
			progress->pr_y  = y2;
			progress->progress_ratio = rate;
		}

	}
	base_color = ui_get_color();
	ui_set_color(progresscolor);

	if (progress->direction_option <= 1) {
		y1 = progress->y1 + progress->thick;
		y2 = progress->y2 - progress->thick;
	} else {
		x1 = progress->x1 + progress->thick;
		x2 = progress->x2 - progress->thick;
	}


	draw_solidrectangle(p_screen_stu, x1, y1, x2, y2);

	ui_set_color(base_color);

	return 0;
}

//填充结构体
static int screen_set(screen_info *screen_info_stu, char *framebuffer)
{
	screen_info *screen_source_stu = screen_info_stu;
	/* 初始化图形参数 */
	screen_source_stu->screen_width  = SCREEN_LCD_WIDTH; 
	screen_source_stu->screen_height = SCREEN_LCD_HEIGHT;

	if((screen_source_stu->screen_width < 40) || (screen_source_stu->screen_height < 40))
	{
		printk("sunxi cartoon error: invalid screen width or height\n");

		return -1;
	}
	
	screen_source_stu->screen_size = screen_source_stu->screen_width * screen_source_stu->screen_height * 4;
	screen_source_stu->screen_buf = framebuffer;
	screen_source_stu->color = RIGOL_COLOR_GREEN;
	if(!screen_source_stu->screen_buf)
	{
		return -1;
	}
	
	mdelay(5);
	
	return 0;
}

//开始画进度条
static void progressbar_run(struct work_struct *work)
{
	static unsigned long u64Timestamp = 0;
	static unsigned long delay;
	static int length;
	int x1, x2, y1, y2;
	
	if(0 == u64Timestamp)
	{
		length = PROGRESSBAR_UBOOT_LEN;
		delay = NORMAL_START_DELAY;
		u64Timestamp = jiffies;
		screen_set(&g_screen_source_stu, (char *)KERNEL_FRAME_BUFFER_ADDR);
		
		x1 = g_screen_source_stu.screen_width / PROGRESSBAR_START_X_TIMES;
		x2 = g_screen_source_stu.screen_width - x1;
		y1 = g_screen_source_stu.screen_height - g_screen_source_stu.screen_height / PROGRESSBAR_START_Y_TIMES;
		y2 = g_screen_source_stu.screen_height - g_screen_source_stu.screen_height / PROGRESSBAR_END_Y_TIMES;
		
		progressbar_create(&g_processbar_stu, x1, y1, x2, y2, 0);
		progressbar_config(&g_processbar_stu, RIGOL_COLOR_RED, RIGOL_COLOR_GRAY, 3);
		progressbar_active(&g_screen_source_stu, &g_processbar_stu);
		draw_string((unsigned int *)(KERNEL_FRAME_BUFFER_ADDR + (x1 + y1 * SCREEN_LCD_WIDTH)* 4) , "upgrade the system....");
	}
	progressbar_upgrate(&g_screen_source_stu, &g_processbar_stu, length);
	
	if(length <= PROGRESSBAR_COMPLETE_RATE)
	{
		length += PROGRESSBAR_EVERY_STEP;
		queue_delayed_work(mywork, &progressbar_work, delay);
	}
	else
	{
		progressbar_exit();
		printk("%s finally end here-------------------------------\n", __FUNCTION__);
		return;
	}
	
}

static int progressbar_init(void)
{
	mywork = create_singlethread_workqueue("my work");
	printk("****************BOOT UI *************************\n");
	//printk("we get value from uboot: %s\n", pArray);
	INIT_DELAYED_WORK(&progressbar_work, progressbar_run);
	queue_delayed_work(mywork, &progressbar_work, 0);
	//schedule_delayed_work(&progressbar_work, 0);
	
	return 0;
}

static void progressbar_exit(void)
{
	destroy_workqueue(mywork);
}

//
static int draw_char(unsigned int *framebuffer, int pos)
{
	 
    int i, j, k;
    int mask = CHAR_MASK;
    unsigned int *buf = framebuffer - SCREEN_LCD_WIDTH * CHAR_HEIGHT;
    for(i = pos; i < pos + CHAR_HEIGHT; i++)
    {
		for(j = 0; j < 2; j++)
		{
			for(k = 0; k < CHAR_BIT; k++)
			{
				if(alpha[i][j] & (mask >> k))
				{
					memset(&buf[(i - pos) * SCREEN_LCD_WIDTH + j * CHAR_BIT + k], RIGOL_COLOR_WHITE, 4);
				}
			}
		}
	}
	 
	return 0;     
}

//string
static int draw_string(unsigned int *framebuffer, char *string)
{
	int i;
	int u32CharPos;
	unsigned int *pTempBuf = framebuffer;
	for(i = 0; i < strlen(string); i++)
	{
		if('.' == string[i])
		{
			u32CharPos = CHAR_POINT_POS;
			draw_char(pTempBuf, u32CharPos);
		}
		else if(' ' == string[i])
		{
		}
		else 
		{
			u32CharPos = CHAR_POS(string[i]) * CHAR_WIDTH;
			draw_char(pTempBuf, u32CharPos);
		}
		pTempBuf += 10;
	}
	
	return 0;
}


early_initcall(progressbar_init);
