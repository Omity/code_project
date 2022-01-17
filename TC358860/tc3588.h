#ifndef _TC3588_H_
#define _TC3588_H_

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/sysfs.h>
#include <linux/delay.h>
#include <linux/kernel.h>

#define TC3588_ADDR_LENGTH    2

#define IIC_CMD_WRITE      0
#define IIC_CMD_READ       1
#define IIC_CMD_DELAY      2
#define IIC_CMD_CHECK      3

#define DELAY_S            (1*1000*1000)

unsigned long tc3588_init_cmd[][4] = {
	/** 命令格式:  数据       长度        mask    check data
	 */
		//RS1_START
		//IO Voltahge Setting
		{0x000000010008,	0x06,	IIC_CMD_WRITE,	0x00},   //IOB_CTRL1
		//Boot Settings
		{0x000065900010,	0x06,	IIC_CMD_WRITE,	0x00},   //BootWaitCount
		{0x000404080410,	0x06,	IIC_CMD_WRITE,	0x00},   //Boot Set0
		{0x011D00080810,	0x06,	IIC_CMD_WRITE,	0x00},   //Boot Set1
		{0x030002740C10,	0x06,	IIC_CMD_WRITE,	0x00},   //Boot Set2
		{0x00C800201010,	0x06,	IIC_CMD_WRITE,	0x00},   //Boot Set3
		{0x000000011410,	0x06,	IIC_CMD_WRITE,	0x00},   //Boot Ctrl
		//delay
		{1000,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x1810,	0x06,	IIC_CMD_READ,	0x000000021810},   //Boot Status--->value: 00000002
		//Check if 0x1018<bit2:0> is expected value, If not wait until 0x1018<bit2:0> will be expected value
		//Setting for 26MHz REFCLK
		{0x410000,	0x03,	IIC_CMD_WRITE,	0x00},   //ExtCLKSel
		{0x03020800,	0x04,	IIC_CMD_WRITE,	0x00},   //DPREFPLLCTRL_1
		{0x0C02301D0C00,	0x06,	IIC_CMD_WRITE,	0x00},   //DPREFPLLCTRL_2
		//Internal PCLK Setting for Non Preset or REFCLK=26MHz
		{0x3A05B0,	0x03,	IIC_CMD_WRITE,	0x00},   //SET CG_VIDPLL_CTRL1
		{0x0406B0,	0x03,	IIC_CMD_WRITE,	0x00},   //SET CG_VIDPLL_CTRL2
		{0x2F07B0,	0x03,	IIC_CMD_WRITE,	0x00},   //SET CG_VIDPLL_CTRL3
		{0x0008B0,	0x03,	IIC_CMD_WRITE,	0x00},   //SET CG_VIDPLL_CTRL4
		{0x2109B0,	0x03,	IIC_CMD_WRITE,	0x00},   //SET CG_VIDPLL_CTRL5
		{0x0E0AB0,	0x03,	IIC_CMD_WRITE,	0x00},   //SET CG_VIDPLL_CTRL6
		//DSI Clock setting for Non Preset or REFCLK=26MHz
		{0x0003262DB041,	0x06,	IIC_CMD_WRITE,	0x00},   //MIPI_PLL_CNF
		{0x00001B51BC41,	0x06,	IIC_CMD_WRITE,	0x00},   //MIPI_PLL_PARA
		{0x00000030C041,	0x06,	IIC_CMD_WRITE,	0x00},   //MIPI_PLL_FIX
		{0x0000963DA841,	0x06,	IIC_CMD_WRITE,	0x00},   //DSI_PLL_LOCKCNT
		{0x000000031410,	0x06,	IIC_CMD_WRITE,	0x00},   //Boot Ctrl
		//delay
		{1000,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x1810,	0x06,	IIC_CMD_READ,	0x000000061810},   //Boot Status--->value: 00000006
		//Check if 0x1018<bit2:0> is expected value, If not wait until 0x1018<bit2:0> will be expected value
		//Additional Setting for eDP
		//{0x010082,	0x03,	IIC_CMD_CHECK,	0x00},    //Sink Count
		{0x410380,	0x03,	IIC_CMD_WRITE,	0x00},   //Max Downspread
		{0x0D00B4,	0x03,	IIC_CMD_WRITE,	0x00},   //AL Mode Control Link
		{0x1232B6,	0x03,	IIC_CMD_WRITE,	0x00},   //PL Mainlink REFCLK Ctrl
		{0xA001B4,	0x03,	IIC_CMD_WRITE,	0x00},   //FULLTRANTIME0
		{0xF702B4,	0x03,	IIC_CMD_WRITE,	0x00},   //FULLTRANTIME1
		{0x0303B4,	0x03,	IIC_CMD_WRITE,	0x00},   //FULLTRANTIME2
		{0x9004B4,	0x03,	IIC_CMD_WRITE,	0x00},   //FASTTRNTIME0
		{0x6505B4,	0x03,	IIC_CMD_WRITE,	0x00},   //FASTTRNTIME1
		{0x0006B4,	0x03,	IIC_CMD_WRITE,	0x00},   //FASTTRNTIME2
		{0x0D09B4,	0x03,	IIC_CMD_WRITE,	0x00},   //AL_LOCKTIME_CTRL0
		{0x080AB4,	0x03,	IIC_CMD_WRITE,	0x00},   //AL_LOCKTIME_CTRL1
		{0x020BB4,	0x03,	IIC_CMD_WRITE,	0x00},   //AL_LOCKTIME_CTRL2
		{0x1420B4,	0x03,	IIC_CMD_WRITE,	0x00},   //AL_OFFDLY_CTRL0
		{0x0521B4,	0x03,	IIC_CMD_WRITE,	0x00},   //AL_OFFDLY_CTRL1
		{0x2822B4,	0x03,	IIC_CMD_WRITE,	0x00},   //AL_OFFDLY_CTRL2
		{0x0A23B4,	0x03,	IIC_CMD_WRITE,	0x00},   //AL_OFFDLY_CTRL3
		{0x3C24B4,	0x03,	IIC_CMD_WRITE,	0x00},   //AL_OFFDLY_CTRL4
		{0x0F25B4,	0x03,	IIC_CMD_WRITE,	0x00},   //AL_OFFDLY_CTRL5
		{0x5026B4,	0x03,	IIC_CMD_WRITE,	0x00},   //AL_OFFDLY_CTRL6
		{0x1427B4,	0x03,	IIC_CMD_WRITE,	0x00},   //AL_OFFDLY_CTRL7
		{0x2828B4,	0x03,	IIC_CMD_WRITE,	0x00},   //AL_SLEEPDLY_CTRL0
		{0x0A29B4,	0x03,	IIC_CMD_WRITE,	0x00},   //AL_SLEEPDLY_CTRL1
		{0x502AB4,	0x03,	IIC_CMD_WRITE,	0x00},   //AL_SLEEPDLY_CTRL2
		{0x142BB4,	0x03,	IIC_CMD_WRITE,	0x00},   //AL_SLEEPDLY_CTRL3
		{0x782CB4,	0x03,	IIC_CMD_WRITE,	0x00},   //AL_SLEEPDLY_CTRL4
		{0x1E2DB4,	0x03,	IIC_CMD_WRITE,	0x00},   //AL_SLEEPDLY_CTRL5
		{0xA02EB4,	0x03,	IIC_CMD_WRITE,	0x00},   //AL_SLEEPDLY_CTRL6
		{0x282FB4,	0x03,	IIC_CMD_WRITE,	0x00},   //AL_SLEEPDLY_CTRL7
		//DPRX CAD Register Setting
		{0xFF8EB8,	0x03,	IIC_CMD_WRITE,	0x00},   //Set CR_OPT_WCNT0
		{0xFF8FB8,	0x03,	IIC_CMD_WRITE,	0x00},   //Set CR_OPT_WCNT1
		{0xFF9AB8,	0x03,	IIC_CMD_WRITE,	0x00},   //Set CR_OPT_WCNT2
		{0xFF9BB8,	0x03,	IIC_CMD_WRITE,	0x00},   //Set CR_OPT_WCNT3
		{0x0E00B8,	0x03,	IIC_CMD_WRITE,	0x00},   //Set CDR_PHASE_LP_EN
		{0x0226BB,	0x03,	IIC_CMD_WRITE,	0x00},   //RX_VREG_VALUE
		{0x2001BB,	0x03,	IIC_CMD_WRITE,	0x00},   //RX_VREG_ENABLE
		{0xF1C0B8,	0x03,	IIC_CMD_WRITE,	0x00},   //RX_CDR_LUT1
		{0xF1C1B8,	0x03,	IIC_CMD_WRITE,	0x00},   //RX_CDR_LUT2
		{0xF0C2B8,	0x03,	IIC_CMD_WRITE,	0x00},   //RX_CDR_LUT3
		{0xF0C3B8,	0x03,	IIC_CMD_WRITE,	0x00},   //RX_CDR_LUT4
		{0xF0C4B8,	0x03,	IIC_CMD_WRITE,	0x00},   //RX_CDR_LUT5
		{0xF0C5B8,	0x03,	IIC_CMD_WRITE,	0x00},   //RX_CDR_LUT6
		{0xF0C6B8,	0x03,	IIC_CMD_WRITE,	0x00},   //RX_CDR_LUT7
		{0xF0C7B8,	0x03,	IIC_CMD_WRITE,	0x00},   //RX_CDR_LUT8
		{0x000BB8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP1P1
		{0x0033B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP1P2
		{0x005BB8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP1P3
		{0x0010B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP2P1
		{0x0038B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP2P2
		{0x0060B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP2P3
		{0x0015B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP2P4
		{0x003DB8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP2P5
		{0x0065B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP2P6
		{0x001AB8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP2P7
		{0x0042B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP2P8
		{0x006AB8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP2P9
		{0x001FB8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP3P1
		{0x0047B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP3P2
		{0x006FB8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP3P3
		{0x0024B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP4P1
		{0x004CB8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP4P2
		{0x0074B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP4P3
		{0x0029B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP4P4
		{0x0051B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP4P5
		{0x0079B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP4P6
		{0x002EB8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP5P7
		{0x0056B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP5P2
		{0x007EB8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_CP5P3
		{0x1090BB,	0x03,	IIC_CMD_WRITE,	0x00},   //ctle_em_data_rate_control_0[7:0]
		{0x0F91BB,	0x03,	IIC_CMD_WRITE,	0x00},   //ctle_em_data_rate_control_1[7:0]
		{0xF692BB,	0x03,	IIC_CMD_WRITE,	0x00},   //ctle_em_data_rate_control_2[7:0]
		{0x1093BB,	0x03,	IIC_CMD_WRITE,	0x00},   //ctle_em_data_rate_control_3[7:0]
		{0x0F94BB,	0x03,	IIC_CMD_WRITE,	0x00},   //ctle_em_data_rate_control_4[7:0]
		{0xF695BB,	0x03,	IIC_CMD_WRITE,	0x00},   //ctle_em_data_rate_control_5[7:0]
		{0x1096BB,	0x03,	IIC_CMD_WRITE,	0x00},   //ctle_em_data_rate_control_6[7:0]
		{0x0F97BB,	0x03,	IIC_CMD_WRITE,	0x00},   //ctle_em_data_rate_control_7[7:0]
		{0xF698BB,	0x03,	IIC_CMD_WRITE,	0x00},   //ctle_em_data_rate_control_8[7:0]
		{0x1099BB,	0x03,	IIC_CMD_WRITE,	0x00},   //ctle_em_data_rate_control_A[7:0]
		{0x0F9ABB,	0x03,	IIC_CMD_WRITE,	0x00},   //ctle_em_data_rate_control_B[7:0]
		{0xF69BBB,	0x03,	IIC_CMD_WRITE,	0x00},   //ctle_em_data_rate_control_0[7:0]
		{0x038AB8,	0x03,	IIC_CMD_WRITE,	0x00},   //CR_OPT_CTRL
		{0x0396B8,	0x03,	IIC_CMD_WRITE,	0x00},   //EQ_OPT_CTRL
		{0x07D1BB,	0x03,	IIC_CMD_WRITE,	0x00},   //ctle_em_contro_1
		{0x07B0BB,	0x03,	IIC_CMD_WRITE,	0x00},   //eye_configuration_0
		{0x048BB8,	0x03,	IIC_CMD_WRITE,	0x00},   //CR_OPT_MIN_EYE_VALID
		{0x458CB8,	0x03,	IIC_CMD_WRITE,	0x00},   //CR_OPT_WCNT0_EYE
		{0x058DB8,	0x03,	IIC_CMD_WRITE,	0x00},   //CT_OPT_WCNT1_EYE
		{0x0497B8,	0x03,	IIC_CMD_WRITE,	0x00},   //EQ_OPT_MIN_EYE_VALID
		{0xE098B8,	0x03,	IIC_CMD_WRITE,	0x00},   //EQ_OPT_WCNT0_FEQ
		{0x2E99B8,	0x03,	IIC_CMD_WRITE,	0x00},   //EQ_OPT_WCNT1_FEQ
		{0x000E80,	0x03,	IIC_CMD_WRITE,	0x00},   //TRAINING_AUX_RD_INTERVAL
		{0xA430B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_DIV_CTRL0_1p62gbs_30mhz
		{0x0931B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_DIV_CTRL1_1p62gbs_30mhz
		{0xA435B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_DIV_CTRL0_2p16gbs_30mhz
		{0x0C36B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_DIV_CTRL1_2p16gbs_30mhz
		{0x843FB8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_DIV_CTRL0_2p70gbs_30mhz
		{0x1240B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_DIV_CTRL1_2p70gbs_30mhz
		{0xA044B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_DIV_CTRL0_3p24gbs_30mhz
		{0x0945B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_DIV_CTRL1_3p24gbs_30mhz
		{0xA049B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_DIV_CTRL0_4p32gbs_30mhz
		{0x0C4AB8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_DIV_CTRL1_4p32gbs_30mhz
		{0x8053B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_DIV_CTRL0_5p40gbs_30mhz
		{0x1254B8,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_DIV_CTRL1_5p40gbs_30mhz
		{0x00C5BB,	0x03,	IIC_CMD_WRITE,	0x00},   //OVERRIDE_DATA
		{0x09CBBB,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_FDBK_DIV2_MATCH
		{0xDDC0BB,	0x03,	IIC_CMD_WRITE,	0x00},   //REFCLK_COUNT
		{0xCDC1BB,	0x03,	IIC_CMD_WRITE,	0x00},   //FEEDBK_CLK_COUNT
		{0x70C2BB,	0x03,	IIC_CMD_WRITE,	0x00},   //COUNT_MSB
		{0x04C5BB,	0x03,	IIC_CMD_WRITE,	0x00},   //OVERRIDE_DATA
		{0x0CCBBB,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_FDBK_DIV2_MATCH
		{0xA6C0BB,	0x03,	IIC_CMD_WRITE,	0x00},   //REFCLK_COUNT
		{0xD3C1BB,	0x03,	IIC_CMD_WRITE,	0x00},   //FEEDBK_CLK_COUNT
		{0x70C2BB,	0x03,	IIC_CMD_WRITE,	0x00},   //COUNT_MSB
		{0x0CC5BB,	0x03,	IIC_CMD_WRITE,	0x00},   //OVERRIDE_DATA
		{0x12CBBB,	0x03,	IIC_CMD_WRITE,	0x00},   //PLL_FDBK_DIV2_MATCH
		{0x6EC0BB,	0x03,	IIC_CMD_WRITE,	0x00},   //REFCLK_COUNT
		{0xCDC1BB,	0x03,	IIC_CMD_WRITE,	0x00},   //FEEDBK_CLK_COUNT
		{0x70C2BB,	0x03,	IIC_CMD_WRITE,	0x00},   //COUNT_MSB
		{0x000000071410,	0x06,	IIC_CMD_WRITE,	0x00},   //Boot Ctrl
		//delay
		{1000,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x1810,	0x06,	IIC_CMD_READ,	0x000000071810},   //Boot Status--->value: 00000007
		//Check if 0x1018<bit2:0> is expected value, If not wait until 0x1018<bit2:0> will be expected value
		//Settings for Duplicate mode
		//{0x000000C51800,	0x06,	IIC_CMD_CHECK,	0x00},    //ClockEN
		//{0x000000002000,	0x06,	IIC_CMD_CHECK,	0x00},    //SRAMPG
		////Video Size Related Setting for Non Preset
		//{0x000000A01001,	0x06,	IIC_CMD_CHECK,	0x00},    //HTIM2_LEFT
		//{0x000000A02401,	0x06,	IIC_CMD_CHECK,	0x00},    //HTIM2_RIGHT
		{0x000001404801,	0x06,	IIC_CMD_WRITE,	0x00},   //DPRX_HTIM2
		//{0x000000000401,	0x06,	IIC_CMD_CHECK,	0x00},    //OVERLAP/DUMMY PIXEL FOR LEFT
		//{0x000000001801,	0x06,	IIC_CMD_CHECK,	0x00},    //OVERLAP/DUMMY PIXEL FOR RIGHT
		//{0x0000FF003C01,	0x06,	IIC_CMD_CHECK,	0x00},    //DUMMY COLOR PIXEL
		{0x3E0B03C02029,	0x06,	IIC_CMD_WRITE,	0x00},   //DSI0_PIC_SYN_PKT_A
		//{0x3E0B03C02039,	0x06,	IIC_CMD_CHECK,	0x00},    //DSI1_PIC_SYN_PKT_A
		//eDP Settings for Link Training
		{0x31B6,	0x03,	IIC_CMD_READ,	0x0131B6},   //PL Mainlink Status--->value: 01
		//Check if 0xB631<bit1:0>=01b., If not wait until 0xB631<bit1:0>=01b
		{0x110080,	0x03,	IIC_CMD_WRITE,	0x00},   //DPCD Rev
		{0x060180,	0x03,	IIC_CMD_WRITE,	0x00},   //Max Link Rate
		{0x040280,	0x03,	IIC_CMD_WRITE,	0x00},   //Max Lane Count
		//{0x003080,	0x03,	IIC_CMD_CHECK,	0x00},    //GUID
		//{0x003180,	0x03,	IIC_CMD_CHECK,	0x00},    //GUID
		//{0x003280,	0x03,	IIC_CMD_CHECK,	0x00},    //GUID
		//{0x003380,	0x03,	IIC_CMD_CHECK,	0x00},    //GUID
		//{0x003480,	0x03,	IIC_CMD_CHECK,	0x00},    //GUID
		//{0x003580,	0x03,	IIC_CMD_CHECK,	0x00},    //GUID
		//{0x003680,	0x03,	IIC_CMD_CHECK,	0x00},    //GUID
		//{0x003780,	0x03,	IIC_CMD_CHECK,	0x00},    //GUID
		//{0x003880,	0x03,	IIC_CMD_CHECK,	0x00},    //GUID
		//{0x003980,	0x03,	IIC_CMD_CHECK,	0x00},    //GUID
		//{0x003A80,	0x03,	IIC_CMD_CHECK,	0x00},    //GUID
		//{0x003B80,	0x03,	IIC_CMD_CHECK,	0x00},    //GUID
		//{0x003C80,	0x03,	IIC_CMD_CHECK,	0x00},    //GUID
		//{0x003D80,	0x03,	IIC_CMD_CHECK,	0x00},    //GUID
		//{0x003E80,	0x03,	IIC_CMD_CHECK,	0x00},    //GUID
		//{0x003F80,	0x03,	IIC_CMD_CHECK,	0x00},    //GUID
		//{0x000084,	0x03,	IIC_CMD_CHECK,	0x00},    //IEEE_OUI(Sink)
		//{0x000184,	0x03,	IIC_CMD_CHECK,	0x00},    //IEEE_OUI(Sink)
		//{0x000284,	0x03,	IIC_CMD_CHECK,	0x00},    //IEEE_OUI(Sink)
		//{0x440384,	0x03,	IIC_CMD_CHECK,	0x00},    //Device Identifiler(Sink)
		//{0x500484,	0x03,	IIC_CMD_CHECK,	0x00},    //Device Identifiler(Sink)
		//{0x320584,	0x03,	IIC_CMD_CHECK,	0x00},    //Device Identifiler(Sink)
		//{0x440684,	0x03,	IIC_CMD_CHECK,	0x00},    //Device Identifiler(Sink)
		//{0x530784,	0x03,	IIC_CMD_CHECK,	0x00},    //Device Identifiler(Sink)
		//{0x490884,	0x03,	IIC_CMD_CHECK,	0x00},    //Device Identifiler(Sink)
		//{0x000085,	0x03,	IIC_CMD_CHECK,	0x00},    //IEEE_OUI(Branch)
		//{0x000185,	0x03,	IIC_CMD_CHECK,	0x00},    //IEEE_OUI(Branch)
		//{0x000285,	0x03,	IIC_CMD_CHECK,	0x00},    //IEEE_OUI(Branch)
		//{0x440385,	0x03,	IIC_CMD_CHECK,	0x00},    //Device Identifiler(Branch)
		//{0x500485,	0x03,	IIC_CMD_CHECK,	0x00},    //Device Identifiler(Branch)
		//{0x320585,	0x03,	IIC_CMD_CHECK,	0x00},    //Device Identifiler(Branch)
		//{0x440685,	0x03,	IIC_CMD_CHECK,	0x00},    //Device Identifiler(Branch)
		//{0x530785,	0x03,	IIC_CMD_CHECK,	0x00},    //Device Identifiler(Branch)
		//{0x490885,	0x03,	IIC_CMD_CHECK,	0x00},    //Device Identifiler(Branch)
		{0x0B08B6,	0x03,	IIC_CMD_WRITE,	0x00},   //Set AUXTXHSEN
		{0x1E00B8,	0x03,	IIC_CMD_WRITE,	0x00},   //Set CDR_PHASE_LP_EN
		{0x000087,	0x03,	IIC_CMD_WRITE,	0x00},   //DPCD 0700h
		//For DP EDID Read
		//{0x3001B2,	0x03,	IIC_CMD_CHECK,	0x00},    //
		//{0x000002B2,	0x04,	IIC_CMD_CHECK,	0x00},    //
		//{0x0000000404B2,	0x06,	IIC_CMD_CHECK,	0x00},    //
		//Dummy Color Bar Setting
		//{0x006400320003,	0x06,	IIC_CMD_CHECK,	0x00},    //CBR00_HTIM1
		//{0x006401400403,	0x06,	IIC_CMD_CHECK,	0x00},    //CBR00_HTIM2
		//{0x000400040803,	0x06,	IIC_CMD_CHECK,	0x00},    //CBR00_VTIM1
		//{0x000801E00C03,	0x06,	IIC_CMD_CHECK,	0x00},    //CBR00_VTIM2
		//{0x000000001003,	0x06,	IIC_CMD_CHECK,	0x00},    //CBR00_MODE
		//{0x00A000001403,	0x06,	IIC_CMD_CHECK,	0x00},    //CBR00_COLOR
		//VSYNC monitor output setting for DP connection
		{0x016A00001450,	0x06,	IIC_CMD_WRITE,	0x00},   //Monitor Signal Selection
		{0x000040008C00,	0x06,	IIC_CMD_WRITE,	0x00},   //GPIOOUTMODE
		{0x000000088000,	0x06,	IIC_CMD_WRITE,	0x00},   //GPIOC
		//GPIO setting for HPD (When HPD is controlled by GPIO2)
		//{0x000000000408,	0x06,	IIC_CMD_WRITE,	0x00},   //IOB_CTRL2
		//{0x000000008400,	0x06,	IIC_CMD_WRITE,	0x00},   //GPIOO
		//{0x0000000F8000,	0x06,	IIC_CMD_WRITE,	0x00},   //GPIOC
		//{0x000000048400,	0x06,	IIC_CMD_WRITE,	0x00},   //GPIOO
		//I2C direction control for DP connection
		//{0x00000001B000,	0x06,	IIC_CMD_CHECK,	0x00},    //I2CMD_SL
		//Start Link Training
		//delay
		{5000000,	0x00,	IIC_CMD_DELAY,	0x00}, 
		//RS1_END
		//By detecting VSYNC monitor output on GPIO
		//RS2_START
		//I2C direction control for DP connection
		//{0x00000000B000,	0x06,	IIC_CMD_CHECK,	0x00},    //I2CMD_SL
		//{0x00000000B000,	0x06,	IIC_CMD_CHECK,	0x00},    //I2CMD_SL
		//{0x00000000B000,	0x06,	IIC_CMD_CHECK,	0x00},    //I2CMD_SL
		//{0x00000000B000,	0x06,	IIC_CMD_CHECK,	0x00},    //I2CMD_SL
		//After Link Training finishes
		//Check Link Training Status
		{0x0282,	0x03,	IIC_CMD_READ,	0x770282},   //LANE0_1_STATUS--->value: 77
		//Check if 0x8202 is expected value., If not, Link Training has problem.
		{0x0382,	0x03,	IIC_CMD_READ,	0x770382},   //LANE2_3_STATUS--->value: 77
		//Check if 0x8203 is expected value., If not, Link Training has problem.
		{0x0482,	0x06,	IIC_CMD_READ,	0x81    },   //LANE_ALIGN_STATUS_UPDATED--->value: 81 or 01
		//Check if 0x8204 is expected value., If not, Link Training has problem.
		//Timing Re-Setting
		{0x011D0801,	0x04,	IIC_CMD_WRITE,	0x00},   //VPCTRL_LEFT
		{0x011D1C01,	0x04,	IIC_CMD_WRITE,	0x00},   //VPCTRL_RIGHT
		//{0x00041401,	0x04,	IIC_CMD_CHECK,	0x00},    //VTIM1_LEFT
		//{0x00042801,	0x04,	IIC_CMD_CHECK,	0x00},    //VTIM1_RIGHT
		//{0x00044C01,	0x04,	IIC_CMD_CHECK,	0x00},    //DPRX_VTIIM1
		{0x00085201,	0x04,	IIC_CMD_WRITE,	0x00},   //DPRX_VTIIM2
		{0x000002747020,	0x06,	IIC_CMD_WRITE,	0x00},   //DSI_APF_VDELAYCNT
		{0x00208C20,	0x04,	IIC_CMD_WRITE,	0x00},   //DSI_HSYNC_WIDTH
		{0x00C89020,	0x04,	IIC_CMD_WRITE,	0x00},   //DSI_HBPR
		//DSI Transition Time Setting for 26MHz REFCLK
		{0x00000A282020,	0x06,	IIC_CMD_WRITE,	0x00},   //Set_LINE_INIT_COUNT
		{0x0000001A7021,	0x06,	IIC_CMD_WRITE,	0x00},   //Set_DSI_HSTXVREGCNT
		//DSI Transition Time Setting for Non Preset
		{0x000000015421,	0x06,	IIC_CMD_WRITE,	0x00},   //PPI_DPHY_LPTXTIMECNT
		{0x000500005821,	0x06,	IIC_CMD_WRITE,	0x00},   //PPI_DPHY_TCLK_HEADERCNT
		{0x000200005C21,	0x06,	IIC_CMD_WRITE,	0x00},   //PPI_DPHY_TCLK_TRAILCNT
		{0x000000016021,	0x06,	IIC_CMD_WRITE,	0x00},   //PPI_DPHY_THS_HEADERCNT
		{0x000027106421,	0x06,	IIC_CMD_WRITE,	0x00},   //PPI_DPHY_TWAKEUPCNT
		{0x000000066821,	0x06,	IIC_CMD_WRITE,	0x00},   //PPI_DPHY_TCLK_POSTCNT
		{0x000200026C21,	0x06,	IIC_CMD_WRITE,	0x00},   //PPI_DPHY_THSTRAILCNT
		{0x000100017821,	0x06,	IIC_CMD_WRITE,	0x00},   //PPI_DSI_BTA_COUNT
		//{0x000000004021,	0x06,	IIC_CMD_CHECK,	0x00},    //PPI_DPHY_DLYCNTRL
		//{0x000000008821,	0x06,	IIC_CMD_CHECK,	0x00},    //PPI_DPHY_CAP
		////DSI Start
		//{0x760123458802,	0x06,	IIC_CMD_CHECK,	0x00},    //Set CMP0_POST_SWAP
		//{0x760123459802,	0x06,	IIC_CMD_CHECK,	0x00},    //Set CMP1_POST_SWAP
		//{0x0F0001000001,	0x06,	IIC_CMD_CHECK,	0x00},    //VIDEOMODE
		{0x000000817C20,	0x06,	IIC_CMD_WRITE,	0x00},   //DSI_DSITX_MODE
		{0x000000005020,	0x06,	IIC_CMD_WRITE,	0x00},   //DSI_FUNC_MODE
		{0x000000011C20,	0x06,	IIC_CMD_WRITE,	0x00},   //DSI_DSITX_START
		//delay
		{100,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x6020,	0x06,	IIC_CMD_READ,	0x000000036020},   //DSITX_INIT_INT_STAT--->value: 00000003
		//Check if 0x2060/4060<bit1:0>=11b., If not wait until 0x2060/4060<bit1:0>=11b.
		//GPIO setting for LCD control.  (Depends on LCD specification and System configuration)
		{0x0000000C8400,	0x06,	IIC_CMD_WRITE,	0x00},   //GPIOO
		{0x000000048400,	0x06,	IIC_CMD_WRITE,	0x00},   //GPIOO
		//delay
		{5000,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x0000000C8400,	0x06,	IIC_CMD_WRITE,	0x00},   //GPIOO
		//DSI Hs Clock Mode
		{0x000000205020,	0x06,	IIC_CMD_WRITE,	0x00},   //DSI_FUNC_MODE
		{0x00000000102A,	0x06,	IIC_CMD_WRITE,	0x00},   //DSI0_CQMODE
		{0x00000000103A,	0x06,	IIC_CMD_WRITE,	0x00},   //DSI1_CQMODE
		//Command Transmission Before Video Start. (Depeds on LCD specification)
		//LCD Initialization
		//Software Reset
		{0x81000105FC22,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_HEADER
		//delay
		{200,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x0022,	0x06,	IIC_CMD_READ,	0x000000010022},   //DSIG_CQ_STATUS--->value: 00000001
		//Check if <bit0>=1, If not, wait until <bit0>=1
		//Set Pixel Format
		{0x81773A15FC22,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_HEADER
		//delay
		{200,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x0022,	0x06,	IIC_CMD_READ,	0x000000010022},   //DSIG_CQ_STATUS--->value: 00000001
		//Check if <bit0>=1, If not, wait until <bit0>=1
		//Set Column Address
		{0x83000539FC22,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_HEADER
		{0x0400002A0023,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_PAYLOAD
		{0x000000FF0423,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_PAYLOAD_mirror
		//delay
		{200,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x0022,	0x06,	IIC_CMD_READ,	0x000000010022},   //DSIG_CQ_STATUS--->value: 00000001
		//Check if <bit0>=1, If not, wait until <bit0>=1
		//Set Page Address
		{0x83000539FC22,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_HEADER
		{0x0600002B0023,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_PAYLOAD
		{0x0000003F0423,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_PAYLOAD_mirror
		//delay
		{200,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x0022,	0x06,	IIC_CMD_READ,	0x000000010022},   //DSIG_CQ_STATUS--->value: 00000001
		//Check if <bit0>=1, If not, wait until <bit0>=1
		//Set Tear On
		{0x81003515FC22,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_HEADER
		//delay
		{200,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x0022,	0x06,	IIC_CMD_READ,	0x000000010022},   //DSIG_CQ_STATUS--->value: 00000001
		//Check if <bit0>=1, If not, wait until <bit0>=1
		//Set Tear Scanline
		{0x83000339FC22,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_HEADER
		{0x000000440023,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_PAYLOAD
		//delay
		{200,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x0022,	0x06,	IIC_CMD_READ,	0x000000010022},   //DSIG_CQ_STATUS--->value: 00000001
		//Check if <bit0>=1, If not, wait until <bit0>=1
		//Write Display Brightness
		{0x81FF5115FC22,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_HEADER
		//delay
		{200,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x0022,	0x06,	IIC_CMD_READ,	0x000000010022},   //DSIG_CQ_STATUS--->value: 00000001
		//Check if <bit0>=1, If not, wait until <bit0>=1
		//Write Control Display
		{0x81245315FC22,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_HEADER
		//delay
		{200,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x0022,	0x06,	IIC_CMD_READ,	0x000000010022},   //DSIG_CQ_STATUS--->value: 00000001
		//Check if <bit0>=1, If not, wait until <bit0>=1
		//Adaptive Brightness Control
		{0x81015515FC22,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_HEADER
		//delay
		{200,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x0022,	0x06,	IIC_CMD_READ,	0x000000010022},   //DSIG_CQ_STATUS--->value: 00000001
		//Check if <bit0>=1, If not, wait until <bit0>=1
		//Exit Sleep
		{0x81001105FC22,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_HEADER
		//delay
		{200,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x0022,	0x06,	IIC_CMD_READ,	0x000000010022},   //DSIG_CQ_STATUS--->value: 00000001
		//Check if <bit0>=1, If not, wait until <bit0>=1
		//delay
		{120000,	0x00,	IIC_CMD_DELAY,	0x00}, 
		//MCAP
		{0x8100B023FC22,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_HEADER
		//delay
		{200,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x0022,	0x06,	IIC_CMD_READ,	0x000000010022},   //DSIG_CQ_STATUS--->value: 00000001
		//Check if <bit0>=1, If not, wait until <bit0>=1
		//Backlight Control 4
		{0x83001429FC22,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_HEADER
		{0x48407DCE0023,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_PAYLOAD
		{0x887867560423,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_PAYLOAD_mirror
		{0xC3B5A7980823,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_PAYLOAD_mirror
		{0xF2E9DED10C23,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_PAYLOAD_mirror
		{0x0004FFFA1023,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_PAYLOAD_mirror
		//delay
		{200,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x0022,	0x06,	IIC_CMD_READ,	0x000000010022},   //DSIG_CQ_STATUS--->value: 00000001
		//Check if <bit0>=1, If not, wait until <bit0>=1
		{0x000000205020,	0x06,	IIC_CMD_WRITE,	0x00},   //DSI_FUNC_MODE
		{0x80040010102A,	0x06,	IIC_CMD_WRITE,	0x00},   //DSI0_CQMODE
		//{0x80040010103A,	0x06,	IIC_CMD_CHECK,	0x00},    //DSI1_CQMODE
		{0x00000001042A,	0x06,	IIC_CMD_WRITE,	0x00},   //DSI0_VideoSTART
		//{0x00000001043A,	0x06,	IIC_CMD_CHECK,	0x00},    //DSI1_VideoSTART
		//Check if eDP video is coming
		{0x000000015401,	0x06,	IIC_CMD_WRITE,	0x00},   //Set_DPVideoEn
		//Command Transmission After Video Start. (Depends on LCD specification)
		//delay
		{35000,	0x00,	IIC_CMD_DELAY,	0x00}, 
		//MCAP
		{0x8000B023FC22,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_HEADER
		//delay
		{35000,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x0022,	0x06,	IIC_CMD_READ,	0x000000010022},   //DSIG_CQ_STATUS--->value: 00000001
		//Check if <bit0>=1, If not, wait until <bit0>=1
		//Interface Setting
		{0x8014B323FC22,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_HEADER
		//delay
		{35000,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x0022,	0x06,	IIC_CMD_READ,	0x000000010022},   //DSIG_CQ_STATUS--->value: 00000001
		//Check if <bit0>=1, If not, wait until <bit0>=1
		//MCAP
		{0x8003B023FC22,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_HEADER
		//delay
		{35000,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x0022,	0x06,	IIC_CMD_READ,	0x000000010022},   //DSIG_CQ_STATUS--->value: 00000001
		//Check if <bit0>=1, If not, wait until <bit0>=1
		//Set Display On
		{0x80002905FC22,	0x06,	IIC_CMD_WRITE,	0x00},   //DSIG_CQ_HEADER
		//delay
		{35000,	0x00,	IIC_CMD_DELAY,	0x00}, 
		{0x0022,	0x06,	IIC_CMD_READ,	0x000000010022},   //DSIG_CQ_STATUS--->value: 00000001
		//Check if <bit0>=1, If not, wait until <bit0>=1
		//RS2_END
		//end of list 
};

static s32 tc3588_i2c_read(struct i2c_client *client, s32 len, u8 *buf);
static s32 tc3588_i2c_write(struct i2c_client *client, s32 len, u8 *buf);
static int tc3588_configure(struct i2c_client *client);




#endif
