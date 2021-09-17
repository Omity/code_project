/*******************************************************************************
                      普源精电科技有限公司版权所有
********************************************************************************
  文 件 名: uart_phare.h
  功能描述: 键盘板按键驱动，针对有CPLD的键盘板程序进行的

  作    者: sn02241
  版    本: V1.00
  创建日期: 2017.03.27

  修改历史：
  作者          修改时间            版本           修改内容
  sn02241       2017.03.27          V1.00       initial
  sn03505       2021.03.11          V2.0        修改版

*******************************************************************************/
#ifndef UART_PHARE_H
#define UART_PHARE_H


#define KEY_PHARE_NO_ERR 1
#define KEY_PHARE_ERR   -1

#include "uart_phy.h"



/*----------------------------------  宏定义  ------------------------------------*/
#define KEY_COUNT               30     //有效按键个数，即面板上按键个数，包括旋钮支持的按键.
#define KEY_KNOB_COUNT          2      //旋钮相当于2个按键
#define KEY_TOTAL_COUNT         (KEY_COUNT + KEY_KNOB_COUNT)
/* 帧头 */
#define KEY_FRAME_HD            0xAA   //键值帧头
#define KEY_HD_LEN              1      //键值帧头长度，1字节

/* 帧中的键值数据长度，最大阵列（包括空余位置，sweep * detect）占字节数 */
#define KEY_VALID_LEN           7       //按键值长度，7字节
/* 版本信息 */
#define KEY_VER_LEN             1      //键盘版本号长度，1字节
#define KEY_VER_MASK            0xFF   //版本号掩码

/* CRC多项式为0b10011。校验值得出的方法为将从帧头开始，到版本号结束所有
 * 字节看做一个“长字”(帧头为最高字节)，左移4位后对10011取余，最终的CRC字节
 * 由4位0(高四位)和4位余数(低四位)组成。
 */
#define CRC4_POLYNOMIAL         0x13         //CRC4生成多项式系数
#define KEY_CRC4_LEN            1            //校验码长度，1字节

/* 长按键 */
#define KEY_LONG_FLAG            3         // 长按键标志位
#define COUNT_IS_FULL            255       // 长按键count值满

#define KEY_CRC4_POS            KEY_HD_LEN + KEY_VALID_LEN + KEY_VER_LEN  // 校验码位置

//键值一帧数据长度，10bytes：1byte帧头 + 7bytes键值 + 1byte长按键 + 1byte CRC校验码
#define KEY_FRAME_LEN           (KEY_HD_LEN + KEY_VALID_LEN + KEY_VER_LEN + KEY_CRC4_LEN) //10

/* 根据原理图和PCB定义各行列键值对应的键值掩码，(Col, Row)
 * Row表示Row = 7，即7行
 * Col表示Col = 8，即8行
 */
#define BIULD_KEY_MASK_BIT(bit)  (((unsigned long long)0x0000000000000001) << bit)

// Byte n( Row, col )
// Byte 1(1,1 -- 1,8)
#define KEY_MASK_BIT0            BIULD_KEY_MASK_BIT(0)    /* (1,1),bit0, k1 */
#define KEY_MASK_BIT1            BIULD_KEY_MASK_BIT(1)    /* (1,2),bit1, K2 */
#define KEY_MASK_BIT2            BIULD_KEY_MASK_BIT(2)    /* (1,3),bit2, K3 */
#define KEY_MASK_BIT3            BIULD_KEY_MASK_BIT(3)    /* (1,4),bit3, K30 */
#define KEY_MASK_BIT4            BIULD_KEY_MASK_BIT(4)    /* (1,5),bit4, k4 */
#define KEY_MASK_BIT5            BIULD_KEY_MASK_BIT(5)    /* (1,6),bit5, k5 */
#define KEY_MASK_BIT6            BIULD_KEY_MASK_BIT(6)    /* (1,7),bit6, k31*/
#define KEY_MASK_BIT7            BIULD_KEY_MASK_BIT(7)    /* (1,8),bit7, 没有这个 */

// Byte 2(2,1 -- 2,8)
#define KEY_MASK_BIT8            BIULD_KEY_MASK_BIT(8)    /* (2,1),bit8, 没有这个 */
#define KEY_MASK_BIT9            BIULD_KEY_MASK_BIT(9)    /* (2,2),bit9, 没有这个*/
#define KEY_MASK_BIT10           BIULD_KEY_MASK_BIT(10)   /* (2,3),bit10, k6 */
#define KEY_MASK_BIT11           BIULD_KEY_MASK_BIT(11)   /* (2,4),bit11, k7*/
#define KEY_MASK_BIT12           BIULD_KEY_MASK_BIT(12)   /* (2,5),bit12, k8 */
#define KEY_MASK_BIT13           BIULD_KEY_MASK_BIT(13)   /* (2,6),bit13, K9  */
#define KEY_MASK_BIT14           BIULD_KEY_MASK_BIT(14)   /* (2,7),bit14, K10 */
#define KEY_MASK_BIT15           BIULD_KEY_MASK_BIT(15)   /* (2,8),bit15, K11 */

// Byte 3(3,1 -- 3,8)
#define KEY_MASK_BIT16           BIULD_KEY_MASK_BIT(16)   /* (3,1),bit16, k12 */
#define KEY_MASK_BIT17           BIULD_KEY_MASK_BIT(17)   /* (3,2),bit17, 没有这个 */
#define KEY_MASK_BIT18           BIULD_KEY_MASK_BIT(18)   /* (3,3),bit18, 没有这个 */
#define KEY_MASK_BIT19           BIULD_KEY_MASK_BIT(19)   /* (3,4),bit19, 没有这个 */
#define KEY_MASK_BIT20           BIULD_KEY_MASK_BIT(20)   /* (3,5),bit20, 没有这个  */
#define KEY_MASK_BIT21           BIULD_KEY_MASK_BIT(21)   /* (3,6),bit21, 没有这个 */
#define KEY_MASK_BIT22           BIULD_KEY_MASK_BIT(22)   /* (3,7),bit22, k15 */
#define KEY_MASK_BIT23           BIULD_KEY_MASK_BIT(23)   /* (3,8),bit23, k16 */

// Byte 4(4,1 -- 4,8)
#define KEY_MASK_BIT24           BIULD_KEY_MASK_BIT(24)   /* (4,1),bit24, 没有这个 */
#define KEY_MASK_BIT25           BIULD_KEY_MASK_BIT(25)   /* (4,2),bit25, k17 */
#define KEY_MASK_BIT26           BIULD_KEY_MASK_BIT(26)   /* (4,3),bit26, k18 */
#define KEY_MASK_BIT27           BIULD_KEY_MASK_BIT(27)   /* (4,4),bit27, 没有这个 */
#define KEY_MASK_BIT28           BIULD_KEY_MASK_BIT(28)   /* (4,5),bit28, 没有这个 */
#define KEY_MASK_BIT29           BIULD_KEY_MASK_BIT(29)   /* (4,6),bit29, 没有这个 */
#define KEY_MASK_BIT30           BIULD_KEY_MASK_BIT(30)   /* (4,7),bit30, K19 */
#define KEY_MASK_BIT31           BIULD_KEY_MASK_BIT(31)   /* (4,8),bit31, 没有这个 */

// Byte 5(5,1 -- 5,8)
#define KEY_MASK_BIT32           BIULD_KEY_MASK_BIT(32)   /* (5,1),bit32, K20 */
#define KEY_MASK_BIT33           BIULD_KEY_MASK_BIT(33)   /* (5,2),bit33, k21 */
#define KEY_MASK_BIT34           BIULD_KEY_MASK_BIT(34)   /* (5,3),bit34, k22 */
#define KEY_MASK_BIT35           BIULD_KEY_MASK_BIT(35)   /* (5,4),bit35, k23 */
#define KEY_MASK_BIT36           BIULD_KEY_MASK_BIT(36)   /* (5,5),bit36, k24  */
#define KEY_MASK_BIT37           BIULD_KEY_MASK_BIT(37)   /* (5,6),bit37, 没有这个 */
#define KEY_MASK_BIT38           BIULD_KEY_MASK_BIT(38)   /* (5,7),bit38, 没有这个 */
#define KEY_MASK_BIT39           BIULD_KEY_MASK_BIT(39)   /* (5,8),bit39, 没有这个 */

// Byte 6(6,1 -- 6,8)
#define KEY_MASK_BIT40           BIULD_KEY_MASK_BIT(40)   /* (6,1),bit40, k25 */
#define KEY_MASK_BIT41           BIULD_KEY_MASK_BIT(41)   /* (6,2),bit41, 没有这个 */
#define KEY_MASK_BIT42           BIULD_KEY_MASK_BIT(42)   /* (6,3),bit42, k26  */
#define KEY_MASK_BIT43           BIULD_KEY_MASK_BIT(43)   /* (6,4),bit43, k27 */
#define KEY_MASK_BIT44           BIULD_KEY_MASK_BIT(44)   /* (6,5),bit44, 没有这个 */
#define KEY_MASK_BIT45           BIULD_KEY_MASK_BIT(45)   /* (6,6),bit45, k28 */
#define KEY_MASK_BIT46           BIULD_KEY_MASK_BIT(46)   /* (6,7),bit46, k29 */
#define KEY_MASK_BIT47           BIULD_KEY_MASK_BIT(47)   /* (7,3),bit47, 旋钮左转  */

// Byte 7(7,1 -- 7,8)
#define KEY_MASK_BIT48           BIULD_KEY_MASK_BIT(48)   /* (7,1),bit48, 旋钮右转 */
#define KEY_MASK_BIT49           BIULD_KEY_MASK_BIT(49)   /* (7,2),bit49, 旋钮按键 */
#define KEY_MASK_BIT50           BIULD_KEY_MASK_BIT(50)   /* (7,3),bit50, 没有这个 */
#define KEY_MASK_BIT51           BIULD_KEY_MASK_BIT(51)   /* (7,4),bit51, 没有这个 */
#define KEY_MASK_BIT52           BIULD_KEY_MASK_BIT(52)   /* (7,5),bit52, 没有这个 */
#define KEY_MASK_BIT53           BIULD_KEY_MASK_BIT(53)   /* (7,6),bit53, 没有这个 */
#define KEY_MASK_BIT54           BIULD_KEY_MASK_BIT(54)   /* (7,7),bit54, 没有这个 */
#define KEY_MASK_BIT55           BIULD_KEY_MASK_BIT(55)   /* (7,8),bit55, 没有这个 */

/* 重新指出knob的位置 */
//获得旋钮两位编码，knob1为【高位】，knob2为【低位】
#define KEY_MASK_KNOB_1_POS      48
#define KEY_MASK_KNOB_2_POS      47
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

/* 由于在键盘阵列中存在没有使用的节点，则每帧数据中除了后16 bit（ver + crc）和帧头 8bit 全部
   不使用外，中间56bit中也存在没有使用的位，逻辑不保证这些未使用位的状态，因此需要在解析前
   将没有用到的Bit位置为1，避免误判。根据原理图，得到无效位掩码 */
#define KEY_UNUSED_KEY          (KEY_MASK_BIT7  | KEY_MASK_BIT8  | KEY_MASK_BIT9 | KEY_MASK_BIT17  | KEY_MASK_BIT18 |\
                                 KEY_MASK_BIT19 | KEY_MASK_BIT20 | KEY_MASK_BIT21| KEY_MASK_BIT24 |\
                                 KEY_MASK_BIT27 | KEY_MASK_BIT28 | KEY_MASK_BIT29| KEY_MASK_BIT31 | KEY_MASK_BIT37 |\
                                 KEY_MASK_BIT38 | KEY_MASK_BIT39 | KEY_MASK_BIT41| KEY_MASK_BIT44 | KEY_MASK_BIT50 |\
                                 KEY_MASK_BIT51 | KEY_MASK_BIT52 | KEY_MASK_BIT53| KEY_MASK_BIT54 | KEY_MASK_BIT55)

#define KEY_UNUSED_MASK         (KEY_UNUSED_KEY|0xFF00000000000000)

/* 按键状态*/
#define KEY_STATE_REL    0
#define KEY_STATE_DN     1
#define KEY_STATE_LONG   2

/* linux系统下,按键值不能超过KEY_MAX(0x2ff),所以,保能将按键映射在现有的键值上, 摘自input.h.
 * (最好映射在标准键盘上,这样标准键盘也可控制仪器)
 */
#define BTN_TRIGGER_HAPPY1		0x2c0
#define BTN_TRIGGER_HAPPY2		0x2c1
#define BTN_TRIGGER_HAPPY3		0x2c2
#define BTN_TRIGGER_HAPPY4		0x2c3
#define BTN_TRIGGER_HAPPY5		0x2c4
#define BTN_TRIGGER_HAPPY6		0x2c5
#define BTN_TRIGGER_HAPPY7		0x2c6
#define BTN_TRIGGER_HAPPY8		0x2c7
#define BTN_TRIGGER_HAPPY9		0x2c8
#define BTN_TRIGGER_HAPPY10		0x2c9
#define BTN_TRIGGER_HAPPY11		0x2ca
#define BTN_TRIGGER_HAPPY12		0x2cb
#define BTN_TRIGGER_HAPPY13		0x2cc
#define BTN_TRIGGER_HAPPY14		0x2cd
#define BTN_TRIGGER_HAPPY15		0x2ce
#define BTN_TRIGGER_HAPPY16		0x2cf
#define BTN_TRIGGER_HAPPY17		0x2d0
#define BTN_TRIGGER_HAPPY18		0x2d1
#define BTN_TRIGGER_HAPPY19		0x2d2
#define BTN_TRIGGER_HAPPY20		0x2d3
#define BTN_TRIGGER_HAPPY21		0x2d4
#define BTN_TRIGGER_HAPPY22		0x2d5
#define BTN_TRIGGER_HAPPY23		0x2d6
#define BTN_TRIGGER_HAPPY24		0x2d7
#define BTN_TRIGGER_HAPPY25		0x2d8
#define BTN_TRIGGER_HAPPY26		0x2d9
#define BTN_TRIGGER_HAPPY27		0x2da
#define BTN_TRIGGER_HAPPY28		0x2db
#define BTN_TRIGGER_HAPPY29		0x2dc
#define BTN_TRIGGER_HAPPY30		0x2dd
#define BTN_TRIGGER_HAPPY31		0x2de
#define BTN_TRIGGER_HAPPY32		0x2df
#define BTN_TRIGGER_HAPPY33		0x2e0
#define BTN_TRIGGER_HAPPY34		0x2e1
#define BTN_TRIGGER_HAPPY35		0x2e2
#define BTN_TRIGGER_HAPPY36		0x2e3
#define BTN_TRIGGER_HAPPY37		0x2e4
#define BTN_TRIGGER_HAPPY38		0x2e5KEY_MASK_BIT19 | KEY_MASK_BIT20 | KEY_MASK_BIT21| KEY_MASK_BIT24
#define BTN_TRIGGER_HAPPY39		0x2e6
#define BTN_TRIGGER_HAPPY40		0x2e7

/************************************/
#define RIGOL_KBD_KEY_OUTPUT1      BTN_TRIGGER_HAPPY1  // output1 
#define RIGOL_KBD_KEY_OUTPUT2      BTN_TRIGGER_HAPPY2  // output2
#define RIGOL_KBD_KEY_OUTPUT3      BTN_TRIGGER_HAPPY3  // output3
#define RIGOL_KBD_KEY_ALL_OFF      BTN_TRIGGER_HAPPY4  // all-onoff
#define RIGOL_KBD_KEY_DIR_UP       BTN_TRIGGER_HAPPY5  // dir-up
#define RIGOL_KBD_KEY_NUM_0        BTN_TRIGGER_HAPPY6  // num0
#define RIGOL_KBD_KEY_PRESET       BTN_TRIGGER_HAPPY7  // preset
#define RIGOL_KBD_KEY_ON_OFF_1     BTN_TRIGGER_HAPPY8  // on/off1
#define RIGOL_KBD_KEY_ON_OFF_2     BTN_TRIGGER_HAPPY9  // on/off2
#define RIGOL_KBD_KEY_ON_OFF_3     BTN_TRIGGER_HAPPY10 // on/off3

#define RIGOL_KBD_KEY_NUM_9        BTN_TRIGGER_HAPPY11  // num9
#define RIGOL_KBD_KEY_NUM_POINT    BTN_TRIGGER_HAPPY12 // num-point
#define RIGOL_KBD_KEY_NUM_1        BTN_TRIGGER_HAPPY13 // num1
#define RIGOL_KBD_KEY_NUM_2        BTN_TRIGGER_HAPPY14 // num2
#define RIGOL_KBD_KEY_DIR_LEFT     BTN_TRIGGER_HAPPY15 // dir-left
#define RIGOL_KBD_KEY_NUM_8        BTN_TRIGGER_HAPPY16 // num8
#define RIGOL_KBD_KEY_NUM_3        BTN_TRIGGER_HAPPY17 // num3
#define RIGOL_KBD_KEY_DIR_RIGHT    BTN_TRIGGER_HAPPY18 // dir-right
#define RIGOL_KBD_KEY_ARB          BTN_TRIGGER_HAPPY19  // arb
#define RIGOL_KBD_KEY_ANALYZE      BTN_TRIGGER_HAPPY20 // analyze

#define RIGOL_KBD_KEY_NUM_7        BTN_TRIGGER_HAPPY21 // num7
#define RIGOL_KBD_KEY_NUM_6        BTN_TRIGGER_HAPPY22 // num6
#define RIGOL_KBD_KEY_NUM_5        BTN_TRIGGER_HAPPY23 // num5
#define RIGOL_KBD_KEY_NUM_4        BTN_TRIGGER_HAPPY24 // num4
#define RIGOL_KBD_KEY_UTIL         BTN_TRIGGER_HAPPY25 // utility
#define RIGOL_KBD_KEY_STORE        BTN_TRIGGER_HAPPY26 // Store
#define RIGOL_KBD_KEY_BACK         BTN_TRIGGER_HAPPY27 // back
#define RIGOL_KBD_KEY_DIR_DOWN     BTN_TRIGGER_HAPPY28 // dir-down
#define RIGOL_KBD_KEY_ENTER        BTN_TRIGGER_HAPPY29 // enter
#define RIGOL_KBD_KEY_KNOB         BTN_TRIGGER_HAPPY30 // knob key

/***************** 旋钮(3) ********************/
#define RIGOL_KBD_KEY_KONB_LEFT    BTN_TRIGGER_HAPPY31 // konb_left
#define RIGOL_KBD_KEY_KONB_RIGHT   BTN_TRIGGER_HAPPY32 // konb_right


#define KEY_UNKNOWN  240
#define RIGOL_KBD_KEY_INVALID   KEY_UNKNOWN


static unsigned short rigol_keycodes[KEY_TOTAL_COUNT] = {
RIGOL_KBD_KEY_OUTPUT1,  RIGOL_KBD_KEY_OUTPUT2,   RIGOL_KBD_KEY_OUTPUT3,  RIGOL_KBD_KEY_ALL_OFF,
RIGOL_KBD_KEY_DIR_UP,   RIGOL_KBD_KEY_NUM_0,     RIGOL_KBD_KEY_PRESET,   RIGOL_KBD_KEY_ON_OFF_1,
RIGOL_KBD_KEY_ON_OFF_2, RIGOL_KBD_KEY_ON_OFF_3,  RIGOL_KBD_KEY_NUM_9,    RIGOL_KBD_KEY_NUM_POINT, 
RIGOL_KBD_KEY_NUM_1,    RIGOL_KBD_KEY_NUM_2,     RIGOL_KBD_KEY_DIR_LEFT, RIGOL_KBD_KEY_NUM_8,       
RIGOL_KBD_KEY_NUM_3,    RIGOL_KBD_KEY_DIR_RIGHT, RIGOL_KBD_KEY_ARB,      RIGOL_KBD_KEY_ANALYZE, 
RIGOL_KBD_KEY_NUM_7,    RIGOL_KBD_KEY_NUM_6,     RIGOL_KBD_KEY_NUM_5,    RIGOL_KBD_KEY_NUM_4,     
RIGOL_KBD_KEY_UTIL,     RIGOL_KBD_KEY_STORE,     RIGOL_KBD_KEY_BACK,     RIGOL_KBD_KEY_DIR_DOWN, 
RIGOL_KBD_KEY_ENTER,    RIGOL_KBD_KEY_KNOB ,
};

//这个数组与rigol_keycodes是一一对应的！！！
//该数组表示第几位表示rigol_keycodes相同位置按键
static unsigned long long rigol_keycode_bits[KEY_COUNT] = {
    KEY_MASK_BIT0,   KEY_MASK_BIT1,   KEY_MASK_BIT2,  KEY_MASK_BIT3,
    KEY_MASK_BIT4,   KEY_MASK_BIT5,   KEY_MASK_BIT6,  KEY_MASK_BIT10,
    KEY_MASK_BIT11,  KEY_MASK_BIT12,  KEY_MASK_BIT13, KEY_MASK_BIT14,
    KEY_MASK_BIT15,  KEY_MASK_BIT16,  KEY_MASK_BIT22, KEY_MASK_BIT23,
    KEY_MASK_BIT25,  KEY_MASK_BIT26,  KEY_MASK_BIT30, KEY_MASK_BIT32, 
    KEY_MASK_BIT33,  KEY_MASK_BIT34,  KEY_MASK_BIT35, KEY_MASK_BIT36,
    KEY_MASK_BIT40,  KEY_MASK_BIT42,  KEY_MASK_BIT43, KEY_MASK_BIT45, 
    KEY_MASK_BIT46,  KEY_MASK_BIT49,
};

//这个数组与rigol_keycodes是一一对应的！！！
#if KEY_DEBUG
static unsigned char rigol_keycodes_debug_string[KEY_COUNT][20] = {
    "output1",   "output2",   "output3",   "all-onoff",   "dir-up",
    "num0",      "preset",    "on/off1",   "on/off2",    "on/off3",
    "num9",      "num-point",  "num1",      "num2",     "dir-left",  
    "num8" ,     "num3",       "dir-right",  "arb",      "analyze", 
    "num7",      "num6",        "num5",       "num4" ,    "utility" , 
    "store",     "back",        "dir-down" , "enter",     "knob key",
};
#endif

//按键状态数组
static unsigned char rigol_key_state[3] = {
	"release", 
	"down",
	"longkey",
};

/* 上次按键值 */
static unsigned long long g_u64KeyValueOld = 0xFFFFFFFFFFFFFFFF;

/* input子系统决定必须上报一次按下再上报一次抬起，依次循环进行，
 * 否则会上报不成功，因此需要记住上次上报的状态，默认为release
 */
extern volatile unsigned int g_u32KnobLeftStateOld;
extern volatile unsigned int g_u32KnobRightStateOld;

/* 位操作 */
#define CHECK_BIT(b,v)          ((v >> b) & 0x1) //检查某位是否为1
#define HIGH_BIT_MASK           0x80             //字节最高位掩码
#define LOW_BIT_MASK            0x01             //字节最低位掩码


/*************************************** func c++ *******************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  * 函    数：rigol_kbd_knob_value_get
  * 描    述：获取按键旋钮值
  * 输入参数：
  *           参数名称                  参数类型           参数说明
  *           u64KeyValue               u64             获取的键值  
          
  * 返 回 值：无
  * 说    明：外部函数
 ******************************************************************************/
static unsigned short rigol_kbd_knob_value_get(unsigned long long u64KeyValue);



/*******************************************************************************
  * 函    数：rigol_kbd_parse_key
  * 描    述：分析按键值
  * 输入参数：
  *           参数名称       参数类型           参数说明
  *           u64keyValue     u64*           获取的按键值
  *           u32keycode      u32*           获取的键值是否有效
  *           u32keystate      u32*          获取的按键状态（按下还是释放)
  * 返 回 值：错误码
  * 说    明：外部函数
 ******************************************************************************/
static int rigol_kbd_parse_key(unsigned long long *u64keyValue, unsigned int *u32keycode, 
unsigned int *u32keystate, unsigned int *u32LongKeyCount);




/*******************************************************************************
  * 函    数：kbdGetAndReportKey
  * 描    述：获取和上报按键值
  * 输入参数：
  *           参数名称      参数类型           参数说明
  *           pu8keybuf     u8*             接收的数据         
  * 返 回 值：错误码
  * 说    明：外部函数
 ******************************************************************************/
static int kbdGetAndReportKey(unsigned char *pu8keybuf);


/*******************************************************************************
  * 函    数：drvCRC4OneBit
  * 描    述：对按键值做CRC校验
  * 输入参数：
  *           参数名称                 参数类型             参数说明
  *           pu8DataBuf               u8*              待CRC校验数据  
  *           u32DataLen               u32             带检测的数据长度           
  * 返 回 值：错误码
  * 说    明：外部函数
 ******************************************************************************/
unsigned char drvCRC4OneBit(unsigned char *pu8DataBuf, unsigned int u32DataLen);

/*******************************************************************************
 函 数 名:  PhareDataThread
 描    述:  解析和处理接受数值的数组
 输入参数:
           参数名称                  参数类型           参数说明
           pvResvData               void*            串口接收的数据  
 输出参数：
 返 回 值:
 说    明：
*******************************************************************************/
extern void *PhareDataThread(void*  pvResvData);

#ifdef __cplusplus

}
#endif


#endif // UART_PHARE_H
/*************************************** end of file*******************************************/
