/*
 *
 * RIGOL GPIO driver.
 *
 * Copyright (c) 2021-2030, FRIGOL TECHNOLOGIES CO., LTD. all rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
/*******************************************************************************
                      普源精电科技技术有限公司版权所有
********************************************************************************
  源文件名: gpio_trig.c
  功能描述: GPIO 设备驱动，用于控制8个IO作为触发输入和输出。
           D0_IN:PE0
	   D0_OUT:PE1
           D1_IN:PE2
	   D1_OUT:PE3
           D2_IN:PE4
	   D2_OUT:PE5
           D3_IN:PE6
	   D3_OUT:PE7

  作    者: sn02320
  版    本:
  创建日期:
  修改历史:
  作者      修改时间        版本                            修改内容
*******************************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h> /* copy_to_usr copy_from_usr*/
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/delay.h>
#include <linux/bcd.h>
#include <linux/capability.h>
#include <linux/cdev.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/of_irq.h>
#include <linux/sched.h>   //wake_up_process()
#include <linux/kthread.h> //kthread_create()/kthread_run()
#include <linux/err.h>     //IS_ERR()/PTR_ERR()

#define TRIG_GPIO_OUTPUT(pin, level)   gpio_direction_output(pin, level)
#define TRIG_GPIO_REQUEST(pin, label)  gpio_request(pin, label)
#define TRIG_GPIO_FREE(pin) 		   gpio_free(pin)
#define TRIG_GPIO_GET_VALUE(pin) 	   gpio_get_value(pin)

#define TRIG_GPIO_AS_INPUT(pin) \
  do                            \
  {                             \
    gpio_direction_input(pin);  \
  } while (0)

#define TRIG_IRQ_TAB                                                                     \
  {                                                                                      \
    IRQ_TYPE_EDGE_RISING, IRQ_TYPE_EDGE_FALLING, IRQ_TYPE_LEVEL_LOW, IRQ_TYPE_LEVEL_HIGH \
  }

// ioctl 的out命令
#define TRIG_GPIO_OUT_LOW 0
#define TRIG_GPIO_OUT_HIGH 1
// ioctl 的in命令
#define TRIG_GPIO_SET_PID 2
#define TRIG_GPIO_SET_TRIG_RISING 3
#define TRIG_GPIO_SET_TRIG_FALLING 4
#define TRIG_GPIO_SET_TRIG_LOW 5
#define TRIG_GPIO_SET_TRIG_HIGH 6
#define GPIO_IN_READ_STATE      7


static int trigOut_gpios[] = {42, 52, 53, 259,258}; //PB10, PB20, PB21, PI2, PI3 
static int trigIn_gpios[] = {273, 272, 230, 231}; //PI17, PI16, PH6, PH7
#define GPIO_OUT_NUM ARRAY_SIZE(trigOut_gpios)
#define GPIO_IN_NUM ARRAY_SIZE(trigIn_gpios)

// trigIn 相关宏定义
#define	TRIG_GPIO_IN_D0        0
#define	TRIG_GPIO_IN_D1        1
#define	TRIG_GPIO_IN_D2        2
#define	TRIG_GPIO_IN_D3        3
#define TRIG_GPIO_IN_1         273
#define TRIG_GPIO_IN_2         272
#define TRIG_GPIO_IN_3         230
#define TRIG_GPIO_IN_4         231
#define TRIG_GPIO_OUT_1        42
#define TRIG_GPIO_OUT_2        52
#define TRIG_GPIO_OUT_3        53
#define TRIG_GPIO_OUT_4        258
#define	TRIG_GPIO_IN_D0_NAME   "gpio_trig_in1"
#define	TRIG_GPIO_IN_D1_NAME   "gpio_trig_in2"
#define	TRIG_GPIO_IN_D2_NAME   "gpio_trig_in3"
#define	TRIG_GPIO_IN_D3_NAME   "gpio_trig_in4"
#define TRIG_STATE_TRUE        1
#define TRIG_STATE_FALSE       0
#define TRIG_GPIO_IN_NUM       4
#define TRIG_GPIO_LOW          0

#define DEVICE_NAME "gpio_trig"

struct gpioData
{
  int gpio_in[4];
  int gpio_out[5];
  int gpio_irq[4];

  char as8InGpioName[4][20];

  struct task_struct *task; //中断发生后,向哪个进程发送信号
  int pid;                  //进程号
  struct siginfo info;

  spinlock_t lock;
};

struct gpioData *pstTrigGpioInfo;

//用户空间查询trigIN所用到的数据结构
struct trigInDataStu{
	char trigInName[4][20];
	int trigInState[4];
	
};

struct trigInDataStu trigInData;

/* open close 函数 */
static int gpio_trig_open(struct inode *inode, struct file *filp)
{
  return 0;
}

static int gpio_trig_close(struct inode *inode, struct file *filp)
{
  return 0;
}

/* 中断处理函数 */
static irq_handler_t trig_gpioD0_irq(int irq, void *arg)
{

  disable_irq_nosync(irq);    //中断屏蔽,防止中断嵌套
  printk("trig_gpio_irq_d0\n");

  //触发之后标志位置1,表示触发成功
  trigInData.trigInState[TRIG_GPIO_IN_D0] = TRIG_STATE_TRUE;
  
  enable_irq(irq);

  return IRQ_RETVAL(IRQ_HANDLED);
}

static irq_handler_t trig_gpioD1_irq(int irq, void *arg)
{

  disable_irq_nosync(irq);
  printk("trig_gpio_irq_d1\n");

  trigInData.trigInState[TRIG_GPIO_IN_D1] = TRIG_STATE_TRUE;
  enable_irq(irq);
  return IRQ_RETVAL(IRQ_HANDLED);
}

static irq_handler_t trig_gpioD2_irq(int irq, void *arg)
{

  disable_irq_nosync(irq);
  printk("trig_gpio_irq_d2\n");
 
  trigInData.trigInState[TRIG_GPIO_IN_D2] = TRIG_STATE_TRUE;
  enable_irq(irq);
  return IRQ_RETVAL(IRQ_HANDLED);
}

static irq_handler_t trig_gpioD3_irq(int irq, void *arg)
{
	
  disable_irq_nosync(irq);
  printk("trig_gpio_irq_d3\n");

  trigInData.trigInState[TRIG_GPIO_IN_D3] = TRIG_STATE_TRUE;
  enable_irq(irq);
  return IRQ_RETVAL(IRQ_HANDLED);
}

/**
 * ---------------------------------------------------------
 * |输入触发方式  | 上升沿  | 下降沿  |  高电平  |  低电平 |
 * | Trig-in      |         |         |          |         |
 * | Trig-out     |    0    |    0    |  不确定  |    0    |
 * ---------------------------------------------------------
 * Trig-in1 (PI17) ----> Trig-out1(PB10)
 * Trig-in2 (PI16) ----> Trig-out2(PB20)
 * Trig-in3 (PH6)  ----> Trig-out3(PB21)
 * Trig-in4 (PH7)  ----> Trig-out4(PI3)
 * 由于trig-in和trig-out在电路图上相连,所以trig-out的值会影响到trig-in
 * 模式的选择, 所以在设置触发方式时默认会把对应的trig-out置0
 */
/* 设置out电压为低电平 */
static int trig_out_set_low(int gpio_in, struct gpioData *pData)
{
	int ret = 0;
	int gpioOut;
	char as8OutGpioName[20];
	switch(gpio_in)
	{
		case TRIG_GPIO_IN_1:        //Trig-in1
			gpioOut = TRIG_GPIO_OUT_1;
			sprintf(as8OutGpioName, "%s_%d", "gpio_out", TRIG_GPIO_OUT_1);
			break;
		case TRIG_GPIO_IN_2:        //Trig-in2
			gpioOut = TRIG_GPIO_OUT_2;
			sprintf(as8OutGpioName, "%s_%d", "gpio_out", TRIG_GPIO_OUT_2);
			break;
		case TRIG_GPIO_IN_3:        //Trig-in3
			gpioOut = TRIG_GPIO_OUT_3;
			sprintf(as8OutGpioName, "%s_%d", "gpio_out", TRIG_GPIO_OUT_3);
			break;
		case TRIG_GPIO_IN_4:        //Trig-in4
			gpioOut = TRIG_GPIO_OUT_4;
			sprintf(as8OutGpioName, "%s_%d", "gpio_out", TRIG_GPIO_OUT_4);
			break;		
		default:
			sprintf(as8OutGpioName, "%s", "gpio_out");
			break;
	}
	//spin_lock(&pData->lock);
	//ret = TRIG_GPIO_OUTPUT(gpioOut, TRIG_GPIO_LOW);
	//if(ret)
	//{
		//printk("failed!");
	//}
	//spin_unlock(&pData->lock);
	
	//return ret;
	if (gpio_is_valid(gpioOut))
    {
      ret = gpio_request(gpioOut, as8OutGpioName);
      if (ret)
      {
        printk("[GPIO]d0_gpio_out gpio request failed");
        goto err_gpio_req;
      }
      ret = TRIG_GPIO_OUTPUT(gpioOut, TRIG_GPIO_LOW);
      if (ret)
      {
        printk("[GPIO]set_direction for d0_gpio_out gpio failed");
        goto err_gpio_req;
      }
	}
      return 0;
      
err_gpio_req:
	if(gpio_is_valid(gpioOut))
	{
		gpio_free(gpioOut);
	}
	
	return ret;
}

/* 注册中断 */
static int trig_in_irq_register(struct gpioData *pData, int gpio_in, unsigned long flags)
{
	switch(gpio_in)
	{
		case TRIG_GPIO_IN_D0:
			return request_irq(pData->gpio_irq[gpio_in],
							   trig_gpioD0_irq,
							   flags,
							   "trig0",
							   NULL);
		case TRIG_GPIO_IN_D1:
			return request_irq(pData->gpio_irq[gpio_in],
							   trig_gpioD1_irq,
							   flags,
							   "trig1",
							   NULL);
		case TRIG_GPIO_IN_D2:
			return request_irq(pData->gpio_irq[gpio_in],
							   trig_gpioD2_irq,
							   flags,
							   "trig2",
							   NULL);
		case TRIG_GPIO_IN_D3:
			return request_irq(pData->gpio_irq[gpio_in],
							   trig_gpioD3_irq,
							   flags,
							   "trig3",
							   NULL);
		default:
			return 1;
	}
}

/* 注销中断 */
static int trig_in_irq_deregister(struct gpioData *pData, int gpio_in)
{
	gpio_free(pData->gpio_in[gpio_in]);
	free_irq(pData->gpio_irq[gpio_in], NULL);
}

/* ioctl 函数 */
/*
 * 
 * name: gpio_trig_ioctl
 * @param struct file *filp, unsigned int cmd, unsigned long arg
 * @return 
 * @description IO命令,当使用out时中断关闭, 使用in时out对应脚默认置低       
 */

static long gpio_trig_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
  int ret = 0;
  int i;
  //unsigned long irqflags;
  struct gpioData *pstTrig = pstTrigGpioInfo;
  struct trigInDataStu tempTrigInData;
  switch (cmd)
  {
  case TRIG_GPIO_OUT_LOW:
     printk("gpio_trig_ioctl = %d, %lu\n", cmd, arg);

    //禁止内核抢占
    spin_lock(&pstTrig->lock);
    trig_in_irq_deregister(pstTrig, arg);
    TRIG_GPIO_OUTPUT(trigOut_gpios[arg], cmd);
    trigInData.trigInState[arg] = TRIG_STATE_FALSE;
    spin_unlock(&pstTrig->lock);
    break;
  
  case TRIG_GPIO_OUT_HIGH:
    printk("gpio_trig_ioctl = %d, %lu\n", cmd, arg);

    //禁止内核抢占
    spin_lock(&pstTrig->lock);
    trig_in_irq_deregister(pstTrig, arg);
    TRIG_GPIO_OUTPUT(trigOut_gpios[arg], cmd);
    trigInData.trigInState[arg] = TRIG_STATE_FALSE;
    spin_unlock(&pstTrig->lock);
    
    break;
  case TRIG_GPIO_SET_PID:

    //接收用户发下来的PID
    pstTrig->pid = arg;
    pstTrig->info.si_signo = trigIn_gpios[arg];
    pstTrig->info.si_code = -1;
    pstTrig->info.si_int = 0;
    rcu_read_lock();
    pstTrig->task = pid_task(find_get_pid(pstTrig->pid), PIDTYPE_PID);
    if (pstTrig->task == NULL)
    {
      printk("no such pid\n");
      rcu_read_unlock();
      return -ENODEV;
    }
    rcu_read_unlock();
    break;

  case TRIG_GPIO_SET_TRIG_RISING:
      printk("gpio_trig_ioctl_irq = %d, %lu,%d\n", cmd, arg,pstTrig->gpio_irq[arg]);
      if((ret = trig_in_irq_register(pstTrig, arg, IRQ_TYPE_EDGE_RISING) < 0))
      {
		  irq_set_irq_type(pstTrig->gpio_irq[arg],IRQ_TYPE_EDGE_RISING);
      }
      trig_out_set_low(pstTrig->gpio_in[arg], pstTrig);
      break;
  case TRIG_GPIO_SET_TRIG_FALLING:
      printk("gpio_trig_ioctl_irq = %d, %lu\n", cmd, arg);
      if((ret = trig_in_irq_register(pstTrig, arg, IRQ_TYPE_EDGE_FALLING) < 0))
      {
		  irq_set_irq_type(pstTrig->gpio_irq[arg],IRQ_TYPE_EDGE_FALLING);
	  }
      trig_out_set_low(pstTrig->gpio_in[arg], pstTrig);
      break;
  case TRIG_GPIO_SET_TRIG_LOW:
      printk("gpio_trig_ioctl_irq = %d, %lu\n", cmd, arg);
      if((ret = trig_in_irq_register(pstTrig, arg, IRQ_TYPE_LEVEL_LOW) < 0))
      {
		  irq_set_irq_type(pstTrig->gpio_irq[arg],IRQ_TYPE_LEVEL_LOW);
	  }
      trig_out_set_low(pstTrig->gpio_in[arg], pstTrig);
      break;
  case TRIG_GPIO_SET_TRIG_HIGH:
      printk("gpio_trig_ioctl_irq = %d, %lu\n", cmd, arg);
      if((ret = trig_in_irq_register(pstTrig, arg, IRQ_TYPE_LEVEL_HIGH) < 0))
      {
		  irq_set_irq_type(pstTrig->gpio_irq[arg],IRQ_TYPE_LEVEL_HIGH);
	  }
      trig_out_set_low(pstTrig->gpio_in[arg], pstTrig);
      break;
  case GPIO_IN_READ_STATE:
      if(copy_from_user(&tempTrigInData, (struct trigInDataStu *)arg, sizeof(struct trigInDataStu)))
      {
		  return -EFAULT;
	  }
	  for(i = 0; i < TRIG_GPIO_IN_NUM; i++)
	  {
		  sprintf(tempTrigInData.trigInName[i], "%s", trigInData.trigInName[i]);
	      tempTrigInData.trigInState[i] = trigInData.trigInState[i];
	  }
      if(copy_to_user((struct trigInDataStu *)arg, &tempTrigInData, sizeof(struct trigInDataStu)))
      {
		  return -EFAULT;
	  }
	  for(i = 0; i < TRIG_GPIO_IN_NUM; i++)
	  {
	      printk("%s state: %d\n",tempTrigInData.trigInName[i], tempTrigInData.trigInState[i]);
	      trigInData.trigInState[i] = 0;
	  }
	  
	  break;
      
  default:
    return -EINVAL;
  }
  return 0;
}

/* 相关文件操作 */
static struct file_operations gpio_trig_fops = {
    .owner = THIS_MODULE,
    .open = gpio_trig_open,
    .release = gpio_trig_close,
    .unlocked_ioctl = gpio_trig_ioctl,
};

static struct miscdevice trig_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &gpio_trig_fops,
};

static int trig_gpio_parse_dt(struct device *dev, struct gpioData *pdata)
{
  int ret = 0;

 /* d0~d4 gpio info */
    for(int i = 0; i < 4; i++)
    {
        pdata->gpio_in[i] = trigIn_gpios[i];
        pdata->gpio_out[i] = trigOut_gpios[i];
    }
    pdata->gpio_out[4] = trigOut_gpios[4];


  return ret;
}

static int trig_gpio_configure(struct gpioData *pdata)
{
  int ret = 0;
  int irqno = 0;
  char as8OutGpioName[20] = {0};

  /*  IN gpio */
  for(int i = 0; i < 4; i++)
  {
     sprintf(pdata->as8InGpioName[i], "%s_%d","gpio_in",i);

    if (gpio_is_valid(pdata->gpio_in[i]))
    {
	  //申请gpio资源
      ret = gpio_request(pdata->gpio_in[i], pdata->as8InGpioName[i]); //修改gpioIn名称
      if (ret)
      {
        printk("[GPIO]d0_gpio_in gpio request failed");
        goto err_gpio_req;
      }
      //设置gpio 为输入
      TRIG_GPIO_AS_INPUT(pdata->gpio_in[i]);
      //获取gpio对应的IRQ值
      pdata->gpio_irq[i] = gpio_to_irq(pdata->gpio_in[i]);

    }

  }
	//申请中断
	trig_in_irq_register(pdata, TRIG_GPIO_IN_D0, IRQ_TYPE_EDGE_FALLING);     
    trig_in_irq_register(pdata, TRIG_GPIO_IN_D1, IRQ_TYPE_EDGE_FALLING);
    trig_in_irq_register(pdata, TRIG_GPIO_IN_D2, IRQ_TYPE_EDGE_FALLING);
    trig_in_irq_register(pdata, TRIG_GPIO_IN_D3, IRQ_TYPE_EDGE_FALLING);
    //trigIn的Pin脚命名
    strcpy(trigInData.trigInName[TRIG_GPIO_IN_D0], TRIG_GPIO_IN_D0_NAME);
    strcpy(trigInData.trigInName[TRIG_GPIO_IN_D1], TRIG_GPIO_IN_D1_NAME);
    strcpy(trigInData.trigInName[TRIG_GPIO_IN_D2], TRIG_GPIO_IN_D2_NAME);
    strcpy(trigInData.trigInName[TRIG_GPIO_IN_D3], TRIG_GPIO_IN_D3_NAME);
    

  /*  out gpio */
  for(int i = 0; i < 5; i++)
  {
    sprintf(as8OutGpioName, "%s_%d","gpio_out",i);

     /* D0 out gpio */
    if (gpio_is_valid(pdata->gpio_out[i]))
    {
      ret = gpio_request(pdata->gpio_out[i], as8OutGpioName);
      if (ret)
      {
        printk("[GPIO]d0_gpio_out gpio request failed");
        goto err_gpio_req;
      }

      ret = TRIG_GPIO_OUTPUT(pdata->gpio_out[i], TRIG_GPIO_LOW);
      if (ret)
      {
        printk("[GPIO]set_direction for d0_gpio_out gpio failed");
        goto err_gpio_req;
      }
    }
  }

 

  return 0;

err_gpio_req:
    /*  out gpio */
  for(int i = 0; i < 5; i++)
  {
      if (gpio_is_valid(pdata->gpio_out[i]))
      {
        gpio_free(pdata->gpio_out[i]);
      }
  }

  for(int i = 0; i < 4; i++)
  {
      if (gpio_is_valid(pdata->gpio_in[i]))
      {
        gpio_free(pdata->gpio_in[i]);
      }
  }

  

  return ret;
}

static int gpio_trig_probe(struct platform_device *pdev)
{

  int error = 0;
  //printk("gpio_trig_probe\n");
  struct device_node *np = pdev->dev.of_node;
  enum of_gpio_flags flag = 0;


  // 申请GPIO结构资源
  pstTrigGpioInfo = devm_kzalloc(&pdev->dev, sizeof(struct gpioData), GFP_KERNEL);
  if (IS_ERR(pstTrigGpioInfo))
  {
    error = PTR_ERR(pstTrigGpioInfo);
    printk("Failed to request Holl device: %d\n", error);
    return error;
  }
 
  // 从设备树中获取管角配置信息
  trig_gpio_parse_dt(&pdev->dev, pstTrigGpioInfo);

  // 申请GPIO资源
  if (trig_gpio_configure(pstTrigGpioInfo) == 0)
  {
     //注册中断信号
  }

  //init lock
  spin_lock_init(&pstTrigGpioInfo->lock);

  //注册设备
  error = misc_register(&trig_miscdev);
  if (error < 0)
  {
    dev_info(&pdev->dev, "Fail to register misc device!\n");
    return error;
  }
  // platform_set_drvdata(pdev, pstTrigGpioInfo);
  //dev_info(&pdev->dev, "initialized\n");

  return error;
}

static int gpio_trig_remove(struct platform_device *pdev)
{
	int i;
  misc_deregister(&trig_miscdev);
   /*  out gpio */
  for(i = 0; i < 5; i++)
  {
        gpio_free(pstTrigGpioInfo->gpio_out[i]);
        //free_irq(pstTrigGpioInfo->gpio_irq[i], NULL);
  }

  for(i = 0; i < 4; i++)
  {
        gpio_free(pstTrigGpioInfo->gpio_in[i]);
        free_irq(pstTrigGpioInfo->gpio_irq[i], NULL);
  }

  return 0;
}

static const struct of_device_id gpio_trig_dt_ids[] = {
    {.compatible = "sunxi,gpio_trig"}, //需要和设备树中的字符串一致
    {},

};

MODULE_DEVICE_TABLE(of, gpio_trig_dt_ids);
// 定义trig 平台驱动
static struct platform_driver gpio_trig_driver = {
    .probe = gpio_trig_probe, // 设备和驱动匹配完成之后进行的初始化操作
    .remove = gpio_trig_remove,
    .driver = {
        .name = DEVICE_NAME, //设备名称，用于驱动匹配
        .owner = THIS_MODULE,
        .of_match_table = gpio_trig_dt_ids,
    },
};

// 注册驱动
module_platform_driver(gpio_trig_driver);

MODULE_AUTHOR("rigol sn02320");
MODULE_DESCRIPTION("gpio-trig devices driver");
MODULE_LICENSE("GPL");
