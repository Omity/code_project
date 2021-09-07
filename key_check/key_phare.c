/*******************************************************************************
                普源精电科技技术有限公司版权所有
********************************************************************************
  源文件名: key_phare.c
  功能描述: 按键键值解析
  作    者: YangYuLiang
  版    本: Ver1.0
  创建日期: 2020.03.12
    
  修改历史:
  修改日期：
  版 本 号：
  修 改 人：
  修改内容：
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <stdbool.h>
#include <time.h>
// 用户定义头文件

#include "key_phare.h"
#include "uart_queue.h"

/* input子系统决定必须上报一次按下再上报一次抬起，依次循环进行，
 * 否则会上报不成功，因此需要记住上次上报的状态，默认为release
 */
volatile unsigned int g_u32KnobLeftStateOld  = KEY_STATE_REL;
volatile unsigned int g_u32KnobRightStateOld = KEY_STATE_REL;
/*******************************************************************************
  * 函    数：rigol_kbd_knob_value_get
  * 描    述：获取按键旋钮值
  * 输入参数：
  *           参数名称                  参数类型           参数说明
  *           u64KeyValue               u64             获取的键值  
          
  * 返 回 值：无
  * 说    明：外部函数
 ******************************************************************************/
static unsigned short rigol_kbd_knob_value_get(unsigned long long u64KeyValue)
{
	unsigned char   u8KnobValueOld = 0;
	unsigned char  u8KnobValueNew = 0;
    unsigned short  u16KeyValue    = RIGOL_KBD_KEY_INVALID;
    static short s16Left  = 0;
    static short s16Right = 0;

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
				s16Left++;kbdGetAndReportKey
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
        u16KeyValue = RIGOL_KBD_KEY_KONB_LEFT;
    }
    else if(s16Right == KEY_KNOB_IDENTIFY_DIR_STEP)
    {
        s16Right = 0;
        s16Left  = 0;
        u16KeyValue = RIGOL_KBD_KEY_KONB_RIGHT;
    }
    
    return (u16KeyValue);
}

/*******************************************************************************
  * 函    数：rigol_kbd_parse_key
  * 描    述：分析按键值
  * 输入参数：
  *           参数名称       参数类型           参数说明
  *           u64keyValue     u64*           获取的按键值
  *           u32keycode      u32*           获取的键值是否有效
  *           u32keystate     u32*          获取的按键状态（按下还是释放)
  * 返 回 值：错误码
  * 说    明：外部函数
 ******************************************************************************/
static int rigol_kbd_parse_key(unsigned long long *u64keyValue, unsigned int *u32keycode, unsigned int *u32keystate)
{
    int s32Ret = -1;
	int i = 0;
    unsigned long long u64KeyValueNew = *u64keyValue;

    // 确认按键值有没有变化（亦或相同为0, 不同为1）
    unsigned long long u64KeyMask = 0; 
   
    u64KeyMask = u64KeyValueNew ^ g_u64KeyValueOld;
    
    //两次按键值没有改变，说明是重复数据（按下和抬起的键值不同）
    if((u64KeyMask == 0) || (u32keystate != 1))
    {
        key_debug("==============  no change key============== \n");
        return s32Ret;
    }

    //1->0 means down,0->1 means release
    *u32keystate = (u64KeyValueNew & u64KeyMask) ? KEY_STATE_REL : KEY_STATE_DN;
    *u32keycode  = RIGOL_KBD_KEY_INVALID;

    
    //菜单键（按键所属区域的判定）
	//如果不是旋钮则判断是否为按键
	for(i = 0; i < KEY_COUNT; i++)
	{
		if(u64KeyMask == rigol_keycode_bits[i])
		{
            
			s32Ret = 0;
			*u32keycode = rigol_keycodes[i];
    		// set current key value as last key value.
    		g_u64KeyValueOld = *u64keyValue;
			return s32Ret;
		}
	}


	//判断是否为旋钮，判断依据是：检测旧值与新值比较变化的位是否为knob的掩码（两个）
	if(u64KeyMask == KEY_MASK_KNOB_1 || u64KeyMask == KEY_MASK_KNOB_2)
	{
		//判断为旋钮
		s32Ret = 0;
		key_debug("knob: 0x%llx-0x%llx!\n", u64KeyValueNew, u64KeyMask);
		*u32keycode = rigol_kbd_knob_value_get(u64KeyValueNew);
	}
	else
	{
        	key_debug("INVALID key!\n");
	}

    // set current key value as last key value.
    g_u64KeyValueOld = *u64keyValue;
    return s32Ret;
}


/*******************************************************************************
  * 函    数：kbdGetAndReportKey
  * 描    述：获取和上报按键值
  * 输入参数：
  *           参数名称      参数类型           参数说明
  *           pu8keybuf     u8*             接收的数据         
  * 返 回 值：错误码
  * 说    明：外部函数
 ******************************************************************************/
static int kbdGetAndReportKey(unsigned char *pu8keybuf)
{
    // 函数返回值
    int s32Ret = 1;

    int i = 0;
    // 获取的按键值
    unsigned int u32keycode = 0;
    // 获取的键值缓存值
    unsigned long long u64KeyTempValue = 0;
    // 获取的按键状态
    unsigned int u32keystate = 0;
    // 键值的临时版本号
    unsigned char  u32TmpVersion = 0;
    memcpy(&u64KeyTempValue, pu8keybuf, KEY_VALID_LEN); 
 
    //长按键
    char *ps8IsLongKey = NULL;       //add at 2021年09月06日16:27:32
 
    /* mask the unused key first */
    u64KeyTempValue |= KEY_UNUSED_MASK;
    
    // 用户按下长按键
    if( KEY_VER_FLAG == pu8keybuf[KEY_VER_POS])
    {
        u32keystate = KEY_LONG_FLAG;
    }
    // 按键解析函数
    s32Ret = rigol_kbd_parse_key(&u64KeyTempValue, &u32keycode, &u32keystate);
    
    if(!s32Ret && u32keycode != RIGOL_KBD_KEY_INVALID)
    {
		//判断是否为旋钮，判断依据是：检测旧值与新值比较变化的位是否为knob的掩码（两个）
		if(u32keycode == RIGOL_KBD_KEY_KONB_LEFT)
		{
			//判断为旋钮
			key_debug("knob: %s\n", "left");
            		// 三木运算符 (获取按键是否按下)
			u32keystate = (g_u32KnobLeftStateOld == KEY_STATE_REL) ? KEY_STATE_DN:KEY_STATE_REL;
			g_u32KnobLeftStateOld = u32keystate;
		}
		else if(u32keycode == RIGOL_KBD_KEY_KONB_RIGHT)
		{
			//判断为旋钮
			key_debug("knob: %s\n", "right");
			u32keystate = g_u32KnobRightStateOld==KEY_STATE_REL?KEY_STATE_DN:KEY_STATE_REL;
			g_u32KnobRightStateOld = u32keystate;
		}
		else//按键值
		{
			for(i = 0; i < KEY_COUNT; i++)
			{
				if((u32keycode) == rigol_keycodes[i])
				{
                    //是否长按键
                    if(KEY_VER_FLAG == pu8keybuf[KEY_VER_POS]) 
                    {                        
                        ps8IsLongKey = strcat((char*)rigol_keycodes_debug_string[i], IS_LONG_KEY);
                        key_debug("%s button, %s\n", ps8IslongKey, u32keystate?"down":"release");
                    }
                    else
                    {
                        key_debug("%s button, %s\n", ps8IslongKey,rigol_keycodes_debug_string[i],
														 u32keystate?"down":"release");
                    }
					break;
				
			}
        }
    }
    return s32Ret;
}

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
unsigned char 
drvCRC4OneBit(unsigned char *pu8DataBuf, unsigned int u32DataLen)
{
    // CRC校验结果值 
	unsigned char u8DataCrcValue = 0;
    // 存储暂时的数据的缓存区
    unsigned char *pu8Data = NULL;
    // 暂时CRC的校验数据
    unsigned char  u8TmpData = 0;
    // CRC校验长度
	unsigned int u32TempDataLen = u32DataLen * 8 + 4;
	unsigned int u32Subscripts = 0; 
    unsigned int u32RightShift = 0;
	unsigned int u32i = 0;

    // 申请空间
	pu8Data = (unsigned char*)malloc(u32TempDataLen);
	// 赋值，原数据每一位保存为一个字节
	for(u32i = 0; u32i < u32TempDataLen - 4; u32i++)
	{
        /* 计算数组的角标和所在的比特位，每8位一个字节
		 * 处以8，商为下角标，与树为比特位（从高算）
		 */
        u32Subscripts = u32i >> 0x3;
		u32RightShift = u32i & 0x7;
		/* 计算数组的角标和所在的比特位，每8位一个字节 */ 
		pu8Data[u32i] = !!(pu8DataBuf[u32Subscripts] & (HIGH_BIT_MASK >> u32RightShift));
	}

	// 最后四位补0
	for(u32i = u32TempDataLen - 4; u32i < u32TempDataLen; u32i++)
	{
		pu8Data[u32i] = 0;
	}

	// 预先取出高4位
	for(u32i = 0; u32i < 4; u32i++)
	{
		u8DataCrcValue |= (pu8Data[u32i] & LOW_BIT_MASK) << (3 - u32i);
	}

	// 计算CRC4
	for(u32i = 4; u32i < u32TempDataLen; u32i++)
	{
		/*  凑够要计算的5位：
		 *  1.当第一次进入时，u8Crc保存4位，左移一位，然后补上新的低位
		 *  2.当再次进入时，u8Crc是计算后的值，也是只有4位，左移一位补最低位
		 */
		u8TmpData = (u8DataCrcValue << 0x1) | (pu8Data[u32i] & LOW_BIT_MASK);
		u8DataCrcValue = CHECK_BIT(4, u8TmpData) ? (u8TmpData ^ CRC4_POLYNOMIAL) : u8TmpData;
	}

	free(pu8Data);
	pu8Data = NULL;
    return u8DataCrcValue;
}


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
void *PhareDataThread(void*  pvResvData)
{
    // 存储按键值数组
    unsigned char au8KeyFrame[UART_RESV_DATA_LEN] = { 0 };
    // 存储一组串口数组
    unsigned char au8UartDataBuf[UART_RESV_DATA_LEN] = { 0 };
    // CRC校验值
    unsigned char u8CrcDataValue = 0;
    
    int i = 0;
                        
    // 数据队列
    while(1)
    { 
        //等待信号(>0) 
        sem_wait(&g_pstkeyDeal->semKeyDeal);
        
        // 一直获取键值数据，直到长度小于必要长度10
        while(g_pstkeyDeal->s32KeyDealFlag > 0)
        { 
            
           // 获取键值解析队列
            for( i = 0; i < UART_RESV_DATA_LEN; ++i)  
            {
               queueOut( g_pstResvDataQueue, au8UartDataBuf + i); 
            } 
            // 计算CRC4校验值
	    u8CrcDataValue = drvCRC4OneBit(au8UartDataBuf, KEY_CRC4_POS);
    	    // 判断帧头是否为0xAA，并判断校验值是否正确，接收数据完成
            if((KEY_FRAME_HD == au8UartDataBuf[0] )
			     && (u8CrcDataValue == au8UartDataBuf[KEY_CRC4_POS]))
            {
                
		// 拷贝除帧头部分的数据（带上版本号）
                memcpy(au8KeyFrame, au8UartDataBuf + 1, KEY_VALID_LEN + 1);
                kbdGetAndReportKey(au8KeyFrame);        
            }

            // 队列数据小于键值解析长度
           if ( queueLength( g_pstResvDataQueue ) < UART_RESV_DATA_LEN)
           {
               // 清标志位
               g_pstkeyDeal->s32KeyDealFlag = 0; 
               // 清除键值数据
               queueClear(g_pstResvDataQueue);
           }            
            
        }

        //信号量+1，进而触发Resv的任务
	sem_post(&g_pstkeyDeal->semUartResv);
    }
		
}








