#include "draw_progressbar.h"


screen_info screen_source_stu;
processbar_t processbar_stu;
static struct delayed_work progressbar_work;
static struct workqueue_struct *mywork;
//static char *pArray = NULL;

//static int __init get_arg_from_uboot(char *s)
//{
	//if(NULL != s)
	//{
		//sprintf(pArray, "%s", s);
	//}
	
	//return 0;
//}

//__setup("partitions=", get_arg_from_uboot);

//设置颜色
void ui_set_color(unsigned int color)
{
	screen_source_stu.color = color;
}

//获取颜色
int ui_get_color(void)
{
	return screen_source_stu.color;
}


//画进度条边框
int draw_hollowrectangle(int x1, int y1, int x2, int y2)
{
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
	base1  = screen_source_stu.screen_buf + (screen_source_stu.screen_width * start_y + start_x)* 4;
	base2  = base1 + (end_y - start_y) * screen_source_stu.screen_width * 4;
	cache_start = (uint)base1;
	for(x=start_x;x<=end_x;x++)
	{
		*((int *)base1) = screen_source_stu.color;
		*((int *)base2) = screen_source_stu.color;
		base1 += 4;
		base2 += 4;
	}
	base1  = screen_source_stu.screen_buf + (screen_source_stu.screen_width * start_y + start_x)* 4;
	base2  = base1 + (end_x - start_x) * 4;
	line_offset = screen_source_stu.screen_width * 4;
	for(y=start_y;y<=end_y;y++)
	{
		*((int *)base1) = screen_source_stu.color;
		*((int *)base2) = screen_source_stu.color;
		base1 += line_offset;
		base2 += line_offset;
	}

	//flush_cache((uint)cache_start, screen_source_stu.screen_width * screen_source_stu.screen_height * 4);
	flush_cache_all();

	return 0;
}

//填充进度条
int draw_solidrectangle(int x1, int y1, int x2, int y2)
{
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

	base1 = screen_source_stu.screen_buf + (screen_source_stu.screen_width * start_y + start_x) * 4;
	base2 = base1 + screen_source_stu.screen_width * (end_y - start_y) * 4;
	base1_1 = base1;
	base2_1 = base2;
	cache_start = (uint)base1;
	for(x=end_x;x>=start_x;x--)
	{
		for(y=end_y;y>=start_y;y-=2)
		{
			*((int *)base1_1) = screen_source_stu.color;
			*((int *)base2_1) = screen_source_stu.color;
			base1_1 += screen_source_stu.screen_width * 4;
			base2_1 -= screen_source_stu.screen_width * 4;
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
int progressbar_create(int x1, int y1, int x2, int y2, int op)
{
	//processbar_t *progress = NULL;
	int tmp;

	//progress = kmalloc(sizeof(processbar_t), GFP_KERNEL);
	//progress->kmalloc_addr = progress;
	//if(!progress)
	//{
		//printk("sprite cartoon ui: create progressbar failed\n");

		//return 0;
	//}
	processbar_stu.direction_option = op;
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
	processbar_stu.x1 = x1;
	processbar_stu.x2 = x2;
	processbar_stu.y1 = y1;
	processbar_stu.y2 = y2;
	processbar_stu.width  = x2 - x1;
	processbar_stu.height = y2 - y1;
	//配置默认属性，防止没有调用配置函数而属性缺失
	processbar_stu.st_x	 = processbar_stu.pr_x = x1 + 1;
	processbar_stu.st_y	 = processbar_stu.pr_y = y1 + 1;
	processbar_stu.frame_color 	 = RIGOL_COLOR_WHITE;
	processbar_stu.progress_color = RIGOL_COLOR_GREEN;
	processbar_stu.progress_ratio = 0;
	processbar_stu.thick          = 1;
	processbar_stu.frame_color 	 = RIGOL_COLOR_GREEN;
	processbar_stu.progress_color = RIGOL_COLOR_RED;
	
	return 0;
}


//配置进度条
int progressbar_config(int frame_color, int progress_color, int thickness)
{
	//processbar_t *progress = (processbar_t *)p;

	//if(!p)
	//{
		//return -1;
	//}
	processbar_stu.frame_color 	 = frame_color;
	processbar_stu.progress_color = progress_color;
	processbar_stu.progress_ratio = 0;
	processbar_stu.thick          = thickness;
	switch (processbar_stu.direction_option) {
	case 0:
		processbar_stu.st_x = processbar_stu.pr_x = processbar_stu.x1 + thickness;
		processbar_stu.st_y = processbar_stu.pr_y = processbar_stu.y1 + thickness;
		break;
	case 1:
		processbar_stu.st_x = processbar_stu.pr_x = processbar_stu.x2 + thickness;
		processbar_stu.st_y = processbar_stu.pr_y = processbar_stu.y1 + thickness;
		break;
	case 2:
		processbar_stu.st_x = processbar_stu.pr_x = processbar_stu.x1 + thickness;
		processbar_stu.st_y = processbar_stu.pr_y = processbar_stu.y1 + thickness;
		break;
	case 3:
		processbar_stu.st_x = processbar_stu.pr_x = processbar_stu.x1 + thickness;
		processbar_stu.st_y = processbar_stu.pr_y = processbar_stu.y2 + thickness;
		break;
	default:
		break;
	}

	return 0;
}


//设置属性并画出边框
int progressbar_active(void)
{
	int base_color;
	int i;
	//processbar_t *progress = (processbar_t *)p;

	//if(!p)
	//{
		//return -1;
	//}
	base_color = ui_get_color();
	ui_set_color(processbar_stu.frame_color);
	for(i=0;i<processbar_stu.thick;i++)
	{
	    draw_hollowrectangle(processbar_stu.x1+i, processbar_stu.y1+i, processbar_stu.x2-i, processbar_stu.y2-i);
	}
	ui_set_color(base_color);

	return 0;
}


int progressbar_upgrate(int rate)
{
	//processbar_t *progress = (processbar_t *)p;
	int base_color, progresscolor;
	int pixel;
	int x1 = 0, y1 = 0;
	int x2 = 0, y2 = 0;

	if((rate < 0) || (rate > 100))
	{
		printk("ui progressbar: invalid progressbar rate\n");
		return -1;
	}
	//if(!p)
	//{
		//printk("ui progressbar: invalid progressbar pointer\n");
		//return -1;
	//}
	if (processbar_stu.direction_option <= 1)
		pixel = (rate * (processbar_stu.width - processbar_stu.thick*2)/100);
	else
		pixel = (rate * (processbar_stu.height - processbar_stu.thick*2)/100);
	if(rate == processbar_stu.progress_ratio)
	{
		return 0;
	}
	else
	{
		if (processbar_stu.direction_option <= 1) {
			x1 = processbar_stu.pr_x;
			if (processbar_stu.direction_option % 2)
				x2 = processbar_stu.st_x - pixel;
			else
				x2 = processbar_stu.st_x + pixel;
			progresscolor = (rate > processbar_stu.progress_ratio)
					    ? (processbar_stu.progress_color)
					    : (RIGOL_COLOR_BLACK);
			processbar_stu.pr_x  = x2;
			processbar_stu.progress_ratio = rate;
		} else {
			y1 = processbar_stu.pr_y;
			if (processbar_stu.direction_option % 2)
				y2 = processbar_stu.st_y - pixel;
			else
				y2 = processbar_stu.st_y + pixel;

			progresscolor = (rate > processbar_stu.progress_ratio)
					    ? (processbar_stu.progress_color)
					    : (RIGOL_COLOR_BLACK);
			processbar_stu.pr_y  = y2;
			processbar_stu.progress_ratio = rate;
		}

	}
	base_color = ui_get_color();
	ui_set_color(progresscolor);

	if (processbar_stu.direction_option <= 1) {
		y1 = processbar_stu.y1 + processbar_stu.thick;
		y2 = processbar_stu.y2 - processbar_stu.thick;
	} else {
		x1 = processbar_stu.x1 + processbar_stu.thick;
		x2 = processbar_stu.x2 - processbar_stu.thick;
	}


	draw_solidrectangle(x1, y1, x2, y2);

	ui_set_color(base_color);

	return 0;
}


//填充结构体
int screen_set(screen_info *screen_info_stu, char *framebuffer)
{
	screen_info *screen_source_stu = screen_info_stu;
	/* 初始化图形参数 */
	screen_source_stu->screen_width  = 480; 
	screen_source_stu->screen_height = 272;

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
	
	//board_display_set_alpha_mode(0);
	mdelay(5);
	
	return 0;
}


//开始画进度条
static void progressbar_run(struct work_struct *work)
{
	static unsigned long u64Timestamp = 0;
	static unsigned long delay;
	static int length;
	//int i;
	//uint pro_hd;
	int x1, x2, y1, y2;
	
	if(0 == u64Timestamp)
	{
		length = 0;
		delay = 8;
		u64Timestamp = jiffies;
		screen_set(&screen_source_stu, (char *)0xC6400000);
		
		x1 = screen_source_stu.screen_width / 16;
		x2 = screen_source_stu.screen_width - x1;
		y1 = screen_source_stu.screen_height - screen_source_stu.screen_height / 48;
		y2 = screen_source_stu.screen_height - screen_source_stu.screen_height / 56;
		
		progressbar_create(x1, y1, x2, y2, 0);
		progressbar_config(RIGOL_COLOR_RED, RIGOL_COLOR_GRAY, 3);
		progressbar_active();
	}
	progressbar_upgrate(length);
	
	if(length <= 100)
	{
		length += 2;
		queue_delayed_work(mywork, &progressbar_work, delay);
		//schedule_delayed_work(&progressbar_work, delay);
	}
	else
	{
		progressbar_exit();
		printk("%s finally end here-------------------------------\n", __FUNCTION__);
		return;
	}
	
}


int progressbar_init(void)
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

early_initcall(progressbar_init);
