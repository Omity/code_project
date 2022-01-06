

#include "spi.h"




#define spi_delay(delay)   /
{ /
    register uint32 i = 0;  /
    while(i < delay)  { /
        __asm{ /
            NOP; /
            NOP; /
            NOP; /
            NOP; /
        }; /
        i -= 4; /
    } /
}

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/system.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/sched.h>
 
#include <linux/proc_fs.h>
#include <linux/poll.h>
 
#include <asm/bitops.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
 
#include <linux/moduleparam.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <linux/wait.h>
 
#define IOCONFIG4_4                                 IO_ADDRESS(0x200f0000+ 0x054)          //复用管脚gpio4_4
#define IOCONFIG4_5                                 IO_ADDRESS(0x200f0000+ 0x050)          //复用管脚gpio4_5
#define IOCONFIG4_6                                 IO_ADDRESS(0x200f0000+ 0x04c)          //复用管脚gpio4_6
#define IOCONFIG4_7                                 IO_ADDRESS(0x200f0000+ 0x048)          //复用管脚gpio4_7
 
#define GPIO4_BASE                                    0x20180000                        //gpio基地址
#define GPIO4_SIZE                                     0x10000             // 64KB
#define GPIO4_DIR                                      IO_ADDRESS(GPIO4_BASE + 0x400)     //gpio4方向寄存器
#define GPIO_SPI_CS_REG                         IO_ADDRESS(GPIO4_BASE+ 0x40)          //gpio4_4数据寄存器
#define GPIO4_SPI_SCK_REG                    IO_ADDRESS(GPIO4_BASE + 0x80)          //gpio4_5数据寄存器
#define GPIO4_SPI_MOSI_REG                 IO_ADDRESS(GPIO4_BASE+ 0x100)     //gpio4_6数据寄存器
#define GPIO4_SPI_MISO_REG                 IO_ADDRESS(GPIO4_BASE+ 0x200)     //gpio4_7数据寄存器
#define GPIO4_4                                           (1<< 4)   //gpio4_4
#define GPIO4_5                                           (1<< 5)   //gpio4_5
#define GPIO4_6                                           (1<< 6)   //gpio4_6
#define GPIO4_7                                           (1<< 7)   //gpio4_7
 
#define DATA_LENGTH                                21
 
#define DRV_AUTHOR                                "Yang Zheng<yz2012ww@gmail.com>"
#define DRV_DESC                                       "spi driver"                                                                                
#define DRV_VERSION                                "v0.0.1"
 
#if 0
#define DBG(x...)   printk(x)
#define DBG_PRINT
#else
#define DBG(x...)   do {} while (0)
#endif
 
static DECLARE_WAIT_QUEUE_HEAD(spi_waitq);
unsigned char                                       read_tmp_data[DATA_LENGTH] = {0};
void __iomem                                       *reg_ssp_base_va;
 
typedef unsigned char                       byte;
typedef unsigned short                      word;
 
static int                                                read_flag = 0;
 
 
/******************************************************************************
**函数名称：Set_nCS
**函数功能：禁用片选
**输入参数：无
**输出参数：无
**注意：   高电平为禁用片选，低电平为使能片选
******************************************************************************/
void Set_nCS(void)
{
         unsigned char regvalue;
 
         writel(0, IOCONFIG4_4);   //gpio模式
 
         regvalue = readl(GPIO4_DIR);  //设置gpio输出
         regvalue |= GPIO4_4;
         writel(regvalue, GPIO4_DIR);
 
        
         regvalue = readl(GPIO_SPI_CS_REG); 
         regvalue |= GPIO4_4;
         writel(regvalue, GPIO_SPI_CS_REG);  //禁用片选
}
 
 
/******************************************************************************
**函数名称：Clr_nCS
**函数功能：使能片选
**输入参数：无
**输出参数：无
**注意：   高电平为禁用片选，低电平为使能片选
******************************************************************************/
void Clr_nCS(void)
{
         unsigned char regvalue;
 
         writel(0, IOCONFIG4_4);   //gpio模式
 
         regvalue = readl(GPIO4_DIR);  //设置gpio输出
         regvalue |= GPIO4_4;
         writel(regvalue, GPIO4_DIR);
 
         regvalue = readl(GPIO_SPI_CS_REG); 
         regvalue &= ~GPIO4_4;
         writel(0, GPIO_SPI_CS_REG);  //使能片选
 
}
 
 
/******************************************************************************
**函数名称：Set_SCK
**函数功能：SCK为高电平
**输入参数：无
**输出参数：无
**注意：    
******************************************************************************/
void Set_SCK(void)
{
         unsigned char regvalue;
 
         writel(0, IOCONFIG4_5);   //gpio模式
 
         regvalue = readl(GPIO4_DIR);  //设置gpio输出
         regvalue |= GPIO4_5;
         writel(regvalue, GPIO4_DIR);
 
         regvalue =readl(GPIO4_SPI_SCK_REG);  //设置gpio输出高电平
         regvalue |= GPIO4_5;
         writel(regvalue, GPIO4_SPI_SCK_REG);
}
 
 
/******************************************************************************
**函数名称：Set_SCK
**函数功能：SCK为高电平
**输入参数：无
**输出参数：无
**注意：    
******************************************************************************/
void Clr_SCK(void)
{
         unsigned char regvalue;
 
         writel(0, IOCONFIG4_5);   //gpio模式
 
         regvalue = readl(GPIO4_DIR);  //设置gpio输出
         regvalue |= GPIO4_5;
         writel(regvalue, GPIO4_DIR);
 
         regvalue =readl(GPIO4_SPI_SCK_REG);  //设置gpio输出低电平
         regvalue &= ~GPIO4_5;
         writel(regvalue,GPIO4_SPI_SCK_REG); 
 
}
 
 
/******************************************************************************
**函数名称：Set_MOSI
**函数功能：MOSI为高电平
**输入参数：无
**输出参数：无
**注意：    
******************************************************************************/
void Set_MOSI(void)
{
         unsigned char regvalue;
 
         writel(0, IOCONFIG4_6);   //gpio模式
 
         regvalue = readl(GPIO4_DIR);  //设置gpio输出
         regvalue |= GPIO4_6;
         writel(regvalue, GPIO4_DIR);
 
         regvalue = readl(GPIO4_DIR);  //设置gpio输出高电平
         regvalue |= GPIO4_6;
         writel(regvalue, GPIO4_SPI_MOSI_REG);
 
}
 
 
/******************************************************************************
**函数名称：Set_MOSI
**函数功能：MOSI为低电平
**输入参数：无
**输出参数：无
**注意：    
******************************************************************************/
void Clr_MOSI(void)
{
         unsigned char regvalue;
 
         writel(0, IOCONFIG4_6);   //gpio模式
 
         regvalue = readl(GPIO4_DIR);  //设置gpio输出
         regvalue |= GPIO4_6;
         writel(regvalue, GPIO4_DIR);
 
         regvalue = readl(GPIO4_DIR);  //设置gpio输出低电平
         regvalue &= ~GPIO4_6;
         writel(regvalue, GPIO4_SPI_MOSI_REG);
 
}
 
/******************************************************************************
**函数名称：MISO_H
**函数功能：读取MISO_H的值
**输入参数：无
**输出参数：无
**注意：    
******************************************************************************/
unsigned charMISO_H(void)
{
         unsigned char regvalue;
 
         writel(0, IOCONFIG4_7);   //gpio模式
 
         regvalue = readl(GPIO4_DIR);  //设置gpio输入
         regvalue &= ~GPIO4_7;
         writel(regvalue, GPIO4_DIR);
 
         regvalue =readl(GPIO4_SPI_MISO_REG);  //读取输入值
         //DBG("regvalue = %x\n",regvalue);
 
         return regvalue;
}
 
/******************************************************************************
**函数名称：SPICmd8bit
**函数功能：SPI写入参数8bit
**输入参数：WrPara
**输出参数：无
**注意：   保留nCS输出Low
******************************************************************************/
voidSPICmd8bit(byte WrPara)
{
         byte bitcnt;     
         Clr_nCS();
         Clr_SCK();
 
         for(bitcnt = 8; bitcnt != 0; bitcnt--)
         {
                   Clr_SCK();
                   if(WrPara&0x80)
                   {
                            Set_MOSI();
                   }
                   else
                   {
                            Clr_MOSI();
                   }
                   Set_SCK();
                   WrPara <<= 1;
         }
         Clr_SCK();
         Set_MOSI();
 //Set_nCS();            //*此处不关闭nCS，使用连续模式*
}
 
/******************************************************************************
**函数名称：SPIRead8bitt
**函数功能：SPI读取参数8bit
**输入参数：读取8bit数据——RdPara
**输出参数：无
**注意：   保留nCS输出Low
******************************************************************************/
byte SPIRead8bit(void)
{
         byte RdPara = 0;
         byte bitcnt;
          
         Clr_nCS();
         Set_MOSI();               //读FIFO，维持SDI为H 
 
         for(bitcnt=8; bitcnt!=0; bitcnt--)
         {
                   Clr_SCK();
                   RdPara <<= 1;
                   Set_SCK();
                   if(MISO_H())
                   {
                            RdPara |= 0x01;
                   }
                   else
                   {
                            ;
                   }
         }
 
         Clr_SCK();
 
         //Set_nCS();               //*此处不关闭nCS，使用连续模式*
         //DBG("RdPara = %x\n",RdPara);
         return(RdPara);
}
 
/******************************************************************************
**函数名称：SPIRead
**函数功能：SPI读取一个地址数据
**输入参数：adr
**输出参数：无
******************************************************************************/
byte SPIRead(byteadr)
{
         byte tmp;         
 
         SPICmd8bit(adr);               //发送要读取的地址
         tmp = SPIRead8bit();    //读取数据    
         Set_nCS();
 
         return(tmp);
}
 
/******************************************************************************
**函数名称：SPIWrite
**函数功能：SPI写入一个16数据（高8位地址，低8位数据）
**输入参数：WrPara
**输出参数：无
******************************************************************************/
void SPIWrite(wordWrPara)                                                                    
{                                                      
         byte bitcnt;   
          
         Clr_SCK();                           //注意SCK先清0，保持低
         Clr_nCS();
          
         WrPara |= 0x8000;           //写数据高位置1
          
         for(bitcnt=16; bitcnt!=0; bitcnt--)
         {
                   Clr_SCK();
                   if(WrPara&0x8000)
                   {
                            Set_MOSI();
                   }
                   else
                   {
                            Clr_MOSI();
                   }
 
                   Set_SCK();
                   WrPara <<= 1;
         }
          
         Clr_SCK();
         Set_MOSI();
         Set_nCS();
}         
 
/******************************************************************************
**函数名称：SPIBurstRead
**函数功能：SPI连续读取模式
**输入参数：adr——读取地址
**          ptr——存储数据指针
**          length 读取长度
**输出参数：无，数据存在ptr中
******************************************************************************/
void SPIBurstRead(byte adr, byte *ptr, byte length)
{
         byte i;
         if(length<=1)                       //读取长度必须大于1
         {
                   return;
         }
        else
        {
                 Clr_SCK();                          //注意SCK先清0，保持低
                 Clr_nCS();
                 SPICmd8bit(adr);               //读取地址
                 for(i=0;i<length;i++)
                   {
                         ptr[i]= SPIRead8bit();
                   }
                 Set_nCS();        
         }
}
 
/******************************************************************************
**函数名称：SPIBurstWrite
**函数功能：SPI连续写入模式
**输入参数：adr——写入地址
**          ptr——存储数据指针
**          length 写入长度
**输出参数：无
******************************************************************************/
void BurstWrite(byte adr, byte *ptr, byte length)
{
         byte i;
 
         if(length<=1)                       //读取长度不为0或1
         {
                   return;
         }
         else 
         {      
                   Clr_SCK();                           //注意SCK先清0，保持低
                   Clr_nCS();                                             
                   SPICmd8bit(adr|0x80);    //连续写
                   for(i=0;i<length;i++)
                   {
                            SPICmd8bit(ptr[i]);
                   }
                   Set_nCS();        
         }
}
 
long spi_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
         ……
 
         switch(cmd)
         {
                   case SSP_READ_ALT:
                             …...
                            val = *(unsigned int*)arg;
                            addr = (unsignedchar)(val&0xff);
                            data =SPIRead(addr);
                            ……
                            *(unsigned int *)arg= (unsigned int)(data&0x0000ff);
                            break;
 
 
                   case SSP_READ_BURST:
                            {
                                     unsignedchar          addr = 0x00;
 
                                     ……
 
                                     /*读取数据 */
                                     SPIBurstRead(addr,read_tmp_data, DATA_LENGTH);
 
                                     …...
 
                                     break;
                            }
 
                   case SSP_WRITE_ALT:
                            data = 0;
                            val  = *(unsigned int *)arg;
                            tmp = (unsignedshort)((val&0xff0000)>>8);      
                            tmp |= (unsignedshort)((val&0x0000ff)>>0);
                            ……
                            SPIWrite(tmp);
                            break; 
 
                   case SSP_WRITE_BURST:
                            res =copy_from_user(tmp_str, (unsigned char *)arg, sizeof(tmp_str));
                            if (res != 0)
                            {
                                     printk("copydata from the user space error\n");
                            }
                           
                            ……
                           
                            BurstWrite(addr,data, DATA_LENGTH);
                                                                                                      
                            break;
 
                   default:
                            {
                                     printk("Nosuch spi command %#x!\n", cmd);
                                     return -1;
                            }
         }
         return 0;
}
 
int spi_open(structinode * inode, struct file * file)
{
         ……
 
         return 0;
}
 
int spi_close(struct inode * inode, struct file * file)
{
         return 0;
}
 
 
static struct file_operations spi_fops = {
         .owner          =THIS_MODULE,
         .unlocked_ioctl        = spi_ioctl,
         .open           =spi_open,
         .release           =spi_close
};
 
 
static structmiscdevice spi_dev = {
         .minor                =MISC_DYNAMIC_MINOR,                                                                                      
         .name               =DEV_NAME,
         .fops                           =&spi_fops,
};
 
static int __init spi_gpio_init(void)
{
         int ret;
 
         ret = misc_register(&spi_dev);
         if (ret < 0)
         {
                   printk("registerspi_gpio device failed!\n");
 
                   return -1;
         }
 
         printk("SPI driver initializesuccessful! .\n");
 
         return 0;
}
 
static void __exitspi_gpio_exit(void)
{
         misc_deregister(&spi_dev);
 
         printk("SPI driver exit!\n");
}
module_init(spi_gpio_init);
module_exit(spi_gpio_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRV_AUTHOR);
MODULE_DESCRIPTION(DRV_DESC);




