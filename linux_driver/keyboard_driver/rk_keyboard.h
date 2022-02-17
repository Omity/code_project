/********************************************************************
                普源精电科技股份有限公司 版权所有(2020)
*********************************************************************
头文件名: keyboard.h
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

#ifndef MISC_KEYBOARD_KEYBOARD_H_
#define MISC_KEYBOARD_KEYBOARD_H_
/********************************************************************
* Includes
********************************************************************/

/********************************************************************
* Definitions
********************************************************************/
#define KEY_DEBUG        0
#define PRINT_KEY_VALUE  0

/* print keyboard value */
#if KEY_DEBUG
#    define key_debug(fmt,...)                       \
        do                                           \
        {                                            \
            printk(fmt,##__VA_ARGS__);               \
        }while(0)
#else
#    define key_debug(fmt,...)
#endif
/********************************************************************
* Variables
********************************************************************/
/*----------------------------------  宏定义  ------------------------------------*/
#define KEY_COUNT               37     //有效按键个数，即面板上按键个数，包括旋钮支持的按键，这里采用DG1000Z的键盘阵列
#define KEY_KNOB_COUNT          2      //旋钮相当于2个按键
#define KEY_TOTAL_COUNT         (KEY_COUNT + KEY_KNOB_COUNT)
/* 帧头 */
#define KEY_FRAME_HD            0xAA   //键值帧头
#define KEY_HD_LEN              1      //键值帧头长度，1字节

/* 帧中的键值数据长度，最大阵列（包括空余位置，sweep*detect）占字节数 */
#define KEY_VALID_LEN           6      //一帧键值数据中有效键值的长度, 6bytes

/* 版本信息 */
#define KEY_VER_LEN             1      //键盘版本号长度，1字节
#define KEY_VER_MASK            0xFF   //版本号掩码

/* CRC多项式为0b10011。校验值得出的方法为将从帧头开始，到版本号结束所有
 * 字节看做一个“长字”(帧头为最高字节)，左移4位后对10011取余，最终的CRC字节
 * 由4位0(高四位)和4位余数(低四位)组成。
 */
#define CRC4_POLYNOMIAL         0x13         //CRC4生成多项式系数
#define KEY_CRC4_LEN            1            //校验码长度，1字节
#define KEY_CRC4_POS            KEY_HD_LEN+KEY_VALID_LEN+KEY_VER_LEN  //校验码位置

//键值一帧数据长度，9bytes：1byte帧头 + 6bytes键值 + 1byte版本号 + 1byte CRC校验码
#define KEY_FRAME_LEN           (KEY_HD_LEN+KEY_VALID_LEN+KEY_VER_LEN+KEY_CRC4_LEN)//9


/* 根据原理图和PCB定义各行列键值对应的键值掩码，(Col, Row)
 * Row表示sweep，即第一个sweep就是第一行，8个sweep即8行
 */
#define BIULD_KEY_MASK_BIT(bit)  (((u64)0x0000000000000001)<<bit)
#define KEY_MASK_BIT0            BIULD_KEY_MASK_BIT(0)    /* (1,1),bit0,K11  */
#define KEY_MASK_BIT1            BIULD_KEY_MASK_BIT(1)    /* (1,2),bit1,K12  */
#define KEY_MASK_BIT2            BIULD_KEY_MASK_BIT(2)    /* (1,3),bit2,K13 */
#define KEY_MASK_BIT3            BIULD_KEY_MASK_BIT(3)    /* (1,4),bit3, 没有这个 */
#define KEY_MASK_BIT4            BIULD_KEY_MASK_BIT(4)    /* (1,5),bit4, knob1，旋钮 */
#define KEY_MASK_BIT5            BIULD_KEY_MASK_BIT(5)    /* (1,6),bit5, 没有这个 */
#define KEY_MASK_BIT6            BIULD_KEY_MASK_BIT(6)    /* (2,1),bit6,K21  */
#define KEY_MASK_BIT7            BIULD_KEY_MASK_BIT(7)    /* (2,2),bit7,K22 */
#define KEY_MASK_BIT8            BIULD_KEY_MASK_BIT(8)    /* (2,3),bit8,K23 */
#define KEY_MASK_BIT9            BIULD_KEY_MASK_BIT(9)    /* (2,4),bit9,OK，旋钮的确认键 */
#define KEY_MASK_BIT10           BIULD_KEY_MASK_BIT(10)   /* (2,5),bit10, knob2，旋钮 */
#define KEY_MASK_BIT11           BIULD_KEY_MASK_BIT(11)   /* (2,6),bit11, 没有这个 */
#define KEY_MASK_BIT12           BIULD_KEY_MASK_BIT(12)   /* (3,1),bit12,K31  */
#define KEY_MASK_BIT13           BIULD_KEY_MASK_BIT(13)   /* (3,2),bit13,K32 */
#define KEY_MASK_BIT14           BIULD_KEY_MASK_BIT(14)   /* (3,3),bit14,K33 */
#define KEY_MASK_BIT15           BIULD_KEY_MASK_BIT(15)   /* (3,4),bit15,K34 */
#define KEY_MASK_BIT16           BIULD_KEY_MASK_BIT(16)   /* (3,5),bit16, 没有这个*/
#define KEY_MASK_BIT17           BIULD_KEY_MASK_BIT(17)   /* (3,6),bit17, 没有这个 */
#define KEY_MASK_BIT18           BIULD_KEY_MASK_BIT(18)   /* (4,1),bit18,K41  */
#define KEY_MASK_BIT19           BIULD_KEY_MASK_BIT(19)   /* (4,2),bit19,K42 */
#define KEY_MASK_BIT20           BIULD_KEY_MASK_BIT(20)   /* (4,3),bit20,K43 */
#define KEY_MASK_BIT21           BIULD_KEY_MASK_BIT(21)   /* (4,4),bit21,K44 */
#define KEY_MASK_BIT22           BIULD_KEY_MASK_BIT(22)   /* (4,5),bit22, 没有这个 */
#define KEY_MASK_BIT23           BIULD_KEY_MASK_BIT(23)   /* (4,6),bit23, 没有这个 */
#define KEY_MASK_BIT24           BIULD_KEY_MASK_BIT(24)   /* (5,1),bit24, 没有这个 */
#define KEY_MASK_BIT25           BIULD_KEY_MASK_BIT(25)   /* (5,2),bit25, 没有这个 */
#define KEY_MASK_BIT26           BIULD_KEY_MASK_BIT(26)   /* (5,3),bit26, 没有这个 */
#define KEY_MASK_BIT27           BIULD_KEY_MASK_BIT(27)   /* (5,4),bit27, 没有这个 */
#define KEY_MASK_BIT28           BIULD_KEY_MASK_BIT(28)   /* (5,5),bit28, 没有这个 */
#define KEY_MASK_BIT29           BIULD_KEY_MASK_BIT(29)   /* (5,6),bit29, 没有这个 */
#define KEY_MASK_BIT30           BIULD_KEY_MASK_BIT(30)   /* (6,1),bit30, 没有这个  */
#define KEY_MASK_BIT31           BIULD_KEY_MASK_BIT(31)   /* (6,2),bit31, 没有这个 */
#define KEY_MASK_BIT32           BIULD_KEY_MASK_BIT(32)   /* (6,3),bit32, 没有这个 */
#define KEY_MASK_BIT33           BIULD_KEY_MASK_BIT(33)   /* (6,4),bit33, 没有这个 */
#define KEY_MASK_BIT34           BIULD_KEY_MASK_BIT(34)   /* (6,5),bit34, 没有这个 */
#define KEY_MASK_BIT35           BIULD_KEY_MASK_BIT(35)   /* (6,6),bit35, 没有这个 */
#define KEY_MASK_BIT36           BIULD_KEY_MASK_BIT(36)   /* (7,1),bit36, 没有这个  */
#define KEY_MASK_BIT37           BIULD_KEY_MASK_BIT(37)   /* (7,2),bit37, 没有这个 */
#define KEY_MASK_BIT38           BIULD_KEY_MASK_BIT(38)   /* (7,3),bit38, 没有这个 */
#define KEY_MASK_BIT39           BIULD_KEY_MASK_BIT(39)   /* (7,4),bit39, 没有这个 */
#define KEY_MASK_BIT40           BIULD_KEY_MASK_BIT(40)   /* (7,5),bit40, 没有这个 */
#define KEY_MASK_BIT41           BIULD_KEY_MASK_BIT(41)   /* (7,6),bit41, 没有这个 */
#define KEY_MASK_BIT42           BIULD_KEY_MASK_BIT(42)   /* (8,1),bit42, 没有这个  */
#define KEY_MASK_BIT43           BIULD_KEY_MASK_BIT(43)   /* (8,2),bit43, 没有这个 */
#define KEY_MASK_BIT44           BIULD_KEY_MASK_BIT(44)   /* (8,3),bit44, 没有这个 */
#define KEY_MASK_BIT45           BIULD_KEY_MASK_BIT(45)   /* (8,4),bit45, 没有这个 */
#define KEY_MASK_BIT46           BIULD_KEY_MASK_BIT(46)   /* (8,5),bit46, 没有这个 */
#define KEY_MASK_BIT47           BIULD_KEY_MASK_BIT(47)   /* (8,6),bit47, 没有这个 */

/* 重新指出knob的位置 */
//获得旋钮两位编码，knob2为【高位】，knob1为【低位】
#define KEY_MASK_KNOB_1_POS      10
#define KEY_MASK_KNOB_2_POS      4
#define KEY_MASK_KNOB_1          BIULD_KEY_MASK_BIT(KEY_MASK_KNOB_1_POS)
#define KEY_MASK_KNOB_2          BIULD_KEY_MASK_BIT(KEY_MASK_KNOB_2_POS)
#define KEY_KNOB_GET_VALUE(v)    ((((v) & KEY_MASK_KNOB_1) >> KEY_MASK_KNOB_1_POS) | \
                                 (((v) & KEY_MASK_KNOB_2) >> (KEY_MASK_KNOB_2_POS-1))) & 0x3
/* 旋钮编码器变动几次判断方向？最小为2
 * 逆时针（左）：11->01->00->10->11
 * 顺时针（右）：11->10->00->01->11
 * 如果方向反了，那么调整knob的高地位
 */
#define KEY_KNOB_IDENTIFY_DIR_STEP 2

/* 由于在键盘阵列中存在没有使用的节点，则每帧数据中除了后16bit（ver+crc）和帧头8bit全部
   不使用外，中间48bit中也存在没有使用的位，逻辑不保证这些未使用位的状态，因此需要在解析前
   将没有用到的Bit位置为1，避免误判。根据原理图，得到无效位掩码 */
#define KEY_UNUSED_KEY          (KEY_MASK_BIT3  | KEY_MASK_BIT5  | KEY_MASK_BIT11 | KEY_MASK_BIT16 | KEY_MASK_BIT17 |\
                                 KEY_MASK_BIT22 | KEY_MASK_BIT23 | KEY_MASK_BIT24 | KEY_MASK_BIT25 | KEY_MASK_BIT26 |\
                                 KEY_MASK_BIT27 | KEY_MASK_BIT28 | KEY_MASK_BIT29 | KEY_MASK_BIT30 | KEY_MASK_BIT31 |\
                                 KEY_MASK_BIT32 | KEY_MASK_BIT33 | KEY_MASK_BIT34 | KEY_MASK_BIT35 | KEY_MASK_BIT36 |\
                                 KEY_MASK_BIT37 | KEY_MASK_BIT38 | KEY_MASK_BIT39 | KEY_MASK_BIT40 | KEY_MASK_BIT41 |\
                                 KEY_MASK_BIT42 | KEY_MASK_BIT43 | KEY_MASK_BIT44 | KEY_MASK_BIT45 | KEY_MASK_BIT46 |\
                                 KEY_MASK_BIT47)
#define KEY_UNUSED_MASK         (KEY_UNUSED_KEY|0xFFFF000000000000)

/* 按键状态*/
#define KEY_STATE_REL    0
#define KEY_STATE_DN     1
#define KEY_STATE_HOLD   2

/* linux系统下,按键值不能超过KEY_MAX(0x2ff),所以,保能将按键映射在现有的键值上.
 * (最好映射在标准键盘上,这样标准键盘也可控制仪器)
 */
#define RIGOL_KBD_KEY_INVALID   KEY_UNKNOWN
/************************************/
#define RIGOL_KBD_KEY_HOME         BTN_TRIGGER_HAPPY1  //home
#define RIGOL_KBD_KEY_MENU         BTN_TRIGGER_HAPPY2  //menu
#define RIGOL_KBD_KEY_PRESET       BTN_TRIGGER_HAPPY3  //preset
#define RIGOL_KBD_KEY_LOCK         BTN_TRIGGER_HAPPY4  //lock
#define RIGOL_KBD_KEY_TRIG         BTN_TRIGGER_HAPPY5  //trigger
#define RIGOL_KBD_KEY_UTIL         BTN_TRIGGER_HAPPY6  //utility
#define RIGOL_KBD_KEY_STORE        BTN_TRIGGER_HAPPY7  //Store
#define RIGOL_KBD_KEY_HELP_LOCAL   BTN_TRIGGER_HAPPY8  //help & local
#define RIGOL_KBD_KEY_OUTPUT1      BTN_TRIGGER_HAPPY9  //output1
#define RIGOL_KBD_KEY_OUTPUT2      BTN_TRIGGER_HAPPY10 //output2
#define RIGOL_KBD_KEY_COUNTER      BTN_TRIGGER_HAPPY11 //counter
#define RIGOL_KBD_KEY_RETURN       BTN_TRIGGER_HAPPY12 //return
/***************** 旋钮(3) ********************/
#define RIGOL_KBD_KEY_OK           BTN_TRIGGER_HAPPY13 //konb-ok
#define RIGOL_KBD_KEY_KNOB_LEFT    BTN_TRIGGER_HAPPY14 //knob-left
#define RIGOL_KBD_KEY_KNOB_RIGHT   BTN_TRIGGER_HAPPY15 //knob-right
/***************** 方向键（2个）：左右 ****************/
//#define RIGOL_KBD_KEY_LEFT         KEY_LEFT  //dir-left
//#define RIGOL_KBD_KEY_RIGHT        KEY_RIGHT //dir-right
//冯艳红要求更改：combox左右键冲突
#define RIGOL_KBD_KEY_LEFT         BTN_TRIGGER_HAPPY16  //dir-left
#define RIGOL_KBD_KEY_RIGHT        BTN_TRIGGER_HAPPY17 //dir-right


static u16 rigol_keycodes[KEY_TOTAL_COUNT] = {
    RIGOL_KBD_KEY_HOME,    RIGOL_KBD_KEY_MENU,       RIGOL_KBD_KEY_PRESET,     RIGOL_KBD_KEY_LOCK,
    RIGOL_KBD_KEY_TRIG,    RIGOL_KBD_KEY_UTIL,       RIGOL_KBD_KEY_STORE,      RIGOL_KBD_KEY_HELP_LOCAL,
    RIGOL_KBD_KEY_OUTPUT1, RIGOL_KBD_KEY_OUTPUT2,    RIGOL_KBD_KEY_COUNTER,    RIGOL_KBD_KEY_RETURN,
    RIGOL_KBD_KEY_OK,      RIGOL_KBD_KEY_LEFT,       RIGOL_KBD_KEY_RIGHT,      RIGOL_KBD_KEY_KNOB_LEFT,
    RIGOL_KBD_KEY_KNOB_RIGHT,
};

//这个数组与rigol_keycodes是一一对应的！！！
//该数组表示第几位表示rigol_keycodes相同位置按键
static u64 rigol_keycode_bits[KEY_COUNT] = {
    KEY_MASK_BIT0,   KEY_MASK_BIT1,   KEY_MASK_BIT6,   KEY_MASK_BIT7,
    KEY_MASK_BIT8,   KEY_MASK_BIT12,  KEY_MASK_BIT13,  KEY_MASK_BIT14,
    KEY_MASK_BIT19,  KEY_MASK_BIT20,  KEY_MASK_BIT21,  KEY_MASK_BIT18,
    KEY_MASK_BIT9,   KEY_MASK_BIT15,  KEY_MASK_BIT2
};
/********************************************************************
* Declarations
********************************************************************/
#include <linux/regulator/consumer.h>


/* register offset define */
#define SUNXI_UART_RBR (0x00) 		/* receive buffer register */
#define SUNXI_UART_THR (0x00) 		/* transmit holding register */
#define SUNXI_UART_DLL (0x00) 		/* divisor latch low register */
#define SUNXI_UART_DLH (0x04) 		/* diviso latch high register */
#define SUNXI_UART_IER (0x04) 		/* interrupt enable register */
#define SUNXI_UART_IIR (0x08) 		/* interrupt identity register */
#define SUNXI_UART_FCR (0x08) 		/* FIFO control register */
#define SUNXI_UART_LCR (0x0c) 		/* line control register */
#define SUNXI_UART_MCR (0x10) 		/* modem control register */
#define SUNXI_UART_LSR (0x14) 		/* line status register */
#define SUNXI_UART_MSR (0x18) 		/* modem status register */
#define SUNXI_UART_SCH (0x1c) 		/* scratch register */
#define SUNXI_UART_USR (0x7c) 		/* status register */
#define SUNXI_UART_TFL (0x80) 		/* transmit FIFO level */
#define SUNXI_UART_RFL (0x84) 		/* RFL */
#define SUNXI_UART_HALT (0xa4) 		/* halt tx register */
#define SUNXI_UART_RS485 (0xc0)		/* RS485 control and status register */

/* register bit field define */
/* Interrupt Enable Register */
#define SUNXI_UART_IER_PTIME (BIT(7))
#define SUNXI_UART_IER_RS485 (BIT(4))
#define SUNXI_UART_IER_MSI   (BIT(3))
#define SUNXI_UART_IER_RLSI  (BIT(2))
#define SUNXI_UART_IER_THRI  (BIT(1))
#define SUNXI_UART_IER_RDI   (BIT(0))
/* Interrupt ID Register */
#define SUNXI_UART_IIR_FEFLAG_MASK (BIT(6)|BIT(7))
#define SUNXI_UART_IIR_IID_MASK    (BIT(0)|BIT(1)|BIT(2)|BIT(3))
 #define SUNXI_UART_IIR_IID_MSTA    (0)
 #define SUNXI_UART_IIR_IID_NOIRQ   (1)
 #define SUNXI_UART_IIR_IID_THREMP  (2)
 #define SUNXI_UART_IIR_IID_RXDVAL  (4)
 #define SUNXI_UART_IIR_IID_LINESTA (6)
 #define SUNXI_UART_IIR_IID_BUSBSY  (7)
 #define SUNXI_UART_IIR_IID_CHARTO  (12)
/* FIFO Control Register */
#define SUNXI_UART_FCR_RXTRG_MASK  (BIT(6)|BIT(7))
 #define SUNXI_UART_FCR_RXTRG_1CH   (0 << 6)
 #define SUNXI_UART_FCR_RXTRG_1_4   (1 << 6)
 #define SUNXI_UART_FCR_RXTRG_1_2   (2 << 6)
 #define SUNXI_UART_FCR_RXTRG_FULL  (3 << 6)
#define SUNXI_UART_FCR_TXTRG_MASK  (BIT(4)|BIT(5))
 #define SUNXI_UART_FCR_TXTRG_EMP   (0 << 4)
 #define SUNXI_UART_FCR_TXTRG_2CH   (1 << 4)
 #define SUNXI_UART_FCR_TXTRG_1_4   (2 << 4)
 #define SUNXI_UART_FCR_TXTRG_1_2   (3 << 4)
#define SUNXI_UART_FCR_TXFIFO_RST  (BIT(2))
#define SUNXI_UART_FCR_RXFIFO_RST  (BIT(1))
#define SUNXI_UART_FCR_FIFO_EN     (BIT(0))
/* Line Control Register */
#define SUNXI_UART_LCR_DLAB        (BIT(7))
#define SUNXI_UART_LCR_SBC         (BIT(6))
#define SUNXI_UART_LCR_PARITY_MASK (BIT(5)|BIT(4))
 #define SUNXI_UART_LCR_EPAR        (1 << 4)
 #define SUNXI_UART_LCR_OPAR        (0 << 4)
#define SUNXI_UART_LCR_PARITY      (BIT(3))
#define SUNXI_UART_LCR_STOP        (BIT(2))
#define SUNXI_UART_LCR_DLEN_MASK   (BIT(1)|BIT(0))
 #define SUNXI_UART_LCR_WLEN5       (0)
 #define SUNXI_UART_LCR_WLEN6       (1)
 #define SUNXI_UART_LCR_WLEN7       (2)
 #define SUNXI_UART_LCR_WLEN8       (3)
/* Modem Control Register */
#define SUNXI_UART_MCR_MODE_MASK  (BIT(7)|BIT(6))
 #define SUNXI_UART_MCR_MODE_RS485 (2 << 6)
 #define SUNXI_UART_MCR_MODE_SIRE  (1 << 6)
 #define SUNXI_UART_MCR_MODE_UART  (0 << 6)
#define SUNXI_UART_MCR_AFE        (BIT(5))
#define SUNXI_UART_MCR_LOOP       (BIT(4))
#define SUNXI_UART_MCR_RTS        (BIT(1))
#define SUNXI_UART_MCR_DTR        (BIT(0))
/* Line Status Rigster */
#define SUNXI_UART_LSR_RXFIFOE    (BIT(7))
#define SUNXI_UART_LSR_TEMT       (BIT(6))
#define SUNXI_UART_LSR_THRE       (BIT(5))
#define SUNXI_UART_LSR_BI         (BIT(4))
#define SUNXI_UART_LSR_FE         (BIT(3))
#define SUNXI_UART_LSR_PE         (BIT(2))
#define SUNXI_UART_LSR_OE         (BIT(1))
#define SUNXI_UART_LSR_DR         (BIT(0))
#define SUNXI_UART_LSR_BRK_ERROR_BITS 0x1E /* BI, FE, PE, OE bits */
/* Modem Status Register */
#define SUNXI_UART_MSR_DCD        (BIT(7))
#define SUNXI_UART_MSR_RI         (BIT(6))
#define SUNXI_UART_MSR_DSR        (BIT(5))
#define SUNXI_UART_MSR_CTS        (BIT(4))
#define SUNXI_UART_MSR_DDCD       (BIT(3))
#define SUNXI_UART_MSR_TERI       (BIT(2))
#define SUNXI_UART_MSR_DDSR       (BIT(1))
#define SUNXI_UART_MSR_DCTS       (BIT(0))
#define SUNXI_UART_MSR_ANY_DELTA  0x0F
#define MSR_SAVE_FLAGS SUNXI_UART_MSR_ANY_DELTA
/* Status Register */
#define SUNXI_UART_USR_RFF        (BIT(4))
#define SUNXI_UART_USR_RFNE       (BIT(3))
#define SUNXI_UART_USR_TFE        (BIT(2))
#define SUNXI_UART_USR_TFNF       (BIT(1))
#define SUNXI_UART_USR_BUSY       (BIT(0))
/* Halt Register */
#define SUNXI_UART_HALT_LCRUP     (BIT(2))
#define SUNXI_UART_HALT_FORCECFG  (BIT(1))
#define SUNXI_UART_HALT_HTX       (BIT(0))
/* RS485 Control and Status Register */
#define SUNXI_UART_RS485_RXBFA    (BIT(3))
#define SUNXI_UART_RS485_RXAFA    (BIT(2))

/* The global infor of UART channel. */

#if defined(CONFIG_ARCH_SUN8IW5)
#define SUNXI_UART_NUM			5
#endif

#if defined(CONFIG_ARCH_SUN8IW8)
#define SUNXI_UART_NUM			3
#endif

#if defined(CONFIG_ARCH_SUN8IW11)
#define SUNXI_UART_NUM			8
#endif
#if defined(CONFIG_ARCH_SUN8IW10) || defined(CONFIG_ARCH_SUN50IW1)
#define SUNXI_UART_NUM			6
#endif
#if defined(CONFIG_ARCH_SUN50IW2) \
	|| defined(CONFIG_ARCH_SUN50IW3) \
	|| defined(CONFIG_ARCH_SUN50IW6)
#define SUNXI_UART_NUM			4
#endif

#if defined(CONFIG_ARCH_SUN3IW1)
#define SUNXI_UART_NUM			3
#endif

#ifndef SUNXI_UART_NUM
#define SUNXI_UART_NUM			1
#endif

/* In 50/39 FPGA, two UART is available, but they share one IRQ.
   So we define the number of UART port as 1. */
#ifndef CONFIG_EVB_PLATFORM
#undef SUNXI_UART_NUM
#define SUNXI_UART_NUM			1
#endif

#if defined(CONFIG_ARCH_SUN50IW3) \
	|| defined(CONFIG_ARCH_SUN50IW6)
#define SUNXI_UART_FIFO_SIZE		256
#elif defined(CONFIG_ARCH_SUN3IW1)
#define SUNXI_UART_FIFO_SIZE		32
#else
#define SUNXI_UART_FIFO_SIZE		64
#endif

#define SUNXI_UART_DEV_NAME			"rigol keyboard"

struct platform_device *sw_uart_get_pdev(int uart_id);


#endif /* MISC_KEYBOARD_KEYBOARD_H_ */
