/********************************************************************
                普源精电科技股份有限公司 版权所有(2020)
*********************************************************************
头文件名: keyboard.c
功能描述: 实体按键驱动，在内核空间实现，基于串口的驱动进行修改，启动一个work进行串口的数据监测，
                 同时对按键进行解析，将最终键值传递给Linux输入子系统，在用户空间获取按键。
作   者: sn03692
版   本: 1.0
创建日期: 2021年1月22日

修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
修改日期：
版 本 号：
修 改 人：
修改内容：
*********************************************************************/

/*
 *
 * Copyright (c) 2021-2030, RIGOL TECHNOLOGIES CO., LTD. all rights reserved.
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


/********************************************************************
* Includes
********************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/clk.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/ctype.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/platform_device.h>

#include <linux/console.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <linux/clk.h>
#include <linux/clk-private.h>
#include <linux/clk/sunxi.h>
#include <linux/pinctrl/consumer.h>

#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/input.h>
#include <dt-bindings/gpio/gpio.h>

#include "Sousakeyboard.h"
/********************************************************************
* Definitions
********************************************************************/
/* 位操作 */
#define CHECK_BIT(b,v)          ((v>>b)&0x1) //检查某位是否为1
#define HIGH_BIT_MASK           0x80         //字节最高位掩码
#define LOW_BIT_MASK            0x01         //字节最低位掩码


/********************************************************************
* Variables
********************************************************************/
/* 上次按键值 */
static u64 g_u64KeyValueOld = 0xFFFFFFFFFFFFFFFF ;

/* input子系统决定必须上报一次按下再上报一次抬起，依次循环进行，
 * 否则会上报不成功，因此需要记住上次上报的状态，默认为release
 */
volatile u32 g_u32KnobLeftStateOld  = KEY_STATE_REL;
volatile u32 g_u32KnobRightStateOld = KEY_STATE_REL;

#define MAX_FRAM_LEN   16  /*len of one key frame*/
#define MAX_BUF_ITEM   128 /*the cache size of total key frames*/

struct buf_item
{
    unsigned char len;
    unsigned char data[MAX_FRAM_LEN];
};
struct rx_key_buf{
    struct buf_item  items[MAX_BUF_ITEM];
    unsigned short  head;
    unsigned short  tail;
};
struct sw_uart_pdata {
	unsigned int used;
	unsigned int io_num;
	unsigned int port_no;
	char regulator_id[16];
	struct regulator *regulator;
};

struct sw_uart_port {
	struct uart_port port;
	char   name[16];
	struct clk *mclk;
	struct clk *sclk;
	struct clk *pclk;
	unsigned char id;
	unsigned char ier;
	unsigned char lcr;
	unsigned char mcr;
	unsigned char fcr;
	unsigned char dll;
	unsigned char dlh;
	unsigned char rs485;
	unsigned char msr_saved_flags;
	unsigned int lsr_break_flag;
	struct sw_uart_pdata *pdata;
	struct input_dev *input;

	/* for debug */
#define MAX_DUMP_SIZE	1024
	unsigned int dump_len;
	char* dump_buff;
//	struct proc_dir_entry *proc_root;
	struct proc_dir_entry *proc_info;

	struct pinctrl *pctrl;
	struct serial_rs485 rs485conf;

	struct work_struct	qos_work;
	struct rx_key_buf * rx_buf;
	/* 键盘版本（cpld代码版本） */
	u8      get_keyBoard_ver;
	struct mutex    lock;
};


/********************************************************************
* Code
********************************************************************/
//#define CONFIG_SW_UART_DUMP_DATA
/*
 * ********************* Note **********************
 * CONFIG_SW_UART_DUMP_DATA may cause some problems
 * with some commands of 'dmesg', 'logcat', and
 * 'cat /proc/kmsg' in the console. This problem may
 * cause kernel to dead. These commands will work fine
 * in the adb shell. So you must be very clear with
 * this problem if you want define this macro to debug.
 */

/* debug control */
#define SERIAL_DBG(fmt, arg...)	\
			do { \
				if (sw_uport->port.line != 0) \
					pr_debug("%s()%d - "fmt, __func__, __LINE__, ##arg); \
			} while (0)
#define SERIAL_MSG(fmt, arg...)	pr_warn("%s()%d - "fmt, __func__, __LINE__, ##arg)

#ifdef CONFIG_SW_UART_DUMP_DATA
static void sw_uart_dump_data(struct sw_uart_port *sw_uport, char* prompt)
{
	int i, j;
	int head = 0;
	char* buf = sw_uport->dump_buff;
	u32 len = sw_uport->dump_len;
	static char pbuff[128];
	u32 idx = 0;

	BUG_ON(sw_uport->dump_len > MAX_DUMP_SIZE);
	BUG_ON(!sw_uport->dump_buff);
	#define MAX_DUMP_PER_LINE	(16)
	#define MAX_DUMP_PER_LINE_HALF	(MAX_DUMP_PER_LINE >> 1)
	printk(KERN_DEBUG "%s len %d\n", prompt, len);
	for (i = 0; i < len;) {
		if ((i & (MAX_DUMP_PER_LINE-1)) == 0) {
			idx += sprintf(&pbuff[idx], "%04x: ", i);
			head = i;
		}
		idx += sprintf(&pbuff[idx], "%02x ", buf[i]&0xff);
		if ((i & (MAX_DUMP_PER_LINE-1)) == MAX_DUMP_PER_LINE-1
			|| i==len-1) {
			for (j=i-head+1; j<MAX_DUMP_PER_LINE; j++)
				idx += sprintf(&pbuff[idx], "   ");
			idx += sprintf(&pbuff[idx], " |");
			for (j=head; j<=i; j++) {
				if (isascii(buf[j]) && isprint(buf[j]))
					idx += sprintf(&pbuff[idx], "%c", buf[j]);
				else
					idx += sprintf(&pbuff[idx], ".");
			}
			idx += sprintf(&pbuff[idx], "|\n");
			pbuff[idx] = '\0';
			printk(KERN_DEBUG "%s", pbuff);
			idx = 0;
		}
		i++;
	}
	sw_uport->dump_len = 0;
}
#define SERIAL_DUMP(up, ...) do { \
				if (DEBUG_CONDITION) \
					sw_uart_dump_data(up, __VA_ARGS__); \
			} while (0)
#else
#define SERIAL_DUMP(up, ...)	{up->dump_len = 0;}
#endif

#define UART_TO_SPORT(port)	((struct sw_uart_port*)port)

static inline unsigned char serial_in(struct uart_port *port, int offs)
{
	return readb(port->membase + offs);
}

static inline void serial_out(struct uart_port *port, unsigned char value, int offs)
{
	writeb(value, port->membase + offs);
}

static unsigned int sw_uart_modem_status(struct sw_uart_port *sw_uport)
{
	unsigned int status = serial_in(&sw_uport->port, SUNXI_UART_MSR);

	status |= sw_uport->msr_saved_flags;
	sw_uport->msr_saved_flags = 0;

	if (status & SUNXI_UART_MSR_ANY_DELTA && sw_uport->ier & SUNXI_UART_IER_MSI &&
	    sw_uport->port.state != NULL) {
		if (status & SUNXI_UART_MSR_TERI)
			sw_uport->port.icount.rng++;
		if (status & SUNXI_UART_MSR_DDSR)
			sw_uport->port.icount.dsr++;
		if (status & SUNXI_UART_MSR_DDCD)
			uart_handle_dcd_change(&sw_uport->port, status & SUNXI_UART_MSR_DCD);
		if (!(sw_uport->mcr & SUNXI_UART_MCR_AFE) && status & SUNXI_UART_MSR_DCTS)
			uart_handle_cts_change(&sw_uport->port, status & SUNXI_UART_MSR_CTS);

		wake_up_interruptible(&sw_uport->port.state->port.delta_msr_wait);
	}

	SERIAL_DBG("modem status: %x\n", status);
	return status;
}


#define BOTH_EMPTY    (SUNXI_UART_LSR_TEMT | SUNXI_UART_LSR_THRE)
static inline void wait_for_xmitr(struct sw_uart_port *sw_uport)
{
	unsigned int status, tmout = 10000;
	#ifdef CONFIG_SW_UART_PTIME_MODE
	unsigned int offs = SUNXI_UART_USR;
	unsigned char mask = SUNXI_UART_USR_TFNF;
	#else
	unsigned int offs = SUNXI_UART_LSR;
	unsigned char mask = BOTH_EMPTY;
	#endif

	/* Wait up to 10ms for the character(s) to be sent. */
	do {
		status = serial_in(&sw_uport->port, offs);
		if (serial_in(&sw_uport->port, SUNXI_UART_LSR) & SUNXI_UART_LSR_BI)
			sw_uport->lsr_break_flag = SUNXI_UART_LSR_BI;
		if (--tmout == 0)
			break;
		udelay(1);
	} while ((status & mask) != mask);

	/* CTS is unsupported by the 2-line UART, so ignore it. */
	if (sw_uport->pdata->io_num == 2)
		return;

	/* Wait up to 500ms for flow control if necessary */
	if (sw_uport->port.flags & UPF_CONS_FLOW) {
		tmout = 500000;
		for (tmout = 1000000; tmout; tmout--) {
			unsigned int msr = serial_in(&sw_uport->port, SUNXI_UART_MSR);

			sw_uport->msr_saved_flags |= msr & MSR_SAVE_FLAGS;
			if (msr & SUNXI_UART_MSR_CTS)
				break;

			udelay(1);
		}
	}
}
static inline bool sw_is_console_port(struct uart_port *port)
{
	return port->cons && port->cons->index == port->line;
}

static void sw_console_putchar(struct uart_port *port, int c)
{
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);

	wait_for_xmitr(sw_uport);
	serial_out(port, c, SUNXI_UART_THR);
}
/*
 * uart buadrate and apb2 clock config selection
 * We should select an apb2 clock as low as possible
 * for lower power comsumpition, which can satisfy the
 * different baudrates of different ttyS applications.
 *
 * the reference table as follows:
 * pll6 600M
 * apb2div      0        20       19       18       17       16       15       14       13       12       11       10       9        8        7        6         5
 * apbclk       24000000 30000000 31578947 33333333 35294117 37500000 40000000 42857142 46153846 50000000 54545454 60000000 66666666 75000000 85714285 100000000 120000000
 * 115200            *      *         *        *        *        *        *        *        *        *        *        *        *        *       *         *         *
 * 230400                   *         *        *        *        *        *        *        *        *        *        *        *        *       *         *         *
 * 380400            *      *         *                 *        *                 *        *        *        *        *        *        *       *         *         *
 * 460800                   *                                    *                 *        *        *        *        *        *        *       *         *         *
 * 921600                   *                                                      *        *                          *                 *       *         *         *
 * 1000000                            *        *                                            *        *                          *                          *
 * 1500000           *                                                                      *        *                                   *                 *         *
 * 1750000                                                                                                    *                                  *
 * 2000000                            *        *                                                                                *                          *
 * 2500000                                                                *                                                                                          *
 * 3000000                                                                                  *        *                                                     *
 * 3250000                                                                                                    *                                            *
 * 3500000                                                                                                    *
 * 4000000                                                                                                                      *
 */
struct baudset {
	u32 baud;
	u32 uartclk_min;
	u32 uartclk_max;
};

static inline int sw_uart_check_baudset(struct uart_port *port, unsigned int baud)
{
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);
	static struct baudset  baud_set[] = {
		{115200, 24000000, 120000000},
		{230400, 30000000, 120000000},
		{380400, 24000000, 120000000},
		{460800, 30000000, 120000000},
		{921600, 30000000, 120000000},
		{1000000, 31000000, 120000000}, //31578947
		{1500000, 24000000, 120000000},
		{1750000, 54000000, 120000000}, //54545454
		{2000000, 31000000, 120000000}, //31578947
		{2500000, 40000000, 120000000}, //40000000
		{3000000, 46000000, 120000000}, //46153846
		{3250000, 54000000, 120000000}, //54545454
		{3500000, 54000000, 120000000}, //54545454
		{4000000, 66000000, 120000000}, //66666666
	};
	struct baudset *setsel;
	int i;

	if (baud < 115200) {
		if (port->uartclk < 24000000) {
			SERIAL_MSG("uart%d, uartclk(%d) too small for baud %d\n",
				sw_uport->id, port->uartclk, baud);
			return -1;
		}
	} else {
		for (i=0;
		     i<sizeof(baud_set)/sizeof(baud_set[0]) && baud != baud_set[i].baud;
		     i++);
		if (i==sizeof(baud_set)/sizeof(baud_set[0])) {
			SERIAL_MSG("uart%d, baud %d beyond rance\n", sw_uport->id, baud);
			return -1;
		}
		setsel = &baud_set[i];
		if (port->uartclk < setsel->uartclk_min
			|| port->uartclk > setsel->uartclk_max) {
			SERIAL_MSG("uart%d, select set %d, baud %d, uartclk %d beyond rance[%d, %d]\n",
				sw_uport->id, i, baud, port->uartclk,
				setsel->uartclk_min, setsel->uartclk_max);
			return -1;
		}
	}
	return 0;
}

static void sw_uart_force_lcr(struct sw_uart_port *sw_uport, unsigned msecs)
{
	unsigned long expire = jiffies + msecs_to_jiffies(msecs);
	struct uart_port *port = &sw_uport->port;

	/* hold tx so that uart will update lcr and baud in the gap of rx */
	serial_out(port, SUNXI_UART_HALT_HTX|SUNXI_UART_HALT_FORCECFG, SUNXI_UART_HALT);
	serial_out(port, sw_uport->lcr|SUNXI_UART_LCR_DLAB, SUNXI_UART_LCR);
	serial_out(port, sw_uport->dll, SUNXI_UART_DLL);
	serial_out(port, sw_uport->dlh, SUNXI_UART_DLH);
	serial_out(port, SUNXI_UART_HALT_HTX|SUNXI_UART_HALT_FORCECFG|SUNXI_UART_HALT_LCRUP, SUNXI_UART_HALT);
	while (time_before(jiffies, expire) && (serial_in(port, SUNXI_UART_HALT) & SUNXI_UART_HALT_LCRUP));

	/*
	 * In fact there are two DLABs(DLAB and DLAB_BAK) in the hardware implementation.
	 * The DLAB_BAK is sellected only when SW_UART_HALT_FORCECFG is set to 1,
	 * and this bit can be access no matter uart is busy or not.
	 * So we select the DLAB_BAK always by leaving SW_UART_HALT_FORCECFG to be 1.
	 */
	serial_out(port, sw_uport->lcr, SUNXI_UART_LCR);
	serial_out(port, SUNXI_UART_HALT_FORCECFG, SUNXI_UART_HALT);
}

static void sw_uart_set_termios(struct uart_port *port, struct ktermios *termios,
			    struct ktermios *old)
{
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);
	unsigned long flags;
	unsigned int baud, quot, lcr = 0, dll, dlh;

	SERIAL_DBG("set termios ...\n");
	switch (termios->c_cflag & CSIZE) {
	case CS5:
		lcr |= SUNXI_UART_LCR_WLEN5;
		break;
	case CS6:
		lcr |= SUNXI_UART_LCR_WLEN6;
		break;
	case CS7:
		lcr |= SUNXI_UART_LCR_WLEN7;
		break;
	// default:
	case CS8:

		lcr |= SUNXI_UART_LCR_WLEN8;
		break;
	}

	if (termios->c_cflag & CSTOPB)
		lcr |= SUNXI_UART_LCR_STOP;
	if (termios->c_cflag & PARENB)
		lcr |= SUNXI_UART_LCR_PARITY;
	if (!(termios->c_cflag & PARODD))
		lcr |= SUNXI_UART_LCR_EPAR;

	/* set buadrate */
	baud = uart_get_baud_rate(port, termios, old,
				  port->uartclk / 16 / 0xffff,
				  port->uartclk / 16);
	sw_uart_check_baudset(port, baud);
	quot = uart_get_divisor(port, baud);
	dll = quot & 0xff;
	dlh = quot >> 8;
	SERIAL_DBG("set baudrate %d, quot %d\n", baud, quot);

	spin_lock_irqsave(&port->lock, flags);
	uart_update_timeout(port, termios->c_cflag, baud);

	/* Update the per-port timeout. */
	port->read_status_mask = SUNXI_UART_LSR_OE | SUNXI_UART_LSR_THRE | SUNXI_UART_LSR_DR;
	if (termios->c_iflag & INPCK)
		port->read_status_mask |= SUNXI_UART_LSR_FE | SUNXI_UART_LSR_PE;
	if (termios->c_iflag & (BRKINT | PARMRK))
		port->read_status_mask |= SUNXI_UART_LSR_BI;

	/* Characteres to ignore */
	port->ignore_status_mask = 0;
	if (termios->c_iflag & IGNPAR)
		port->ignore_status_mask |= SUNXI_UART_LSR_PE | SUNXI_UART_LSR_FE;
	if (termios->c_iflag & IGNBRK) {
		port->ignore_status_mask |= SUNXI_UART_LSR_BI;
		/*
		 * If we're ignoring parity and break indicators,
		 * ignore overruns too (for real raw support).
		 */
		if (termios->c_iflag & IGNPAR)
			port->ignore_status_mask |= SUNXI_UART_LSR_OE;
	}

	/*
	 * ignore all characters if CREAD is not set
	 */
	if ((termios->c_cflag & CREAD) == 0)
		port->ignore_status_mask |= SUNXI_UART_LSR_DR;

	/* flow control */
	sw_uport->mcr &= ~SUNXI_UART_MCR_AFE;
	if (termios->c_cflag & CRTSCTS)
		sw_uport->mcr |= SUNXI_UART_MCR_AFE;
	serial_out(port, sw_uport->mcr, SUNXI_UART_MCR);

	/*
	 * CTS flow control flag and modem status interrupts
	 */
	sw_uport->ier &= ~SUNXI_UART_IER_MSI;
	if (UART_ENABLE_MS(port, termios->c_cflag))
		sw_uport->ier |= SUNXI_UART_IER_MSI;
	serial_out(port, sw_uport->ier, SUNXI_UART_IER);

	sw_uport->fcr = SUNXI_UART_FCR_RXTRG_1_2 | SUNXI_UART_FCR_TXTRG_1_2
			| SUNXI_UART_FCR_FIFO_EN;
	serial_out(port, sw_uport->fcr, SUNXI_UART_FCR);

	sw_uport->lcr = lcr;
	sw_uport->dll = dll;
	sw_uport->dlh = dlh;
	sw_uart_force_lcr(sw_uport, 50);

	port->ops->set_mctrl(port, port->mctrl);

	/* Must save the current config for the resume of console(no tty user). */
	if (sw_is_console_port(port))
		port->cons->cflag = termios->c_cflag;

	spin_unlock_irqrestore(&port->lock, flags);

	/* Don't rewrite B0 */
	if (tty_termios_baud_rate(termios))
		tty_termios_encode_baud_rate(termios, baud, baud);
	SERIAL_DBG("termios lcr 0x%x fcr 0x%x mcr 0x%x dll 0x%x dlh 0x%x\n",
			sw_uport->lcr, sw_uport->fcr, sw_uport->mcr,
			sw_uport->dll, sw_uport->dlh);
}

/*******************************************************************************
 * 函   数：drvCRC4OneBit
 * 描   述：CRC4 校验
 * 输入参数：
 *                    参数名称                参数类型                                               参数说明
 * 输出参数：
 *                   参数名称                参数类型                                                参数说明
 * 返 回 值：错误代码，0表示正确
 * 说    明：
 ******************************************************************************/
u8 drvCRC4OneBit(u8 *pu8Buf, u32 u32Len)
{
	u8 u8Crc = 0, *pu8Data, u8Tmp;
	u32 u32DataLen = u32Len * 8 + 4;
	u32 u32Subscripts = 0, u32RightShift = 0;
	u32 u32i;

    //内核申请空间
	pu8Data = (u8*)kmalloc(u32DataLen, GFP_KERNEL);
	//赋值，原数据每一位保存为一个字节
	for(u32i = 0; u32i < u32DataLen - 4; u32i++)
	{
		/* 计算数组的角标和所在的比特位，每8位一个字节
		 * 处以8，商为下角标，与树为比特位（从高算）
		 */
		u32Subscripts = u32i >> 0x3;
		u32RightShift = u32i & 0x7;
		pu8Data[u32i] = !!(pu8Buf[u32Subscripts] & (HIGH_BIT_MASK >> u32RightShift));
	}
	//最后四位补0
	for(u32i = u32DataLen - 4; u32i < u32DataLen; u32i++)
	{
		pu8Data[u32i] = 0;
	}

	//预先取出高4位
	for(u32i = 0; u32i < 4; u32i++)
	{
		u8Crc |= (pu8Data[u32i] & LOW_BIT_MASK) << (3-u32i);
	}

	//计算CRC4
	for(u32i = 4; u32i < u32DataLen; u32i++)
	{
		/* 凑够要计算的5位：
		 * 1.当第一次进入时，u8Crc保存4位，左移一位，然后补上新的低位
		 * 2.当再次进入时，u8Crc是计算后的值，也是只有4位，左移一位补最低位
		 */
		u8Tmp = (u8Crc << 0x1) | (pu8Data[u32i] & LOW_BIT_MASK);
		u8Crc = CHECK_BIT(4, u8Tmp) ? (u8Tmp^CRC4_POLYNOMIAL) : u8Tmp;
	}
	kfree(pu8Data);
	pu8Data = NULL;
    return u8Crc;
}
/*insert the char into the current key frame as soon as we get it*/
/* Sardine的一个键值包需要有9个字节，定义如下
 *   --------------------------------------------------------
 *   | head=0xAA |      key value     |  version  |   CRC   |
 *   |   1Byte   |       6Bytes       |   1Byte   |  1Byte  | 
 *   --------------------------------------------------------
 * 本函数用于接收键值，每次只接收一个字节，当接收9字节时，缓存
 * 并指向下一帧数据存储位置，用于存储下一帧数据
 */
static void insert_key_frame(struct sw_uart_port *up,unsigned char ch)
{
	/* 采用static，原因是每帧数据有多个字节，因此这个函数会被多次调用，
	 * 但只有第一个字节是0xAA，需要将头标志保持
	 */
    static bool key_frame_hd = false;
    int len, position;
    if(ch == KEY_FRAME_HD)/*the start flag of new key frame*/
    {
        key_frame_hd = true;
    }
    if(unlikely(!key_frame_hd))/*should never happen*/
    {
        printk("********* key frame format error!*************\n");
        return;
    }

	//操作的是head位置的item
    position = up->rx_buf->head;
	//读取当前item的数据长度
    len = up->rx_buf->items[position].len;
	//下一个数据位置设置为ch，即为frame_head=0xAA
    up->rx_buf->items[position].data[len]=ch;
	//判断长度+1是否超过帧长
    if((up->rx_buf->items[position].len+1) <= KEY_FRAME_LEN)
    {
		//没超过帧长，长度加一
        up->rx_buf->items[position].len += 1;
        //如果完整的接收到一帧数据，则头指针要前移
        if(up->rx_buf->items[position].len == KEY_FRAME_LEN)
        {
			//head指向下一个位置（循环），该位置长度清0，置标志位false
            up->rx_buf->head = (up->rx_buf->head + 1) & (MAX_BUF_ITEM-1);
            up->rx_buf->items[up->rx_buf->head].len = 0;
            key_frame_hd = false; //只是为了检测，下一帧必须要有帧头
#if KEY_DEBUG
            printk("%s: full key frame: 0x", __func__);
            for (int i = 0; i < KEY_FRAME_LEN;i++ )
            {
                printk("%02x",up->rx_buf->items[position].data[i]);
            }
            printk("\n");
#endif
        }
    }

    return;
}

static void keyboard_uart_write(struct sw_uart_port *up, const char *s,unsigned int count)
{
    unsigned long flags;
    unsigned int ier,i;
    int locked = 1;


    local_irq_save(flags);
    /*
     * First save the IER then disable the interrupts
     */
    ier = serial_in(&up->port, SUNXI_UART_IER);
    serial_out(&up->port, SUNXI_UART_IER, 0);

    for(i = 0; i < count; i++, s++)
    {
        if (*s == '\n')
		{
			sw_console_putchar(&up->port, '\r');
		}
        sw_console_putchar(&up->port, *s);
    }

    /*
     * Finally, wait for transmitter to become empty
     * and restore the IER
     */
    wait_for_xmitr(up);
    serial_out(&up->port, SUNXI_UART_IER, ier);
	/*
	 * The receive handling will happen properly because the
	 * receive ready bit will still be set; it is not cleared
	 * on read.  However, modem control will not, we must
	 * call it if we have saved something in the saved flags
	 * while processing with interrupts off.
	 */
	if (up->msr_saved_flags)
	{
		sw_uart_modem_status(up);
	}
		
    if (locked)
        spin_unlock(&up->port.lock);
    local_irq_restore(flags);
}

/* 键盘事件函数，该函数用于控制按键背景LED灯
 * 用此函数控制的灯必须是连接在CPLD上，counter后的灯是直接使用ARM的pwm控制，不包括在内
 * 该函数是由应用层代码触发的，应用层调用write函数写入input_event数据，本函数解析
 * 应用层的事件的注释如下：
 * @event: event handler for events sent _to_ the device, like EV_LED
 *	or EV_SND. The device is expected to carry out the requested
 *	action (turn on a LED, play sound, etc.) The call is protected
 *	by @event_lock and must not sleep
 * Sardine项目中只包括"获取键盘板的版本号"一个事件
 */
static int rigol_kbd_event(struct input_dev *input, unsigned int type, unsigned int code, int value)
{
    struct sw_uart_port *up = input_get_drvdata(input);

	/* 判断应用层指令类型，EV_LED事件包括设置LED和获取版本号，命令一个字节，高4位为CPLD寄存器地址
     * 低4位为数据，在点亮LED命令中第4位表示要设置的LED的状态
     *    - 获取版本号（value=0xF0），直接将指令写下去，CPLD会自动返回版本号数据帧
     *    - 设置LED直接写下去，没有返回
	 */
    if(type == EV_LED)
    {
        //当寄存器地址（高4位）为0xF时，低4位无效，判断为获取版本号
        //当寄存器地址为其他情况时，即高4位为其他值，低4位此时表示LED的状态
        if((value & 0xF0) == 0xF0)
        {
            up->get_keyBoard_ver = 1;
        }
//        key_debug("type = EV_LED, code = 0x%x, value = 0x%x\n", code, value);
        keyboard_uart_write(up, (u8*)&value, 1);
    }
    return 0;
}
/* 判断旋钮用的函数
 * 逆时针（左）：11->01->00->10->11
 * 顺时针（右）：11->10->00->01->11
 */
static u16 rigol_kbd_knob_value_get(u64 u64KeyValue)
{
	u8   u8KnobValueOld = 0;
	u8   u8KnobValueNew = 0;
    u16  u16KeyValue    = RIGOL_KBD_KEY_INVALID;
    static s16 s16Left  = 0;
    static s16 s16Right = 0;

    u8KnobValueOld = KEY_KNOB_GET_VALUE(g_u64KeyValueOld);
    u8KnobValueNew = KEY_KNOB_GET_VALUE(u64KeyValue);

	//解决旋钮缓慢旋转时，偶尔出现步进为2的问题；宁愿偶尔旋一下不响应，也不能旋一下走两下。
    switch (u8KnobValueOld)
    {
		case 3:
			if(u8KnobValueNew == 1)
            {
                s16Left++;
            }
			else if(u8KnobValueNew == 2)
            {
                s16Right++;
            }
			break;
        case 2:
			if(u8KnobValueNew == 3)
			{
				s16Left++;
			}
			else if(u8KnobValueNew == 0)
			{
				s16Right++;
			}
            break;
		case 0:
			if(u8KnobValueNew == 2)
			{
				s16Left++;
			}
			else if(u8KnobValueNew == 1)
			{
				s16Right++;
			}
            break;
		case 1:
			if(u8KnobValueNew == 0)
			{
				s16Left++;
			}
			else if(u8KnobValueNew == 3)
			{
				s16Right++;
			}
			break;
	}

    //旋钮完成一次方向识别
	if(s16Left == KEY_KNOB_IDENTIFY_DIR_STEP)
    {
        s16Right = 0;
        s16Left  = 0;
        u16KeyValue = RIGOL_KBD_KEY_KNOB_LEFT;
    }
    else if(s16Right == KEY_KNOB_IDENTIFY_DIR_STEP)
    {
        s16Right = 0;
        s16Left  = 0;
        u16KeyValue = RIGOL_KBD_KEY_KNOB_RIGHT;
    }

    return (u16KeyValue);
}

/*******************************************************************************
 函 数 名: rigol_kbd_parse_key
 描    述:
 输入参数:
 输出参数：
 返 回 值:
 说    明：if key is changed,return 1. if no key is changed,return 0
*******************************************************************************/
static int rigol_kbd_parse_key(u64 *key, unsigned int *keycode, unsigned int *keystate)
{
    s32 s32Ret = -1;
	int i = 0;
    u64 u64KeyValueNew = *key;
	//compare to last key,and detect the key state(down or release,or not changed)
    u64 u64KeyMask = u64KeyValueNew ^ g_u64KeyValueOld;

	//两次按键值没有改变，说明是重复数据（按下和抬起的键值不同）
	if(u64KeyMask == 0)
    {
		return s32Ret;
    }

    //1->0 means down,0->1 means release
    *keystate = (u64KeyValueNew & u64KeyMask)?KEY_STATE_REL:KEY_STATE_DN;
	*keycode  = RIGOL_KBD_KEY_INVALID;

	//如果不是旋钮则判断是否为按键
	for(i = 0; i < KEY_COUNT; i++)
	{
		if(u64KeyMask == rigol_keycode_bits[i])
		{
			s32Ret = 0;
			*keycode = rigol_keycodes[i];

    		// set current key value as last key value.
    		g_u64KeyValueOld = *key;
			return s32Ret;
		}
	}

	//判断是否为旋钮，判断依据是：检测旧值与新值比较变化的位是否为knob的掩码（两个）
	if(u64KeyMask == KEY_MASK_KNOB_1 || u64KeyMask == KEY_MASK_KNOB_2)
	{
		//判断为旋钮
		s32Ret = 0;
		//key_debug("knob: 0x%llx-0x%llx!\n", u64KeyValueNew, u64KeyMask);
		*keycode = rigol_kbd_knob_value_get(u64KeyValueNew);
	}
	else
	{
//		key_debug("INVALID key!\n");
	}

    // set current key value as last key value.
    g_u64KeyValueOld = *key;
    return s32Ret;
}
/*******************************************************************************
 函 数 名: rigol_kbd_get_and_report_key
 描    述: get the key value which is changed,and report the event to user.
 输入参数:
 输出参数：
 返 回 值:
 说    明：
*******************************************************************************/
static int rigol_kbd_get_and_report_key(struct sw_uart_port *up, u8 *keybuf)
{
    int ret = 1;
    u32 keycode, keystate;
    u64 keyTmp  = 0;
    u8  version = 0;
    memcpy(&keyTmp, keybuf, KEY_VALID_LEN);

	//判断是发送版本号还是发送键值
    if(up->get_keyBoard_ver == 1)
    {
		//发送版本号
        version = keybuf[KEY_VALID_LEN];
    	key_debug("get version = 0x%x!\n", version);
        keycode = (u32)0x0F;           //code
        keystate = (u32)version<<8;    //value
        input_event(up->input, EV_KEY, keycode, keystate);
        input_event(up->input, EV_KEY, keycode, 0);
        input_sync(up->input);
        up->get_keyBoard_ver = 0;
        return 1;
    }
    else
    {
		//发送键值
        /* mask the unused key first */
        keyTmp |= KEY_UNUSED_MASK;
        ret = rigol_kbd_parse_key(&keyTmp, &keycode, &keystate);
        if(!ret && keycode != RIGOL_KBD_KEY_INVALID)
        {
			//判断是否为旋钮，判断依据是：检测旧值与新值比较变化的位是否为knob的掩码（两个）
			if(keycode == RIGOL_KBD_KEY_KNOB_LEFT)
			{
				//判断为旋钮
				key_debug("knob: %s\n", "left");
				keystate = g_u32KnobLeftStateOld==KEY_STATE_REL?KEY_STATE_DN:KEY_STATE_REL;
				g_u32KnobLeftStateOld = keystate;
			}
			else if(keycode == RIGOL_KBD_KEY_KNOB_RIGHT)
			{
				//判断为旋钮
				key_debug("knob: %s\n", "right");
				keystate = g_u32KnobRightStateOld==KEY_STATE_REL?KEY_STATE_DN:KEY_STATE_REL;
				g_u32KnobRightStateOld = keystate;
			}
			else
			{
				for(int i = 0; i < KEY_COUNT; i++)
				{
					if((keycode)==rigol_keycodes[i])
					{
//						key_debug("%s button, %s\n", rigol_keycodes_debug_string[i],
//														 keystate?"down":"release");
						break;
					}
				}
			}

			//上报按键值
            input_report_key(up->input, keycode, keystate);
            input_sync(up->input);
        }
    }
    return ret;
}
/*******************************************************************************
 函 数 名: rigol_kbd_report
 描    述: task queue of reporting key event .
 输入参数:
 输出参数：
 返 回 值:
 说    明：
*******************************************************************************/
static void rigol_kbd_qos_work(struct work_struct *work )
{
    struct sw_uart_port *up = container_of(work, struct sw_uart_port, qos_work);
    u8 key_fram[KEY_FRAME_LEN] = {0};
	u8 crc_value;

    mutex_lock(&up->lock);
	//head != tail时说明有新数据
    while((up->rx_buf->head - up->rx_buf->tail) & (MAX_BUF_ITEM-1))
    {
		//计算CRC4校验值
		crc_value = drvCRC4OneBit(&(up->rx_buf->items[up->rx_buf->tail].data[0]), KEY_CRC4_POS);
    	//判断帧头是否为0xAA，并判断校验值是否正确
        if(up->rx_buf->items[up->rx_buf->tail].data[0] == KEY_FRAME_HD &&
			crc_value == up->rx_buf->items[up->rx_buf->tail].data[KEY_CRC4_POS])
        {
        	//key_debug("crc = %02x\n", crc_value);
			//拷贝除帧头部分的数据（带上版本号）
            memcpy(key_fram, &up->rx_buf->items[up->rx_buf->tail].data[1], KEY_VALID_LEN+1);
            rigol_kbd_get_and_report_key(up, key_fram);

#if PRINT_KEY_VALUE
            printk("%s: key frame: 0x", __func__);
            for (int i = 0; i < KEY_FRAME_LEN; i++)
            {
                printk("%02x", key_fram[i]);
            }
            printk("\n");
#endif
        }

        up->rx_buf->items[up->rx_buf->tail].len = 0;
        up->rx_buf->tail = (up->rx_buf->tail + 1)&(MAX_BUF_ITEM-1);
    }
    mutex_unlock(&up->lock);
    return;
}
/* init uart and start rx. */
static void keyboard_uart_init(struct sw_uart_port *up)
{
    struct ktermios termios;
    static struct ktermios dummy;

//    serial_sw_startup(up);//start first and then set the termios.xyzheng

    memset(&termios, 0, sizeof(struct ktermios));
    termios.c_cflag = CREAD | HUPCL | CLOCAL;
    termios.c_cflag |= CS8; //8bits
    termios.c_cflag |= B1000000; //1M
    sw_uart_set_termios(&up->port,&termios,&dummy);
}


static inline void sw_uart_reset(struct sw_uart_port *sw_uport)
{
	sunxi_periph_reset_assert(sw_uport->mclk);
	sunxi_periph_reset_deassert(sw_uport->mclk);
}

static unsigned int sw_uart_handle_rx(struct sw_uart_port *sw_uport, unsigned int lsr)
{
	unsigned char ch = 0;
	int max_count = 256;
	char flag;

	do {
		if (likely(lsr & SUNXI_UART_LSR_DR)) {
			ch = serial_in(&sw_uport->port, SUNXI_UART_RBR);
#ifdef CONFIG_SW_UART_DUMP_DATA
			sw_uport->dump_buff[sw_uport->dump_len++] = ch;
#endif
		}

		flag = TTY_NORMAL;
		sw_uport->port.icount.rx++;

		if (unlikely(lsr & SUNXI_UART_LSR_BRK_ERROR_BITS)) {
			/*
			 * For statistics only
			 */
			if (lsr & SUNXI_UART_LSR_BI) {
				lsr &= ~(SUNXI_UART_LSR_FE | SUNXI_UART_LSR_PE);
				sw_uport->port.icount.brk++;
				/*
				 * We do the SysRQ and SAK checking
				 * here because otherwise the break
				 * may get masked by ignore_status_mask
				 * or read_status_mask.
				 */
				if (uart_handle_break(&sw_uport->port))
					goto ignore_char;
			} else if (lsr & SUNXI_UART_LSR_PE)
				sw_uport->port.icount.parity++;
			else if (lsr & SUNXI_UART_LSR_FE)
				sw_uport->port.icount.frame++;
			if (lsr & SUNXI_UART_LSR_OE)
				sw_uport->port.icount.overrun++;

			/*
			 * Mask off conditions which should be ignored.
			 */
			lsr &= sw_uport->port.read_status_mask;
#ifdef CONFIG_SERIAL_SUNXI_CONSOLE
			if (sw_is_console_port(&sw_uport->port)) {
				/* Recover the break flag from console xmit */
				lsr |= sw_uport->lsr_break_flag;
			}
#endif
			if (lsr & SUNXI_UART_LSR_BI)
				flag = TTY_BREAK;
			else if (lsr & SUNXI_UART_LSR_PE)
				flag = TTY_PARITY;
			else if (lsr & SUNXI_UART_LSR_FE)
				flag = TTY_FRAME;
		}
		if (uart_handle_sysrq_char(&sw_uport->port, ch))
			goto ignore_char;
		uart_insert_char(&sw_uport->port, lsr, SUNXI_UART_LSR_OE, ch, flag);
ignore_char:
		lsr = serial_in(&sw_uport->port, SUNXI_UART_LSR);
	} while ((lsr & (SUNXI_UART_LSR_DR | SUNXI_UART_LSR_BI)) && (max_count-- > 0));

	SERIAL_DUMP(sw_uport, "Rx");
	spin_unlock(&sw_uport->port.lock);
	tty_flip_buffer_push(&sw_uport->port.state->port);
	spin_lock(&sw_uport->port.lock);

	return lsr;
}

static void sw_uart_stop_tx(struct uart_port *port)
{
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);

	if (sw_uport->ier & SUNXI_UART_IER_THRI) {
		sw_uport->ier &= ~SUNXI_UART_IER_THRI;
		SERIAL_DBG("stop tx, ier %x\n", sw_uport->ier);
		serial_out(port, sw_uport->ier, SUNXI_UART_IER);
	}
}

static void sw_uart_start_tx(struct uart_port *port)
{
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);

	if (!(sw_uport->ier & SUNXI_UART_IER_THRI)) {
		sw_uport->ier |= SUNXI_UART_IER_THRI;
		SERIAL_DBG("start tx, ier %x\n", sw_uport->ier);
		serial_out(port, sw_uport->ier, SUNXI_UART_IER);
	}
}

static void sw_uart_handle_tx(struct sw_uart_port *sw_uport)
{
	struct circ_buf *xmit = &sw_uport->port.state->xmit;
	int count;

	if (sw_uport->port.x_char) {
		serial_out(&sw_uport->port, sw_uport->port.x_char, SUNXI_UART_THR);
		sw_uport->port.icount.tx++;
		sw_uport->port.x_char = 0;
#ifdef CONFIG_SW_UART_DUMP_DATA
		sw_uport->dump_buff[sw_uport->dump_len++] = sw_uport->port.x_char;
		SERIAL_DUMP(sw_uport, "Tx");
#endif
		return;
	}
	if (uart_circ_empty(xmit) || uart_tx_stopped(&sw_uport->port)) {
		sw_uart_stop_tx(&sw_uport->port);
		return;
	}
	count = sw_uport->port.fifosize / 2;
	do {
#ifdef CONFIG_SW_UART_DUMP_DATA
		sw_uport->dump_buff[sw_uport->dump_len++] = xmit->buf[xmit->tail];
#endif
		serial_out(&sw_uport->port, xmit->buf[xmit->tail], SUNXI_UART_THR);
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		sw_uport->port.icount.tx++;
		if (uart_circ_empty(xmit)) {
			break;
		}
	} while (--count > 0);

	SERIAL_DUMP(sw_uport, "Tx");
	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS) {
		spin_unlock(&sw_uport->port.lock);
		uart_write_wakeup(&sw_uport->port);
		spin_lock(&sw_uport->port.lock);
	}
	if (uart_circ_empty(xmit))
		sw_uart_stop_tx(&sw_uport->port);
}



static void sw_uart_force_idle(struct sw_uart_port *sw_uport)
{
	struct uart_port *port = &sw_uport->port;

	if (sw_uport->fcr & SUNXI_UART_FCR_FIFO_EN) {
		serial_out(port, SUNXI_UART_FCR_FIFO_EN, SUNXI_UART_FCR);
		serial_out(port, SUNXI_UART_FCR_TXFIFO_RST
				| SUNXI_UART_FCR_RXFIFO_RST
				| SUNXI_UART_FCR_FIFO_EN, SUNXI_UART_FCR);
		serial_out(port, 0, SUNXI_UART_FCR);
	}

	serial_out(port, sw_uport->fcr, SUNXI_UART_FCR);
	(void)serial_in(port, SUNXI_UART_FCR);
}

/*
 * We should clear busy interupt, busy state and reset lcr,
 * but we should be careful not to introduce a new busy interrupt.
 */
static void sw_uart_handle_busy(struct sw_uart_port *sw_uport)
{
	struct uart_port *port = &sw_uport->port;

	(void)serial_in(port, SUNXI_UART_USR);

	/*
	 * Before reseting lcr, we should ensure than uart is not in busy
	 * state. Otherwise, a new busy interrupt will be introduced.
	 * It is wise to set uart into loopback mode, since it can cut down the
	 * serial in, then we should reset fifo(in my test, busy state
	 * (SUNXI_UART_USR_BUSY) can't be cleard until the fifo is empty).
	 */
	serial_out(port, sw_uport->mcr | SUNXI_UART_MCR_LOOP, SUNXI_UART_MCR);
	sw_uart_force_idle(sw_uport);
	serial_out(port, sw_uport->lcr, SUNXI_UART_LCR);
	serial_out(port, sw_uport->mcr, SUNXI_UART_MCR);
}

static irqreturn_t sw_uart_irq(int irq, void *dev_id)
{
	struct uart_port *port = dev_id;
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);
	unsigned int iir = 0, lsr = 0;
	unsigned long flags;

	spin_lock_irqsave(&port->lock, flags);

	iir = serial_in(port, SUNXI_UART_IIR) & SUNXI_UART_IIR_IID_MASK;
	lsr = serial_in(port, SUNXI_UART_LSR);
	SERIAL_DBG("irq: iir %x lsr %x\n", iir, lsr);

	if (iir == SUNXI_UART_IIR_IID_BUSBSY) {
		sw_uart_handle_busy(sw_uport);
	} else {
		if (lsr & (SUNXI_UART_LSR_DR | SUNXI_UART_LSR_BI))
			lsr = sw_uart_handle_rx(sw_uport, lsr);
		/* has charto irq but no dr lsr? just read and ignore */
		else if (iir & SUNXI_UART_IIR_IID_CHARTO)
			serial_in(&sw_uport->port, SUNXI_UART_RBR);
		sw_uart_modem_status(sw_uport);
		#ifdef CONFIG_SW_UART_PTIME_MODE
		if (iir == SUNXI_UART_IIR_IID_THREMP)
		#else
		if (lsr & SUNXI_UART_LSR_THRE)
		#endif
			sw_uart_handle_tx(sw_uport);
	}

	spin_unlock_irqrestore(&port->lock, flags);

	return IRQ_HANDLED;
}


/* Enable or disable the RS485 support */
static void sw_uart_config_rs485(struct uart_port *port, struct serial_rs485 *rs485conf)
{
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);

	sw_uport->rs485conf = *rs485conf;

	sw_uport->mcr &= ~SUNXI_UART_MCR_MODE_MASK;
	if (rs485conf->flags & SER_RS485_ENABLED) {
		SERIAL_DBG("setting to rs485\n");
		sw_uport->mcr |= SUNXI_UART_MCR_MODE_RS485;

		/*
		 * In NMM mode and no 9th bit(default RS485 mode), uart receive
		 * all the bytes into FIFO before receveing an address byte
		 */
		sw_uport->rs485 |= SUNXI_UART_RS485_RXBFA;
	} else {
		SERIAL_DBG("setting to uart\n");
		sw_uport->mcr |= SUNXI_UART_MCR_MODE_UART;
		sw_uport->rs485 = 0;
	}

	serial_out(port, sw_uport->mcr, SUNXI_UART_MCR);
	serial_out(port, sw_uport->rs485, SUNXI_UART_RS485);
}

static unsigned int sw_uart_tx_empty(struct uart_port *port)
{
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);
	unsigned long flags = 0;
	unsigned int ret = 0;

	spin_lock_irqsave(&sw_uport->port.lock, flags);
	ret = (serial_in(port, SUNXI_UART_USR) & SUNXI_UART_USR_TFE) ? TIOCSER_TEMT : 0;
	spin_unlock_irqrestore(&sw_uport->port.lock, flags);
	return ret;
}

static void sw_uart_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);
	unsigned int mcr = 0;

	if (mctrl & TIOCM_RTS)
		mcr |= SUNXI_UART_MCR_RTS;
	if (mctrl & TIOCM_DTR)
		mcr |= SUNXI_UART_MCR_DTR;
	if (mctrl & TIOCM_LOOP)
		mcr |= SUNXI_UART_MCR_LOOP;
	sw_uport->mcr &= ~(SUNXI_UART_MCR_RTS|SUNXI_UART_MCR_DTR|SUNXI_UART_MCR_LOOP);
	sw_uport->mcr |= mcr;
	SERIAL_DBG("set mcr %x\n", mcr);
	serial_out(port, sw_uport->mcr, SUNXI_UART_MCR);
}

static unsigned int sw_uart_get_mctrl(struct uart_port *port)
{
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);
	unsigned int msr;
	unsigned int ret = 0;

	msr = sw_uart_modem_status(sw_uport);
	if (msr & SUNXI_UART_MSR_DCD)
		ret |= TIOCM_CAR;
	if (msr & SUNXI_UART_MSR_RI)
		ret |= TIOCM_RNG;
	if (msr & SUNXI_UART_MSR_DSR)
		ret |= TIOCM_DSR;
	if (msr & SUNXI_UART_MSR_CTS)
		ret |= TIOCM_CTS;
	SERIAL_DBG("get msr %x\n", msr);
	return ret;
}

static void sw_uart_stop_rx(struct uart_port *port)
{
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);

	if (sw_uport->ier & SUNXI_UART_IER_RLSI) {
		sw_uport->ier &= ~SUNXI_UART_IER_RLSI;
		SERIAL_DBG("stop rx, ier %x\n", sw_uport->ier);
		sw_uport->port.read_status_mask &= ~SUNXI_UART_LSR_DR;
		serial_out(port, sw_uport->ier, SUNXI_UART_IER);
	}
}

static void sw_uart_enable_ms(struct uart_port *port)
{
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);

	if (!(sw_uport->ier & SUNXI_UART_IER_MSI)) {
		sw_uport->ier |= SUNXI_UART_IER_MSI;
		SERIAL_DBG("en msi, ier %x\n", sw_uport->ier);
		serial_out(port, sw_uport->ier, SUNXI_UART_IER);
	}
}

static void sw_uart_break_ctl(struct uart_port *port, int break_state)
{
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);
	unsigned long flags;

	spin_lock_irqsave(&port->lock, flags);
	if (break_state == -1)
		sw_uport->lcr |= SUNXI_UART_LCR_SBC;
	else
		sw_uport->lcr &= ~SUNXI_UART_LCR_SBC;
	serial_out(port, sw_uport->lcr, SUNXI_UART_LCR);
	spin_unlock_irqrestore(&port->lock, flags);
}

static int sw_uart_startup(struct uart_port *port)
{
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);
	int ret;

	SERIAL_DBG("start up ...\n");

	ret = request_irq(port->irq, sw_uart_irq, 0, sw_uport->name, port);
	if (unlikely(ret)) {
		SERIAL_MSG("uart%d cannot get irq %d\n", sw_uport->id, port->irq);
		return ret;
	}

	sw_uport->msr_saved_flags = 0;
	/*
	 * PTIME mode to select the THRE trigger condition:
	 * if PTIME=1(IER[7]), the THRE interrupt will be generated when the
	 * the water level of the TX FIFO is lower than the threshold of the
	 * TX FIFO. and if PTIME=0, the THRE interrupt will be generated when
	 * the TX FIFO is empty.
	 * In addition, when PTIME=1, the THRE bit of the LSR register will not
	 * be set when the THRE interrupt is generated. You must check the
	 * interrupt id of the IIR register to decide whether some data need to
	 * send.
	 */
	sw_uport->ier = SUNXI_UART_IER_RLSI | SUNXI_UART_IER_RDI;
	#ifdef CONFIG_SW_UART_PTIME_MODE
	sw_uport->ier |= SUNXI_UART_IER_PTIME;
	#endif

	sw_uart_config_rs485(port, &sw_uport->rs485conf);

	return 0;
}

static void sw_uart_shutdown(struct uart_port *port)
{
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);

	SERIAL_DBG("shut down ...\n");
	sw_uport->ier = 0;
	sw_uport->lcr = 0;
	sw_uport->mcr = 0;
	sw_uport->fcr = 0;
	serial_out(port, sw_uport->ier, SUNXI_UART_IER);
	free_irq(port->irq, port);
}

static void sw_uart_flush_buffer(struct uart_port *port)
{
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);

	SERIAL_DBG("flush buffer...\n");
	serial_out(port, sw_uport->fcr|SUNXI_UART_FCR_TXFIFO_RST, SUNXI_UART_FCR);
}


static const char *sw_uart_type(struct uart_port *port)
{
	return "SUNXI";
}

static int sw_uart_select_gpio_state(struct pinctrl *pctrl, char *name, u32 no)
{
	int ret = 0;
	struct pinctrl_state *pctrl_state = NULL;

	pctrl_state = pinctrl_lookup_state(pctrl, name);
	if (IS_ERR(pctrl_state)) {
		SERIAL_MSG("UART%d pinctrl_lookup_state(%s) failed! return %p \n", no, name, pctrl_state);
		return -1;
	}

	ret = pinctrl_select_state(pctrl, pctrl_state);
	if (ret < 0)
		SERIAL_MSG("UART%d pinctrl_select_state(%s) failed! return %d \n", no, name, ret);

	return ret;
}

static int sw_uart_request_gpio(struct sw_uart_port *sw_uport)
{
	sw_uport->pctrl = devm_pinctrl_get(sw_uport->port.dev);

	if (IS_ERR_OR_NULL(sw_uport->pctrl)) {
		SERIAL_MSG("UART%d devm_pinctrl_get() failed! return %ld\n", sw_uport->id, PTR_ERR(sw_uport->pctrl));
		return -1;
	}

	return sw_uart_select_gpio_state(sw_uport->pctrl, PINCTRL_STATE_DEFAULT, sw_uport->id);
}

static void sw_uart_release_gpio(struct sw_uart_port *sw_uport)
{
	devm_pinctrl_put(sw_uport->pctrl);
	sw_uport->pctrl = NULL;
}

static void sw_uart_release_port(struct uart_port *port)
{
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);
	struct platform_device *pdev;
	struct resource	*mem_res;

	SERIAL_DBG("release port(iounmap & release io)\n");

	pdev = to_platform_device(port->dev);
	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (mem_res == NULL) {
		SERIAL_MSG("uart%d, get MEM resource failed\n", sw_uport->id);
		return;
	}

	/* release memory resource */
	release_mem_region(mem_res->start, resource_size(mem_res));
	iounmap(port->membase);
	port->membase = NULL;

	/* release io resource */
	sw_uart_release_gpio(sw_uport);
}

static int sw_uart_request_port(struct uart_port *port)
{
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);
	struct platform_device *pdev;
	struct resource	*mem_res;
	int ret;

	SERIAL_DBG("request port(ioremap & request io) %d\n", sw_uport->id);

	pdev = to_platform_device(port->dev);
	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (mem_res == NULL) {
		SERIAL_MSG("uart%d, get MEM resource failed\n", sw_uport->id);
		ret = -ENXIO;
	}

	/* request memory resource */
	if (!request_mem_region(mem_res->start, resource_size(mem_res), SUNXI_UART_DEV_NAME)) {
		SERIAL_MSG("uart%d, request mem region failed\n", sw_uport->id);
		return -EBUSY;
	}

	port->membase = ioremap(mem_res->start, resource_size(mem_res));
	if (!port->membase) {
		SERIAL_MSG("uart%d, ioremap failed\n", sw_uport->id);
		release_mem_region(mem_res->start, resource_size(mem_res));
		return -EBUSY;
	}

	/* request io resource */
	ret = sw_uart_request_gpio(sw_uport);
	if (ret < 0) {
		release_mem_region(mem_res->start, resource_size(mem_res));
		return ret;
	}

	return 0;
}

static void sw_uart_config_port(struct uart_port *port, int flags)
{
	int ret;

	if (flags & UART_CONFIG_TYPE) {
		port->type = PORT_SUNXI;
		ret = sw_uart_request_port(port);
		if (ret)
			return;
	}
}

static int sw_uart_verify_port(struct uart_port *port, struct serial_struct *ser)
{
	if (unlikely(ser->type != PORT_UNKNOWN && ser->type != PORT_SUNXI))
		return -EINVAL;
	if (unlikely(port->irq != ser->irq))
		return -EINVAL;
	return 0;
}

static int sw_uart_ioctl(struct uart_port *port, unsigned int cmd,
			 unsigned long arg)
{
	struct serial_rs485 rs485conf;
	unsigned long flags = 0;


	switch (cmd) {
	case TIOCSRS485:
		if (copy_from_user(&rs485conf, (struct serial_rs485 *)arg,
				   sizeof(rs485conf)))
			return -EFAULT;

		spin_lock_irqsave(&port->lock, flags);
		sw_uart_config_rs485(port, &rs485conf);
		spin_unlock_irqrestore(&port->lock, flags);
		break;

	case TIOCGRS485:
		if (copy_to_user((struct serial_rs485 *) arg,
				 &(UART_TO_SPORT(port)->rs485conf),
				 sizeof(rs485conf)))
			return -EFAULT;
		break;

	// default:
	// 	return -ENOIOCTLCMD;
	}

	return 0;
}

static void sw_uart_pm(struct uart_port *port, unsigned int state,
		      unsigned int oldstate)
{
#ifdef CONFIG_EVB_PLATFORM
	int ret;
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);

	SERIAL_DBG("PM state %d -> %d\n", oldstate, state);

	switch (state) {
	case 0: /* Power up */
		if (sw_uport->mclk->enable_count > 0) {
			SERIAL_MSG("uart%d clk is already enable\n", sw_uport->id);
			break;
		}

		ret = clk_prepare_enable(sw_uport->mclk);
		if (ret) {
			SERIAL_MSG("uart%d release reset failed\n", sw_uport->id);
		}
		break;
	case 3: /* Power down */
		if (sw_uport->mclk->enable_count == 0) {
			SERIAL_MSG("uart%d clk is already disable\n", sw_uport->id);
			break;
		}

		clk_disable_unprepare(sw_uport->mclk);
		break;
	// default；
	// 	SERIAL_MSG("uart%d, Unknown PM state %d\n", sw_uport->id, state);
	}
#endif
}

#ifdef CONFIG_CONSOLE_POLL
/*
 * Console polling routines for writing and reading from the uart while
 * in an interrupt or debug context.
 */

static int sw_get_poll_char(struct uart_port *port)
{
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);
	unsigned int lsr = serial_in(port, SUNXI_UART_LSR);

	if (!(lsr & SUNXI_UART_LSR_DR)) {
		return NO_POLL_CHAR;
	}

	return serial_in(port, SUNXI_UART_RBR);
}


static void sw_put_poll_char(struct uart_port *port,
			unsigned char c)
{
	unsigned int ier;
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);

	/*
	 * First save the IER then disable the interrupts.
	 */
	ier = serial_in(port, SUNXI_UART_IER);

	serial_out(port, 0, SUNXI_UART_IER);
	wait_for_xmitr(sw_uport);

	serial_out(port, c, SUNXI_UART_THR);
	if (c == 10) {
		wait_for_xmitr(sw_uport);
		serial_out(port, 13, SUNXI_UART_THR);
	}
	/*
	 * Finally, wait for transmitter to become empty
	 * and restore the IER
	 */
	wait_for_xmitr(sw_uport);
	serial_out(port, ier, SUNXI_UART_IER);
}

#endif /* CONFIG_CONSOLE_POLL */


static struct uart_ops sw_uart_ops = {
	.tx_empty = sw_uart_tx_empty,
	.set_mctrl = sw_uart_set_mctrl,
	.get_mctrl = sw_uart_get_mctrl,
	.stop_tx = sw_uart_stop_tx,
	.start_tx = sw_uart_start_tx,
	.stop_rx = sw_uart_stop_rx,
	.enable_ms = sw_uart_enable_ms,
	.break_ctl = sw_uart_break_ctl,
	.startup = sw_uart_startup,
	.shutdown = sw_uart_shutdown,
	.flush_buffer = sw_uart_flush_buffer,
	.set_termios = sw_uart_set_termios,
	.type = sw_uart_type,
	.release_port = sw_uart_release_port,
	.request_port = sw_uart_request_port,
	.config_port = sw_uart_config_port,
	.verify_port = sw_uart_verify_port,
	.ioctl = sw_uart_ioctl,
	.pm = sw_uart_pm,
#ifdef CONFIG_CONSOLE_POLL
	.poll_get_char = sw_get_poll_char,
	.poll_put_char = sw_put_poll_char,
#endif
};

static int sw_uart_regulator_request(struct sw_uart_port* sw_uport, struct sw_uart_pdata *pdata)
{
	struct regulator *regu = NULL;

	/* Consider "n***" as nocare. Support "none", "nocare", "null", "" etc. */
	if ((pdata->regulator_id[0] == 'n') || (pdata->regulator_id[0] == 0))
		return 0;

	regu = regulator_get(NULL, pdata->regulator_id);
	if (IS_ERR(regu)) {
		SERIAL_MSG("get regulator %s failed!\n", pdata->regulator_id);
		return -1;
	}
	pdata->regulator = regu;
	return 0;
}

static void sw_uart_regulator_release(struct sw_uart_pdata *pdata)
{
	if (pdata->regulator == NULL)
		return;

	regulator_put(pdata->regulator);
	pdata->regulator = NULL;
}

static int sw_uart_regulator_enable(struct sw_uart_pdata *pdata)
{
	if (pdata->regulator == NULL)
		return 0;

	if (regulator_enable(pdata->regulator) != 0)
		return -1;

	return 0;
}

static int sw_uart_regulator_disable(struct sw_uart_pdata *pdata)
{
	if (pdata->regulator == NULL)
		return 0;

	if (regulator_disable(pdata->regulator) != 0)
		return -1;

	return 0;
}

static struct sw_uart_port sw_uart_ports[SUNXI_UART_NUM];
static struct sw_uart_pdata sw_uport_pdata[SUNXI_UART_NUM];

static ssize_t sunxi_uart_dev_info_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct uart_port *port = dev_get_drvdata(dev);
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);
	struct sw_uart_pdata *pdata = (struct sw_uart_pdata *)dev->platform_data;

	return snprintf(buf, PAGE_SIZE,
	   	"id     = %d \n"
	   	"name   = %s \n"
	   	"irq    = %d \n"
	   	"io_num = %d \n"
		"port->mapbase = %pa \n"
	   	"port->membase = 0x%p \n"
		"port->iobase  = 0x%08lx \n"
		"pdata->regulator    = 0x%p \n"
		"pdata->regulator_id = %s \n",
		sw_uport->id, sw_uport->name, port->irq,
		sw_uport->pdata->io_num,
		&port->mapbase, port->membase, port->iobase,
		pdata->regulator, pdata->regulator_id);
}

static struct device_attribute sunxi_uart_dev_info_attr =
	__ATTR(dev_info, S_IRUGO, sunxi_uart_dev_info_show, NULL);

static ssize_t sunxi_uart_status_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct uart_port *port = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE,
	   	"uartclk = %d \n"
	   	"The Uart controller register[Base: 0x%p]: \n"
	   	"[RTX] 0x%02x = 0x%08x, [IER] 0x%02x = 0x%08x, [FCR] 0x%02x = 0x%08x \n"
	   	"[LCR] 0x%02x = 0x%08x, [MCR] 0x%02x = 0x%08x, [LSR] 0x%02x = 0x%08x \n"
	   	"[MSR] 0x%02x = 0x%08x, [SCH] 0x%02x = 0x%08x, [USR] 0x%02x = 0x%08x \n"
	   	"[TFL] 0x%02x = 0x%08x, [RFL] 0x%02x = 0x%08x, [HALT] 0x%02x = 0x%08x \n",
	   	port->uartclk, port->membase,
		SUNXI_UART_RBR, readl(port->membase + SUNXI_UART_RBR),
		SUNXI_UART_IER, readl(port->membase + SUNXI_UART_IER),
		SUNXI_UART_FCR, readl(port->membase + SUNXI_UART_FCR),
		SUNXI_UART_LCR, readl(port->membase + SUNXI_UART_LCR),
		SUNXI_UART_MCR, readl(port->membase + SUNXI_UART_MCR),
		SUNXI_UART_LSR, readl(port->membase + SUNXI_UART_LSR),
		SUNXI_UART_MSR, readl(port->membase + SUNXI_UART_MSR),
		SUNXI_UART_SCH, readl(port->membase + SUNXI_UART_SCH),
		SUNXI_UART_USR, readl(port->membase + SUNXI_UART_USR),
		SUNXI_UART_TFL, readl(port->membase + SUNXI_UART_TFL),
		SUNXI_UART_RFL, readl(port->membase + SUNXI_UART_RFL),
		SUNXI_UART_HALT, readl(port->membase + SUNXI_UART_HALT));
}
static struct device_attribute sunxi_uart_status_attr =
	__ATTR(status, S_IRUGO, sunxi_uart_status_show, NULL);

static ssize_t sunxi_uart_loopback_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int mcr = 0;
	struct uart_port *port = dev_get_drvdata(dev);

	mcr = readl(port->membase + SUNXI_UART_MCR);
	return snprintf(buf, PAGE_SIZE,
	   	"MCR: 0x%08x, Loopback: %d\n", mcr, mcr&SUNXI_UART_MCR_LOOP ? 1 : 0);
}

static ssize_t sunxi_uart_loopback_store(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf, size_t count)
{
	int mcr = 0;
	int enable = 0;
	struct uart_port *port = dev_get_drvdata(dev);

	if (!strncmp(buf, "enable", 6))
		enable = 1;

	pr_debug("Set loopback: %d \n", enable);

	mcr = readl(port->membase + SUNXI_UART_MCR);
	if (enable)
		writel(mcr|SUNXI_UART_MCR_LOOP, port->membase + SUNXI_UART_MCR);
	else
		writel(mcr&(~SUNXI_UART_MCR_LOOP), port->membase + SUNXI_UART_MCR);

	return count;
}
static struct device_attribute sunxi_uart_loopback_attr =
	__ATTR(loopback, S_IRUGO|S_IWUSR, sunxi_uart_loopback_show, sunxi_uart_loopback_store);

static ssize_t sunxi_uart_ctrl_info_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct uart_port *port = dev_get_drvdata(dev);
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);
	u32 dl = (u32)sw_uport->dlh << 8 | (u32)sw_uport->dll;

	if (dl == 0)
		dl = 1000;

	return snprintf(buf, PAGE_SIZE,
		" ier  : 0x%02x\n"
		" lcr  : 0x%02x\n"
		" mcr  : 0x%02x\n"
		" fcr  : 0x%02x\n"
		" dll  : 0x%02x\n"
		" dlh  : 0x%02x\n"
		" last baud : %d (dl = %d)\n\n"
		"TxRx Statistics:\n"
		" tx     : %d\n"
		" rx     : %d\n"
		" parity : %d\n"
		" frame  : %d\n"
		" overrun: %d\n",
		sw_uport->ier, sw_uport->lcr, sw_uport->mcr,
		sw_uport->fcr, sw_uport->dll, sw_uport->dlh,
		(sw_uport->port.uartclk>>4)/dl, dl,
		sw_uport->port.icount.tx,
		sw_uport->port.icount.rx,
		sw_uport->port.icount.parity,
		sw_uport->port.icount.frame,
		sw_uport->port.icount.overrun);
}
static struct device_attribute sunxi_uart_ctrl_info_attr =
	__ATTR(ctrl_info, S_IRUGO, sunxi_uart_ctrl_info_show, NULL);

static void sunxi_uart_sysfs(struct platform_device *_pdev)
{
	device_create_file(&_pdev->dev, &sunxi_uart_dev_info_attr);
	device_create_file(&_pdev->dev, &sunxi_uart_status_attr);
	device_create_file(&_pdev->dev, &sunxi_uart_loopback_attr);
	device_create_file(&_pdev->dev, &sunxi_uart_ctrl_info_attr);
}

#ifdef CONFIG_SERIAL_SUNXI_CONSOLE
static struct uart_port *sw_console_get_port(struct console *co)
{
	struct uart_port *port = NULL;
	int i, used;

	for (i=0; i<SUNXI_UART_NUM; i++) {
		used = sw_uport_pdata[i].used;
		port = &sw_uart_ports[i].port;
		if ((used == 1) && (port->line == co->index)) {
			break;
		}
	}
	return port;
}

static void sw_console_write(struct console *co, const char *s,
			      unsigned int count)
{
	struct uart_port *port = NULL;
	struct sw_uart_port *sw_uport;
	unsigned long flags;
	unsigned int ier;
	int locked = 1;

	BUG_ON(co->index < 0 || co->index >= SUNXI_UART_NUM);

	port = sw_console_get_port(co);
	if (port == NULL)
		return;
	sw_uport = UART_TO_SPORT(port);

	local_irq_save(flags);
	if (port->sysrq)
		locked = 0;
	else if (oops_in_progress)
		locked = spin_trylock(&port->lock);
	else
		spin_lock(&port->lock);
	ier = serial_in(port, SUNXI_UART_IER);
	serial_out(port, 0, SUNXI_UART_IER);

	uart_console_write(port, s, count, sw_console_putchar);
	wait_for_xmitr(sw_uport);
	serial_out(port, ier, SUNXI_UART_IER);
	if (locked)
		spin_unlock(&port->lock);
	local_irq_restore(flags);
}

static int __init sw_console_setup(struct console *co, char *options)
{
	struct uart_port *port = NULL;
	struct sw_uart_port *sw_uport;
	int baud = 115200;
	int bits = 8;
	int parity = 'n';
	int flow = 'n';

	if (unlikely(co->index >= SUNXI_UART_NUM || co->index < 0))
		return -ENXIO;

	port = sw_console_get_port(co);
	if (port == NULL)
		return -ENODEV;
	sw_uport = UART_TO_SPORT(port);
	if (!port->iobase && !port->membase)
		return -ENODEV;

	if (options)
		uart_parse_options(options, &baud, &parity, &bits, &flow);

	SERIAL_MSG("console setup baud %d parity %c bits %d, flow %c\n",
			baud, parity, bits, flow);
	return uart_set_options(port, co, baud, parity, bits, flow);
}

static struct uart_driver sw_uart_driver;
static struct console sw_console = {
	.name = "ttyS",
	.write = sw_console_write,
	//.device = uart_console_device,
	.setup = sw_console_setup,
	.flags = CON_PRINTBUFFER,
	.index = -1,
	.data = &sw_uart_driver,
};

#define SW_CONSOLE	(&sw_console)
#else
#define SW_CONSOLE	NULL
#endif

static struct uart_driver sw_uart_driver = {
	.owner = THIS_MODULE,
	.driver_name = SUNXI_UART_DEV_NAME,
	.dev_name = "ttyS",
	.nr = SUNXI_UART_NUM,
	.cons = SW_CONSOLE,
};

static int sw_uart_request_resource(struct sw_uart_port* sw_uport, struct sw_uart_pdata *pdata)
{
	SERIAL_DBG("get system resource(clk & IO)\n");

	if (sw_uart_regulator_request(sw_uport, pdata) < 0) {
		SERIAL_MSG("uart%d request regulator failed!\n", sw_uport->id);
		return -ENXIO;
	}
	sw_uart_regulator_enable(pdata);

	#ifdef CONFIG_SW_UART_DUMP_DATA
	sw_uport->dump_buff = (char*)kmalloc(MAX_DUMP_SIZE, GFP_KERNEL);
	if (!sw_uport->dump_buff) {
		SERIAL_MSG("uart%d fail to alloc dump buffer\n", sw_uport->id);
	}
	#endif

	return 0;
}

static int sw_uart_release_resource(struct sw_uart_port* sw_uport, struct sw_uart_pdata *pdata)
{
	SERIAL_DBG("put system resource(clk & IO)\n");

	#ifdef CONFIG_SW_UART_DUMP_DATA
	kfree(sw_uport->dump_buff);
	sw_uport->dump_buff = NULL;
	sw_uport->dump_len = 0;
	#endif

	clk_disable_unprepare(sw_uport->mclk);
	clk_put(sw_uport->mclk);

	sw_uart_regulator_disable(pdata);
	sw_uart_regulator_release(pdata);

	return 0;
}

struct platform_device *sw_key_uart_get_pdev(int uart_id)
{
	if (sw_uart_ports[uart_id].port.dev)
		return to_platform_device(sw_uart_ports[uart_id].port.dev);
	else
		return NULL;
}
EXPORT_SYMBOL(sw_key_uart_get_pdev);

static int sw_uart_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct uart_port *port;
	struct sw_uart_port *sw_uport;
	struct sw_uart_pdata *pdata;
	struct resource *res;
	struct input_dev *input; // 输入子系统设备
	char uart_para[16] = {0};
	int ret = -1;
	int i = 0;
    printk("sunxi_uart_probe\n");
	pdev->id = of_alias_get_id(np, "serial");
	if (pdev->id < 0) {
		SERIAL_MSG("failed to get alias id\n");
		return -EINVAL;
	}

	port = &sw_uart_ports[pdev->id].port;
	port->dev = &pdev->dev;
	pdata = &sw_uport_pdata[pdev->id];
	sw_uport = UART_TO_SPORT(port);
	sw_uport->pdata = pdata;
	sw_uport->id = pdev->id;
	sw_uport->ier = 0;
	sw_uport->lcr = 0;
	sw_uport->mcr = 0;
	sw_uport->fcr = 0;
	sw_uport->dll = 0;
	sw_uport->dlh = 0;
	snprintf(sw_uport->name, 16, SUNXI_UART_DEV_NAME"%d", pdev->id);
	pdev->dev.init_name = sw_uport->name;
	pdev->dev.platform_data = sw_uport->pdata;

	/* request system resource and init them */
	ret = sw_uart_request_resource(sw_uport, pdev->dev.platform_data);
	if (unlikely(ret)) {
		SERIAL_MSG("uart%d error to get resource\n", pdev->id);
		return -ENXIO;
	}

#ifdef CONFIG_EVB_PLATFORM
	sw_uport->mclk = of_clk_get(np, 0);
	if (IS_ERR(sw_uport->mclk)) {
		SERIAL_MSG("uart%d error to get clk\n", pdev->id);
		return -EINVAL;
	}
	/* uart clk come from apb2, apb2 default clk is hosc. if change rate
	 * needed, must switch apb2's source clk first and then set its rate
	 * */
	sw_uport->sclk = of_clk_get(np, 1);
	if (!IS_ERR(sw_uport->sclk)) {
		sw_uport->pclk = of_clk_get(np, 2);
		port->uartclk = clk_get_rate(sw_uport->sclk);
		/*config a fixed divider before switch source clk for apb2 */
		clk_set_rate(sw_uport->sclk, port->uartclk/6);
		/* switch source clock for apb2 */
		clk_set_parent(sw_uport->sclk, sw_uport->pclk);
		ret = of_property_read_u32(np, "clock-frequency",
					&port->uartclk);
		if (ret) {
			SERIAL_MSG("uart%d get clock-freq failed\n", pdev->id);
			return -EINVAL;
		}
		/* set apb2 clock frequency now */
		clk_set_rate(sw_uport->sclk, port->uartclk);
	}

	port->uartclk = clk_get_rate(sw_uport->mclk);
#else
	port->uartclk = 24000000;
#endif

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		SERIAL_MSG("uart%d error to get MEM resource\n", pdev->id);
		return -EINVAL;
	}
	port->mapbase = res->start;

	port->irq = platform_get_irq(pdev, 0);
	if (port->irq < 0) {
		SERIAL_MSG("uart%d error to get irq\n", pdev->id);
		return -EINVAL;
	}

	snprintf(uart_para, sizeof(uart_para), "uart%d_port", pdev->id);
	ret = of_property_read_u32(np, uart_para, &port->line);
	if (ret) {
		SERIAL_MSG("uart%d error to get port property\n", pdev->id);
		return -EINVAL;
	}

	snprintf(uart_para, sizeof(uart_para), "uart%d_type", pdev->id);
	ret = of_property_read_u32(np, uart_para, &pdata->io_num);
	if (ret) {
		SERIAL_MSG("uart%d error to get type property\n", pdev->id);
		return -EINVAL;
	}

	if (of_property_read_bool(np, "linux,rs485-enabled-at-boot-time"))
		sw_uport->rs485conf.flags |= SER_RS485_ENABLED;

	pdata->used = 1;
	port->iotype = UPIO_MEM;
	port->type = PORT_SUNXI;
	port->flags = UPF_BOOT_AUTOCONF;
	port->ops = &sw_uart_ops;
	port->fifosize = SUNXI_UART_FIFO_SIZE;
	platform_set_drvdata(pdev, port);

    // 申请串口接收空间
	sw_uport->rx_buf = devm_kzalloc(&pdev->dev, sizeof(struct rx_key_buf), GFP_KERNEL);
	if(!sw_uport->rx_buf)
	{
		key_debug("rx_buf alloc failed\n");
		goto err_port_line;
	}
    
	//初始化接收队列的头尾指针
	sw_uport->rx_buf->head = sw_uport->rx_buf->tail = 0;

	INIT_WORK(&sw_uport->qos_work, rigol_kbd_qos_work);
	mutex_init(&sw_uport->lock);

	//当前状态不是获取版本号
    sw_uport->get_keyBoard_ver = 0;

    input = devm_input_allocate_device(&pdev->dev);
    if (!input)
    {
        dev_err(&pdev->dev, "failed to allocate input device\n");
        return -ENOMEM;
    }

    //重新设置input的名称，原来会产生inputx(x=0,1,...)
    //dev_set_name(&input->dev, "rigol-kbd");

	//save input device into uart.
    sw_uport->input = input;
    input->name = "rigol keyboard";
    input->phys = "rigol-kbd";
    input_set_drvdata(input, sw_uport);
    input->id.bustype = BUS_HOST;//BUS_RS232;
    input->id.vendor = 0x001E;
    input->id.product = 0x0002;
    input->id.version = 0x0101;
    input->dev.parent = &pdev->dev;

	/* 事件函数，当用户层write下来input_event时触发 */
    input->event = rigol_kbd_event;

	/* 按键列表信息 */
    input->keycode = rigol_keycodes;
    input->keycodesize = sizeof(rigol_keycodes[0]);
    input->keycodemax = KEY_TOTAL_COUNT;

/* 设置时间类型为按键，事件类型列表如下(inxlude/uapi/linux/input.h)
	 * EV_KEY - 按键，按下为1，松开为0
	 * EV_REP - Reset
	 * EV_RST - 鼠标值，相对坐标
	 * EV_ABS - 写字板产生的值，绝对坐标
	 * EV_MSC - 其他
	 * EV_LED - LED设备
	 * EV_SND - 蜂鸣器等，输入声音
	 * EV_FF  - 力反馈
	 */
    __set_bit(EV_KEY, input->evbit);  //set event as 'key' event.
    __clear_bit(EV_REP, input->evbit);//non-repeat

	/* 设置能产生哪些事件（比如按键的L键等），这里的按键都对应键盘上的按键值 */
    for (i = 0; i < KEY_TOTAL_COUNT; i++)
    {
        __set_bit(rigol_keycodes[i], input->keybit);
    }
    
    //按键上报形式上报版本号
    __set_bit(0x0F, input->keybit);

	/* LED_RIGOL定义于include/uapi/linux/input.h中（自己添加），值为0x0B，需要跟应用层保持一致
	 * Archerfish采用LED_RIGOL，此处为了避免修改input.h文件，采用LED_MISC（其他类型LED）
	 * 相当于__set_bit(LED_RIGOL, input->ledbit);
	 */
    input_set_capability(input, EV_LED, LED_MISC/*LED_RIGOL*/);

	/* 配置串口，波特率等信息 */
    keyboard_uart_init(up);

    ret = input_register_device(input);
    if (ret)
    {
        dev_err(&pdev->dev, "unable to register input device\n");
        goto err_port_line;
    }
	sunxi_uart_sysfs(pdev);

	SERIAL_DBG("add uart%d port, port_type %d, uartclk %d\n",
			pdev->id, port->type, port->uartclk);
	return uart_add_one_port(&sw_uart_driver, port);
err_port_line:
    dev_err(&pdev->dev, "[UART%d]: failure [%s]: %d\n",pdev->id, __func__, ret);
	return ret;
}

static int sw_uart_remove(struct platform_device *pdev)
{
	struct sw_uart_port *sw_uport = platform_get_drvdata(pdev);

	SERIAL_DBG("release uart%d port\n", sw_uport->id);
	sw_uart_release_resource(sw_uport, pdev->dev.platform_data);
	return 0;

}

/* UART power management code */
#ifdef CONFIG_PM_SLEEP

#define SW_UART_NEED_SUSPEND(port) \
	((sw_is_console_port(port) && (console_suspend_enabled)) \
		|| !sw_is_console_port(port))

static int sw_uart_suspend(struct device *dev)
{
	struct uart_port *port = dev_get_drvdata(dev);
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);

	if (port) {
		SERIAL_MSG("uart%d suspend\n", sw_uport->id);
		uart_suspend_port(&sw_uart_driver, port);

		if (SW_UART_NEED_SUSPEND(port)) {
			sw_uart_select_gpio_state(sw_uport->pctrl, PINCTRL_STATE_SLEEP, sw_uport->id);
			sw_uart_regulator_disable(dev->platform_data);
		}
	}

	return 0;
}

static int sw_uart_resume(struct device *dev)
{
#ifdef CONFIG_EVB_PLATFORM
	unsigned long flags = 0;
#endif
	struct uart_port *port = dev_get_drvdata(dev);
	struct sw_uart_port *sw_uport = UART_TO_SPORT(port);

	if (port) {
		if (SW_UART_NEED_SUSPEND(port)) {
			sw_uart_regulator_enable(dev->platform_data);
			sw_uart_select_gpio_state(sw_uport->pctrl, PINCTRL_STATE_DEFAULT, sw_uport->id);
		}
#ifdef CONFIG_EVB_PLATFORM
		/* It's used only in super-standby mode.
		  FPGA maybe fall into sw_uart_force_lcr(), so comment it. */
		if (sw_is_console_port(port) && !console_suspend_enabled) {
			spin_lock_irqsave(&port->lock, flags);
			sw_uart_reset(sw_uport);
			serial_out(port, sw_uport->fcr, SUNXI_UART_FCR);
			serial_out(port, sw_uport->mcr, SUNXI_UART_MCR);
			serial_out(port, sw_uport->lcr|SUNXI_UART_LCR_DLAB, SUNXI_UART_LCR);
			serial_out(port, sw_uport->dll, SUNXI_UART_DLL);
			serial_out(port, sw_uport->dlh, SUNXI_UART_DLH);
			serial_out(port, sw_uport->lcr, SUNXI_UART_LCR);
			serial_out(port, sw_uport->ier, SUNXI_UART_IER);
			spin_unlock_irqrestore(&port->lock, flags);
		}
#endif
		uart_resume_port(&sw_uart_driver, port);
		SERIAL_MSG("uart%d resume. DLH: %d, DLL: %d. \n", sw_uport->id, sw_uport->dlh, sw_uport->dll);
	}

	return 0;
}

static const struct dev_pm_ops sw_uart_pm_ops = {
	.suspend = sw_uart_suspend,
	.resume = sw_uart_resume,
};
#define SERIAL_SW_PM_OPS	(&sw_uart_pm_ops)

#else /* !CONFIG_PM_SLEEP */

#define SERIAL_SW_PM_OPS	NULL
#endif /* CONFIG_PM_SLEEP */

static const struct of_device_id sunxi_uart_match[] = {
	{ .compatible = "allwinner,sun8i-uart", },
	{ .compatible = "allwinner,sun50i-uart", },
	{ .compatible = "allwinner,sun3i-uart", },
	{},
};

MODULE_DEVICE_TABLE(of, sunxi_uart_match);
static struct platform_device_id sw_serial_driver_ids[] = 
{
	{
		.name		= "rigol keyboard",
		.driver_data	= (kernel_ulong_t)NULL,
		
    },
};	
static struct platform_driver sw_uport_platform_driver = {
	.probe  = sw_uart_probe,
	.remove = sw_uart_remove,
	.id_table = sw_serial_driver_ids,
	.driver = {
		.name  = SUNXI_UART_DEV_NAME,
		.pm    = SERIAL_SW_PM_OPS,
		.owner = THIS_MODULE,
		// .of_match_table = sunxi_uart_match,
	},
};

static int __init sunxi_uart_init(void)
{
	int ret;
        printk("sunxi_uart_init\n");
	ret = uart_register_driver(&sw_uart_driver);
	if (unlikely(ret)) {
		SERIAL_MSG("driver initializied\n");
		return ret;
	}

	return platform_driver_register(&sw_uport_platform_driver);
}

static void __exit sunxi_uart_exit(void)
{
	SERIAL_MSG("driver exit\n");
#ifdef CONFIG_SERIAL_SUNXI_CONSOLE
	unregister_console(&sw_console);
#endif
	platform_driver_unregister(&sw_uport_platform_driver);
	uart_unregister_driver(&sw_uart_driver);
}

module_init(sunxi_uart_init);
module_exit(sunxi_uart_exit);

MODULE_AUTHOR("Aaron<leafy.myeh@allwinnertech.com>");
MODULE_DESCRIPTION("Driver for Allwinner UART controller");
MODULE_LICENSE("GPL");

