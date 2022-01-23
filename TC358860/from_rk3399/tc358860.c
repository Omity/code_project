#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/fb.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_graph.h>
#include <linux/pm.h>
#include <linux/regulator/consumer.h>
#include <drm/drm_panel.h>
#include <drm/drm_of.h>
#include <linux/regmap.h>

#include "drmP.h"
#include "drm_crtc.h"
#include "drm_crtc_helper.h"
#include "drm_atomic_helper.h"

//IO Voltage Setting
#define IOB_CTRL1           	0x0800
//Boot Settings
#define WAIT0_CNT           	0x1000
#define BOOT_SET0           	0x1004
#define BOOT_SET1           	0x1008
#define BOOT_SET2           	0x100c
#define BOOT_SET3           	0x1010
#define BOOT_CTRL           	0x1014
#define BOOT_STATUS         	0x1018
//Setting for 26MHz REFCLK
#define EXTCLKSEL           	0x0000
#define DPREFPLLCTRL_1      	0x0008
#define DPREFPLLCTRL_2      	0x000c
//Internal PCLK Setting for Non Preset or REFCLK=26MHz
#define CG_VIDPLL_CTRL1     	0xB005
#define CG_VIDPLL_CTRL2     	0xB006
#define CG_VIDPLL_CTRL3     	0xB007
#define CG_VIDPLL_CTRL4     	0xB008
#define CG_VIDPLL_CTRL5     	0xB009
#define CG_VIDPLL_CTRL6     	0xB00A
//DSI Clock setting for Non Preset or REFCLK=26MHz
#define MIPI_PLL_CNF        	0x41B0
#define MIPI_PLL_PARA       	0x41BC
#define MIPI_PLL_FIX        	0x41C0
#define DSI_PLL_LOCKCNT     	0x41A8
//Additional Setting for eDP
#define MAX_DOWNSPREAD      	0x8003
#define AL_MODE_CTRL_LINK   	0xB400
#define PL_MAINLINK_REFCLK_CTRL	0xB632
#define FULLTRANTIME0			0xB401
#define FULLTRANTIME1			0xB402
#define FULLTRANTIME2			0xB403
#define FASTTRNTIME0			0xB404
#define FASTTRNTIME1			0xB405
#define FASTTRNTIME2			0xB406
#define AL_LOCKTIME_CTRL0		0xB409
#define AL_LOCKTIME_CTRL1		0xB40A
#define AL_LOCKTIME_CTRL2		0xB40B
#define AL_OFFDLY_CTRL0			0xB420
#define AL_OFFDLY_CTRL1			0xB421
#define AL_OFFDLY_CTRL2			0xB422
#define AL_OFFDLY_CTRL3			0xB423
#define AL_OFFDLY_CTRL4			0xB424
#define AL_OFFDLY_CTRL5			0xB425
#define AL_OFFDLY_CTRL6			0xB426
#define AL_OFFDLY_CTRL7			0xB427
#define AL_SLEEPDLY_CTRL0		0xB428
#define AL_SLEEPDLY_CTRL1		0xB429
#define AL_SLEEPDLY_CTRL2		0xB42A
#define AL_SLEEPDLY_CTRL3		0xB42B
#define AL_SLEEPDLY_CTRL4		0xB42C
#define AL_SLEEPDLY_CTRL5		0xB42D
#define AL_SLEEPDLY_CTRL6		0xB42E
#define AL_SLEEPDLY_CTRL7		0xB42F
//
#define SPTPIX_LEFT         0x0104
#define SPTPIX_RIGHT        0x0118
#define DPRX_HTIM1          0x0146
#define DSIG_BLANKPKTEN     0x407c
#define CMP0_POST_SWAP      0x0288
#define CMP1_POST_SWAP      0x0298

//DPRX CAD Register Setting
#define CR_OPT_WCNT_0       	0xB88E
#define CR_OPT_WCNT_1       	0xB88F
#define CR_OPT_WCNT_2       	0xB89A
#define CR_OPT_WCNT_3       	0xB89B
#define CDR_PHASE_LP_EN     	0xB800
#define RX_VREG_VALUE       	0xBB26
#define RX_VREG_ENABLE      	0xBB01
#define RX_CDR_LUT1         	0xB8C0
#define RX_CDR_LUT2         	0xB8C1
#define RX_CDR_LUT3         	0xB8C2
#define RX_CDR_LUT4         	0xB8C3
#define RX_CDR_LUT5         	0xB8C4
#define RX_CDR_LUT6         	0xB8C5
#define RX_CDR_LUT7         	0xB8C6
#define RX_CDR_LUT8         	0xB8C7
#define PLL_CP1P1           	0xB80B
#define PLL_CP1P2           	0xB833
#define PLL_CP1P3           	0xB85B
#define PLL_CP2P1           	0xB810
#define PLL_CP2P2           	0xB838
#define PLL_CP2P3           	0xB860
#define PLL_CP2P4           	0xB815
#define PLL_CP2P5           	0xB83D
#define PLL_CP2P6           	0xB865
#define PLL_CP2P7           	0xB81A
#define PLL_CP2P8           	0xB842
#define PLL_CP2P9           	0xB86A
#define PLL_CP3P1           	0xB81F
#define PLL_CP3P2           	0xB847
#define PLL_CP3P3           	0xB86F
#define PLL_CP4P1           	0xB824
#define PLL_CP4P2           	0xB84C
#define PLL_CP4P3           	0xB874
#define PLL_CP4P4           	0xB829
#define PLL_CP4P5           	0xB851
#define PLL_CP4P6           	0xB879
#define PLL_CP5P7           	0xB82E
#define PLL_CP5P2           	0xB856
#define PLL_CP5P3           	0xB87E

#define CTLE_EM_DATA_RATE_CONTROL_0     0xBB90
#define CTLE_EM_DATA_RATE_CONTROL_1     0xBB91
#define CTLE_EM_DATA_RATE_CONTROL_2     0xBB92
#define CTLE_EM_DATA_RATE_CONTROL_3     0xBB93
#define CTLE_EM_DATA_RATE_CONTROL_4     0xBB94
#define CTLE_EM_DATA_RATE_CONTROL_5     0xBB95
#define CTLE_EM_DATA_RATE_CONTROL_6     0xBB96
#define CTLE_EM_DATA_RATE_CONTROL_7     0xBB97
#define CTLE_EM_DATA_RATE_CONTROL_8     0xBB98
#define CTLE_EM_DATA_RATE_CONTROL_9     0xBB99
#define CTLE_EM_DATA_RATE_CONTROL_A     0xBB9a
#define CTLE_EM_DATA_RATE_CONTROL_B     0xBB9b

#define CR_OPT_CTRL             		0xB88A
#define EQ_OPT_CTRL             		0xB896
#define CTLE_EM_CONTRO_1        		0xBBD1
#define EYE_CONFIGURATION_0     		0xBBB0
#define CR_OPT_MIN_EYE_VALID    		0xB88B
#define CR_OPT_WCNT0_EYE        		0xB88C
#define CT_OPT_WCNT1_EYE        		0xB88D
#define EQ_OPT_MIN_EYE_VALID    		0xB897
#define EQ_OPT_WCNT0_FEQ        		0xB898
#define EQ_OPT_WCNT1_FEQ        		0xB899
#define TRAINING_AUX_RD_INTERVAL        0x800E
#define PLL_DIV_CTRL0_1P62GBS_30MHZ		0xB830
#define PLL_DIV_CTRL1_1P62GBS_30MHZ		0xB831
#define PLL_DIV_CTRL0_2P16GBS_30MHZ		0xB835
#define PLL_DIV_CTRL1_2P16GBS_30MHZ		0xB836
#define PLL_DIV_CTRL0_2P70GBS_30MHZ		0xB83F
#define PLL_DIV_CTRL1_2P70GBS_30MHZ		0xB840
#define PLL_DIV_CTRL0_3P24GBS_30MHZ		0xB844
#define PLL_DIV_CTRL1_3P24GBS_30MHZ		0xB845
#define PLL_DIV_CTRL0_4P32GBS_30MHZ		0xB849
#define PLL_DIV_CTRL1_4P32GBS_30MHZ		0xB84A
#define PLL_DIV_CTRL0_5P40GBS_30MHZ		0xB853
#define PLL_DIV_CTRL1_5P40GBS_30MHZ		0xB854
#define OVERRIDE_DATA					0xBBC5
#define PLL_FDBK_DIV2_MATCH				0xBBCB
#define REFCLK_COUNT					0xBBC0
#define FEEDBK_CLK_COUNT				0xBBC1
#define COUNT_MSB						0xBBC2



#define DPRX_MAINLINK_STATUS            0xB631
#define LANE0_1_STATUS             		0x8202
#define LANE2_3_STATUS	                0x8203
#define ALIGN_STATUS            		0x8204
#define DPCD_REV                		0x8000
#define MAX_LINK_RATE           		0x8001
#define MAX_LANE_COUNT          		0x8002
#define AUXTXHSEN               		0xb608
//#define CDR_PHASE_LP_EN         		0xb800
#define DPCD                    		0x8700
#define GPIO_RESET0             		0x0804
#define GPIO_RESET1             		0x0080
#define GPIO_RESET2             		0x0080
#define GPIO_RESET3             		0x0084
#define GPIO3_INTERRUPT1        		0x008c
#define GPIO3_INTERRUPT2        		0x5014

//RS2 START
#define VPCTRL_LEFT_0           		0x0108
#define VPCTRL_RIGHT_0          		0x011c
#define DPRX_VTIM2_2            		0x0152
#define APF_VDELAYCNT_0         		0x2070
#define DSI_HSYNC_WIDTH_0       		0x208c
#define DSI_HBPR_0              		0x2090
//DSI Transition Time Setting for 26MHz REFCLK
#define LINE_INIT_COUNT                 0x2020
#define DSI_HSTXVREGCNT                 0x2170
//DSI Transition Time Setting for Non Preset
#define PPI_DPHY_LPTXTIMECNT_0  		0x2154
#define PPI_DPHY_TCLK_HEADERCNT_0   	0x2158
#define PPI_DPHY_TCLK_TRAILCNT_0    	0x215c
#define PPI_DPHY_THS_HEADERCNT_0    	0x2160
#define PPI_DPHY_TWAKEUPCNT_0       	0x2164
#define PPI_DPHY_TCLK_POSTCNT_0     	0x2168
#define PPI_DPHY_THSTRAILCNT_0      	0x216C
#define PPI_DSI_BTA_COUNT_0         	0x2178
//DSI START
#define DSITX_MODE_0                	0x207c
#define FUNC_MODE_0                 	0x2050
#define DSI_PRTO_INT_MASK_0         	0x201C
#define DSITX_INIT_INT_STAT             0x2060
//DSI Hs Clock Mode
#define DSI0_CQMODE                 	0x2a10
#define DSI1_CQMODE                 	0x3a10
//LCD Initialization
#define CQ_HEADER                   	0x22fc
#define CQ_PAYLOAD                  	0x2300
#define CQ_PAYLOAD_MIRROR_0             0x2304
#define CQ_PAYLOAD_MIRROR_1             0x2308
#define CQ_PAYLOAD_MIRROR_2             0x230c
#define CQ_PAYLOAD_MIRROR_3             0x2310
#define CQ_STATUS                   	0x2200
#define DSITX_FUNC_MODE             	0x4050
#define DSI0_VIDEO_START             	0x2a04
#define DSI1_VIDEO_START             	0x3a04
#define DPVIDEO_EN                  	0x0154


struct tc358860_bridge {
    struct drm_connector connector;
    struct i2c_client *client;
    struct drm_bridge bridge;
    struct drm_panel *panel;
    struct gpio_desc * gpio_rst;
    struct gpio_desc * gpio_enable;
    struct device *dev;
    bool dual_channel;
    struct regmap *regmap;
	
    u32 bus_format;
    u32 max_lane_count;
    u32 lane_count;
    bool pre_enabled;
    bool enabled;

    struct backlight_device *bl;
};

static const struct regmap_config tc358860_regmap_config = {                                                                                                                   
    .name = "tc358860",
    .reg_bits = 16,
    .val_bits = 32,
    .max_register = 0xBFFF,
};

static struct tc358860_bridge *connector_to_tc358860(struct drm_connector *connector)
{
    return container_of(connector, struct tc358860_bridge, connector);
}

static int tc358860_bridge_get_modes(struct drm_connector *connector)
{
    struct tc358860_bridge *tc358860 = connector_to_tc358860(connector);
    int num_modes = 0;
    
    num_modes = drm_panel_get_modes(tc358860->panel);

    
    return num_modes;
}

static void tc358860_io_voltage_setting(struct tc358860 *tc358860)
{
    /* IO Voltage setting.
     * two voltage level for VDDIO, 1.8V and 3.3V.
     * default is 3.3V, only when uses 1.8V for VDDIO
     * voltage,necessary to change the register setting.
     */
    regmap_write(tc358860->regmap,(IOB_CTRL1,0x00000001));

}

static void tc358860_boot_sequence(struct tc358860 *tc358860)
{
    u8 status;
    
    regmap_write(tc358860->regmap,WAIT0_CNT,0x00006590); // BootWaitCount
    regmap_write(tc358860->regmap,BOOT_SET0,0x00040408); // Boot Set0
    regmap_write(tc358860->regmap,BOOT_SET1,0x011D0008); // Boot Set1
    regmap_write(tc358860->regmap,BOOT_SET2,0x03000274); // Boot Set2
    regmap_write(tc358860->regmap,BOOT_SET3,0x00C80020); // Boot Set3
    regmap_write(tc358860->regmap,BOOT_CTRL,0x00000001); // Boot Ctrl
    mdelay(1);

    do {
	        regmap_read(tc358860->regmap,BOOT_STATUS,&status);
        }while(status!=0x00000002); // Check if 0x1018<bit2:0> is expected value
}

static void tc358860_refclk_setting(struct tc358860 *tc358860)
{
	u8 status;
	
	/* Setting for 26MHz REFCLK */
	regmap_write(tc358860->regmap, EXTCLKSEL, 0x41);   //EXTCLKSEL
	regmap_write(tc358860->regmap, DPREFPLLCTRL_1, 0x0302); //DPREFPLLCTRL_1
	regmap_write(tc358860->regmap, DPREFPLLCTRL_2, 0x0c02301D);
	/*Internal PCLK Setting for Non Preset or REFCLK=26MHz */
	regmap_write(tc358860->regmap, CG_VIDPLL_CTRL1, 0x3a);
	regmap_write(tc358860->regmap, CG_VIDPLL_CTRL2, 0x04);
	regmap_write(tc358860->regmap, CG_VIDPLL_CTRL3, 0x2f);
	regmap_write(tc358860->regmap, CG_VIDPLL_CTRL4, 0x00);
	regmap_write(tc358860->regmap, CG_VIDPLL_CTRL5, 0x21);
	regmap_write(tc358860->regmap, CG_VIDPLL_CTRL6, 0x0e);
	/* DSI Clock setting for Non Preset or REFCLK=26MHz */
	regmap_write(tc358860->regmap, MIPI_PLL_CNF, 0x0003262D);
	regmap_write(tc358860->regmap, MIPI_PLL_PARA, 0x00001B51);
	regmap_write(tc358860->regmap, MIPI_PLL_FIX, 0x00000030);
	regmap_write(tc358860->regmap, DSI_PLL_LOCKCNT, 0x0000963D);
	regmap_write(tc358860->regmap, BOOT_CTRL, 0x00000003);
	mdelay(1);
	
	do {
		regmap_read(tc358860->regmap, BOOT_STATUS, &status);
	}while(status != 0x00000006); //Check if 0x1018<bit2:0> is expected value

}

static void tc358860_dprx_cad_setting(struct tc358860 *tc358860)
{
    u8 status;

    /* Additional Setting for eDP */
    regmap_write(tc358860->regmap,MAX_DOWNSPREAD,0x41); // Max Downspread
    regmap_write(tc358860->regmap,DPRX_AL_MODE_CTRL,0x0d); // AL Mode Control Link
    regmap_write(tc358860->regmap, PL_Mainlink_REFCLK_Ctrl, 0x12); //PL Mainlink REFCLK Ctrl
	regmap_write(tc358860->regmap, FULLTRANTIME0, 0xA0); //FULLTRANTIME0
	regmap_write(tc358860->regmap, FULLTRANTIME1, 0xF7); //FULLTRANTIME1
	regmap_write(tc358860->regmap, FULLTRANTIME2, 0x03); //FULLTRANTIME2
	regmap_write(tc358860->regmap, FASTTRNTIME0, 0x90); //FASTTRNTIME0
	regmap_write(tc358860->regmap, FASTTRNTIME1, 0x65); //FASTTRNTIME1
	regmap_write(tc358860->regmap, FASTTRNTIME2, 0x00); //FASTTRNTIME2
	regmap_write(tc358860->regmap, AL_LOCKTIME_CTRL0, 0x0D); //AL_LOCKTIME_CTRL0
	regmap_write(tc358860->regmap, AL_LOCKTIME_CTRL1, 0x08); //AL_LOCKTIME_CTRL1
	regmap_write(tc358860->regmap, AL_LOCKTIME_CTRL2, 0x02); //AL_LOCKTIME_CTRL2
	regmap_write(tc358860->regmap, AL_OFFDLY_CTRL0, 0x14); //AL_OFFDLY_CTRL0
	regmap_write(tc358860->regmap, AL_OFFDLY_CTRL1, 0x05); //AL_OFFDLY_CTRL1
	regmap_write(tc358860->regmap, AL_OFFDLY_CTRL2, 0x28); //AL_OFFDLY_CTRL2
	regmap_write(tc358860->regmap, AL_OFFDLY_CTRL3, 0x0A); //AL_OFFDLY_CTRL3
	regmap_write(tc358860->regmap, AL_OFFDLY_CTRL4, 0x3C); //AL_OFFDLY_CTRL4
	regmap_write(tc358860->regmap, AL_OFFDLY_CTRL5, 0x0F); //AL_OFFDLY_CTRL5
	regmap_write(tc358860->regmap, AL_OFFDLY_CTRL6, 0x50); //AL_OFFDLY_CTRL6
	regmap_write(tc358860->regmap, AL_OFFDLY_CTRL7, 0x14); //AL_OFFDLY_CTRL7
	regmap_write(tc358860->regmap, AL_SLEEPDLY_CTRL0, 0x28); //AL_SLEEPDLY_CTRL0
	regmap_write(tc358860->regmap, AL_SLEEPDLY_CTRL1, 0x0A); //AL_SLEEPDLY_CTRL1
	regmap_write(tc358860->regmap, AL_SLEEPDLY_CTRL2, 0x50); //AL_SLEEPDLY_CTRL2
	regmap_write(tc358860->regmap, AL_SLEEPDLY_CTRL3, 0x14); //AL_SLEEPDLY_CTRL3
	regmap_write(tc358860->regmap, AL_SLEEPDLY_CTRL4, 0x78); //AL_SLEEPDLY_CTRL4
	regmap_write(tc358860->regmap, AL_SLEEPDLY_CTRL5, 0x1E); //AL_SLEEPDLY_CTRL5
	regmap_write(tc358860->regmap, AL_SLEEPDLY_CTRL6, 0xA0); //AL_SLEEPDLY_CTRL6
	regmap_write(tc358860->regmap, AL_SLEEPDLY_CTRL7, 0x28); //AL_SLEEPDLY_CTRL7

    /* DPRX CAD Register setting */
    regmap_write(tc358860->regmap,CR_OPT_WCNT_0,0xFF); // Set CR_OPT_WCNT0
    regmap_write(tc358860->regmap,CR_OPT_WCNT_1,0xFF); // Set CR_OPT_WCNT1
    regmap_write(tc358860->regmap,CR_OPT_WCNT_2,0xFF); // Set CR_OPT_WCNT2
    regmap_write(tc358860->regmap,CR_OPT_WCNT_3,0xFF); // Set CR_OPT_WCNT3
    regmap_write(tc358860->regmap, CDR_PHASE_LP_EN,0x0E); // Set CDR_PHASE_LP_EN
    regmap_write(tc358860->regmap,RX_VREG_VALUE,0x02); // RX_VREG_VALUE
    regmap_write(tc358860->regmap,RX_VREG_ENABLE,0x20); // RX_VREG_ENABLE
    regmap_write(tc358860->regmap,RX_CDR_LUT1,0xF1); // RX_CDR_LUT1
    regmap_write(tc358860->regmap,RX_CDR_LUT2,0xF1); // RX_CDR_LUT2
    regmap_write(tc358860->regmap,RX_CDR_LUT3,0xF0); // RX_CDR_LUT3
    regmap_write(tc358860->regmap,RX_CDR_LUT4,0xF0); // RX_CDR_LUT4
    regmap_write(tc358860->regmap,RX_CDR_LUT5,0xF0); // RX_CDR_LUT5
    regmap_write(tc358860->regmap,RX_CDR_LUT6,0xF0); // RX_CDR_LUT6
    regmap_write(tc358860->regmap,RX_CDR_LUT7,0xF0); // RX_CDR_LUT7
    regmap_write(tc358860->regmap,RX_CDR_LUT8,0xF0); // RX_CDR_LUT8
    regmap_write(tc358860->regmap,PLL_CP1P1,0x00); // PLL_CP1P1
    regmap_write(tc358860->regmap,PLL_CP1P2,0x00); // PLL_CP1P2
    regmap_write(tc358860->regmap,PLL_CP1P3,0x00); // PLL_CP1P3
    regmap_write(tc358860->regmap,PLL_CP2P1,0x00); // PLL_CP2P1
    regmap_write(tc358860->regmap,PLL_CP2P2,0x00); // PLL_CP2P2
    regmap_write(tc358860->regmap,PLL_CP2P3,0x00); // PLL_CP2P3
    regmap_write(tc358860->regmap,PLL_CP2P4,0x00); // PLL_CP2P4
    regmap_write(tc358860->regmap,PLL_CP2P5,0x00); // PLL_CP2P5
    regmap_write(tc358860->regmap,PLL_CP2P6,0x00); // PLL_CP2P6
    regmap_write(tc358860->regmap,PLL_CP2P7,0x00); // PLL_CP2P7
    regmap_write(tc358860->regmap,PLL_CP2P8,0x00); // PLL_CP2P8
    regmap_write(tc358860->regmap,PLL_CP2P9,0x00); // PLL_CP2P9
    regmap_write(tc358860->regmap,PLL_CP3P1,0x00); // PLL_CP3P1
    regmap_write(tc358860->regmap,PLL_CP3P2,0x00); // PLL_CP3P2
    regmap_write(tc358860->regmap,PLL_CP3P3,0x00); // PLL_CP3P3
    regmap_write(tc358860->regmap,PLL_CP4P1,0x00); // PLL_CP4P1
    regmap_write(tc358860->regmap,PLL_CP4P2,0x00); // PLL_CP4P2
    regmap_write(tc358860->regmap,PLL_CP4P3,0x00); // PLL_CP4P3
    regmap_write(tc358860->regmap,PLL_CP4P4,0x00); // PLL_CP4P4
    regmap_write(tc358860->regmap,PLL_CP4P5,0x00); // PLL_CP4P5
    regmap_write(tc358860->regmap,PLL_CP4P6,0x00); // PLL_CP4P6
    regmap_write(tc358860->regmap,PLL_CP5P7,0x00); // PLL_CP5P7
    regmap_write(tc358860->regmap,PLL_CP5P2,0x00); // PLL_CP5P2
    regmap_write(tc358860->regmap,PLL_CP5P3,0x00); // PLL_CP5P3
    regmap_write(tc358860->regmap,CTLE_EM_DATA_RATE_CONTROL_0,0x10); // ctle_em_data_rate_control_0[7:0]
    regmap_write(tc358860->regmap,CTLE_EM_DATA_RATE_CONTROL_1,0x0F); // ctle_em_data_rate_control_1[7:0]
    regmap_write(tc358860->regmap,CTLE_EM_DATA_RATE_CONTROL_2,0xF6); // ctle_em_data_rate_control_2[7:0]
    regmap_write(tc358860->regmap,CTLE_EM_DATA_RATE_CONTROL_3,0x10); // ctle_em_data_rate_control_3[7:0]
    regmap_write(tc358860->regmap,CTLE_EM_DATA_RATE_CONTROL_4,0x0F); // ctle_em_data_rate_control_4[7:0]
    regmap_write(tc358860->regmap,CTLE_EM_DATA_RATE_CONTROL_5,0xF6); // ctle_em_data_rate_control_5[7:0]
    regmap_write(tc358860->regmap,CTLE_EM_DATA_RATE_CONTROL_6,0x10); // ctle_em_data_rate_control_6[7:0]
    regmap_write(tc358860->regmap,CTLE_EM_DATA_RATE_CONTROL_7,0x0F); // ctle_em_data_rate_control_7[7:0]
    regmap_write(tc358860->regmap,CTLE_EM_DATA_RATE_CONTROL_8,0xF6); // ctle_em_data_rate_control_8[7:0]
    regmap_write(tc358860->regmap,CTLE_EM_DATA_RATE_CONTROL_9,0x10); // ctle_em_data_rate_control_A[7:0]
    regmap_write(tc358860->regmap,CTLE_EM_DATA_RATE_CONTROL_A,0x0F); // ctle_em_data_rate_control_B[7:0]
    regmap_write(tc358860->regmap,CTLE_EM_DATA_RATE_CONTROL_B,0xF6); // ctle_em_data_rate_control_0[7:0]
    regmap_write(tc358860->regmap,CR_OPT_CTRL,0x03); // CR_OPT_CTRL
    regmap_write(tc358860->regmap,EQ_OPT_CTRL,0x03); // EQ_OPT_CTRL
    regmap_write(tc358860->regmap,CTLE_EM_CONTRO_1,0x07); // ctle_em_contro_1
    regmap_write(tc358860->regmap,EYE_CONFIGURATION_0,0x07); // eye_configuration_0
    regmap_write(tc358860->regmap,CR_OPT_MIN_EYE_VALID,0x04); // CR_OPT_MIN_EYE_VALID
    regmap_write(tc358860->regmap,CR_OPT_WCNT0_EYE,0x45); // CR_OPT_WCNT0_EYE
    regmap_write(tc358860->regmap,CT_OPT_WCNT1_EYE,0x05); // CT_OPT_WCNT1_EYE
    regmap_write(tc358860->regmap,EQ_OPT_MIN_EYE_VALID,0x04); // EQ_OPT_MIN_EYE_VALID
    regmap_write(tc358860->regmap,EQ_OPT_WCNT0_FEQ,0xE0); // EQ_OPT_WCNT0_FEQ
    regmap_write(tc358860->regmap,EQ_OPT_WCNT1_FEQ,0x2E); // EQ_OPT_WCNT1_FEQ
    regmap_write(tc358860->regmap,TRAINING_AUX_RD_INTERVAL,0x00); // TRAINING_AUX_RD_INTERVAL
    //add by sn03955
    regmap_write(tc358860->regmap, PLL_DIV_CTRL0_1P62GBS_30MHZ, 0xA4); //PLL_DIV_CTRL0_1p62gbs_30mhz
	regmap_write(tc358860->regmap, PLL_DIV_CTRL1_1P62GBS_30MHZ, 0x09); //PLL_DIV_CTRL1_1p62gbs_30mhz
	regmap_write(tc358860->regmap, PLL_DIV_CTRL0_2P16GBS_30MHZ, 0xA4); //PLL_DIV_CTRL0_2p16gbs_30mhz
	regmap_write(tc358860->regmap, PLL_DIV_CTRL1_2P16GBS_30MHZ, 0x0C); //PLL_DIV_CTRL1_2p16gbs_30mhz
	regmap_write(tc358860->regmap, PLL_DIV_CTRL0_2P70GBS_30MHZ, 0x84); //PLL_DIV_CTRL0_2p70gbs_30mhz
	regmap_write(tc358860->regmap, PLL_DIV_CTRL1_2P70GBS_30MHZ, 0x12); //PLL_DIV_CTRL1_2p70gbs_30mhz
	regmap_write(tc358860->regmap, PLL_DIV_CTRL0_3P24GBS_30MHZ, 0xA0); //PLL_DIV_CTRL0_3p24gbs_30mhz
	regmap_write(tc358860->regmap, PLL_DIV_CTRL1_3P24GBS_30MHZ, 0x09); //PLL_DIV_CTRL1_3p24gbs_30mhz
	regmap_write(tc358860->regmap, PLL_DIV_CTRL0_4P32GBS_30MHZ, 0xA0); //PLL_DIV_CTRL0_4p32gbs_30mhz
	regmap_write(tc358860->regmap, PLL_DIV_CTRL1_4P32GBS_30MHZ, 0x0C); //PLL_DIV_CTRL1_4p32gbs_30mhz
	regmap_write(tc358860->regmap, PLL_DIV_CTRL0_5P40GBS_30MHZ, 0x80); //PLL_DIV_CTRL0_5p40gbs_30mhz
	regmap_write(tc358860->regmap, PLL_DIV_CTRL1_5P40GBS_30MHZ, 0x12); //PLL_DIV_CTRL1_5p40gbs_30mhz
	regmap_write(tc358860->regmap, OVERRIDE_DATA, 0x00); //OVERRIDE_DATA
	regmap_write(tc358860->regmap, PLL_FDBK_DIV2_MATCH, 0x09); //PLL_FDBK_DIV2_MATCH
	regmap_write(tc358860->regmap, REFCLK_COUNT, 0xDD); //REFCLK_COUNT
	regmap_write(tc358860->regmap, FEEDBK_CLK_COUNT, 0xCD); //FEEDBK_CLK_COUNT
	regmap_write(tc358860->regmap, COUNT_MSB, 0x70); //COUNT_MSB
	regmap_write(tc358860->regmap, OVERRIDE_DATA, 0x04); //OVERRIDE_DATA
	regmap_write(tc358860->regmap, PLL_FDBK_DIV2_MATCH, 0x0C); //PLL_FDBK_DIV2_MATCH
	regmap_write(tc358860->regmap, REFCLK_COUNT, 0xA6); //REFCLK_COUNT
	regmap_write(tc358860->regmap, FEEDBK_CLK_COUNT, 0xD3); //FEEDBK_CLK_COUNT
	regmap_write(tc358860->regmap, COUNT_MSB, 0x70); 
	regmap_write(tc358860->regmap, OVERRIDE_DATA, 0x0C); //OVERRIDE_DATA
	regmap_write(tc358860->regmap, PLL_FDBK_DIV2_MATCH, 0x12); //PLL_FDBK_DIV2_MATCH
	regmap_write(tc358860->regmap, REFCLK_COUNT, 0x6E); //REFCLK_COUNT
	regmap_write(tc358860->regmap, FEEDBK_CLK_COUNT, 0xCD); //FEEDBK_CLK_COUNT
	regmap_write(tc358860->regmap, COUNT_MSB, 0x70); //COUNT_MSB
    regmap_write(tc358860->regmap,BOOT_CTRL,0x00000007); // Boot Ctrl
    mdelay(1);
	
    do {
	    regmap_read(tc358860->regmap,BOOT_STATUS,&status);
	    udelay(100);
        }while(status!=0x00000007); // Check if 0x1018<bit2:0> is expected value

}

static void tc358860_setting_for_link_training(struct tc358860_bridge *tc358860)
{
    u8 status;

    do {
	regmap_read(tc358860->regmap,DPRX_MAINLINK_STATUS,&status);
	udelay(100);
    }while(status!=0x01); // Check if 0xB631<bit1:0>=01b.
    
    regmap_write(tc358860->regmap,DPCD_REV,0x11); // DPCD Rev
    regmap_write(tc358860->regmap,MAX_LINK_RATE,0x06); // Max Link Rate
    regmap_write(tc358860->regmap,MAX_LANE_COUNT,0x04); // Max Lane Count
    regmap_write(tc358860->regmap,AUXTXHSEN,0x0B); // Set AUXTXHSEN
    regmap_write(tc358860->regmap,CDR_PHASE_LP_EN,0x1E); // Set CDR_PHASE_LP_EN
    regmap_write(tc358860->regmap,DPCD,0x00); // DPCD 0700h
    // Dummy Color Bar Setting
    regmap_write(tc358860->regmap,GPIO3_INTERRUPT2,0x016A0000); // Monitor Signal Selection
    regmap_write(tc358860->regmap,GPIO3_INTERRUPT1,0x00004000); // GPIOOUTMODE
    regmap_write(tc358860->regmap,GPIO_RESET1,0x00000008); // GPIOC
    // GPIO setting for HPD (When HPD is controlled by GPIO2)
    //regmap_write(tc358860->regmap,GPIO_RESET0,0x00000000); // IOB_CTRL2
    //regmap_write(tc358860->regmap,GPIO_RESET3,0x00000000); // GPIOO
    //regmap_write(tc358860->regmap,GPIO_RESET1,0x0000000F); // GPIOC
    //regmap_write(tc358860->regmap,GPIO_RESET3,0x00000004); // GPIOO
    // Start Link Training
    // RS1_END
    // By detecting VSYNC monitor output on GPIO
    // RS2_START
    // After Link Training finishes
    // Check Link Training Status
    msleep(5000);
    do {
	    regmap_read(tc358860->regmap,LANE0_1_STATUS,&status);
	    udelay(100);
        }while(status!=0x77); // Check if 0x8202 is expected value.

    do {
	    regmap_read(tc358860->regmap,LANE2_3_STATUS,&status);
	    udelay(100);
        }while(status!=0x77); //Check if 0x8203 is expected value.

	do {
	    regmap_read(tc358860->regmap,ALIGN_STATUS,&status);
	    udelay(10);
        }while(status & 0x01!=0x01); // Check if 0x8204 is expected value.
    
    printk("finish in checking link training status!\n");
     
}

static void tc358860_dsi_config(struct tc358860_bridge *tc358860)
{
    u8 status;
    
    /* Timing Re-Setting */
    regmap_write(tc358860->regmap,VPCTRL_LEFT_0,0x011D); // VPCTRL_LEFT
    regmap_write(tc358860->regmap,VPCTRL_RIGHT_0,0x011D); // VPCTRL_RIGHT
    regmap_write(tc358860->regmap,DPRX_VTIM2_2,0x0008); // DPRX_VTIIM2
    regmap_write(tc358860->regmap,APF_VDELAYCNT_0,0x00000274); // DSI_APF_VDELAYCNT
    regmap_write(tc358860->regmap,DSI_HSYNC_WIDTH_0,0x0020); // DSI_HSYNC_WIDTH
    regmap_write(tc358860->regmap,DSI_HBPR_0,0x00C8); // DSI_HBPR
    /* DSI Transition Time Setting for 26MHz REFCLK */
    regmap_write(tc358860->regmap,LINE_INIT_COUNT, 0x00000A28); //Set_LINE_INIT_COUNT
	regmap_write(tc358860->regmap,DSI_HSTXVREGCNT, 0x0000001A); //Set_DSI_HSTXVREGCNT
	/* DSI Transition Time Setting for Non Preset */
    regmap_write(tc358860->regmap,PPI_DPHY_LPTXTIMECNT_0,0x00000001); // PPI_DPHY_LPTXTIMECNT
    regmap_write(tc358860->regmap,PPI_DPHY_TCLK_HEADERCNT_0,0x0050000); // PPI_DPHY_TCLK_HEADERCNT
    regmap_write(tc358860->regmap,PPI_DPHY_TCLK_TRAILCNT_0,0x00020000); // PPI_DPHY_TCLK_TRAILCNT
    regmap_write(tc358860->regmap,PPI_DPHY_THS_HEADERCNT_0,0x00000001); // PPI_DPHY_THS_HEADERCNT
    regmap_write(tc358860->regmap,PPI_DPHY_TWAKEUPCNT_0,0x00002710); // PPI_DPHY_TWAKEUPCNT
    regmap_write(tc358860->regmap,PPI_DPHY_TCLK_POSTCNT_0,0x00000006); // PPI_DPHY_TCLK_POSTCNT
    regmap_write(tc358860->regmap,PPI_DPHY_THSTRAILCNT_0,0x00020002); // PPI_DPHY_THSTRAILCNT
    regmap_write(tc358860->regmap,PPI_DSI_BTA_COUNT_0,0x00010001); // PPI_DSI_BTA_COUNT
    // DSI Start
    regmap_write(tc358860->regmap,DSITX_MODE_0,0x00000081); // DSI_DSITX_MODE
    regmap_write(tc358860->regmap,FUNC_MODE_0,0x00000000); // DSI_FUNC_MODE
    regmap_write(tc358860->regmap,DSI_PRTO_INT_MASK_0,0x00000001); // DSI_DSITX_START
    udelay(100);
    do {
	    regmap_read(tc358860->regmap,DSITX_INIT_INT_STAT,&status);
	    udelay(100);
    }while(status!=0x00000003); // Check if 0x2060/4060<bit1:0>=11b.
    // GPIO setting for LCD control.  (Depends on LCD specification and System configuration)
    //regmap_write(tc358860->regmap,GPIO_RESET3,0x0000000C); // GPIOO
    //regmap_write(tc358860->regmap,GPIO_RESET3,0x00000004); // GPIOO
    //mdelay(5);
    //regmap_write(tc358860->regmap,GPIO_RESET3,0x0000000C); // GPIOO
    /* DSI Hs Clock Mode */
    regmap_write(tc358860->regmap,FUNC_MODE_0,0x00000020); // DSI_FUNC_MODE
    regmap_write(tc358860->regmap,DSI0_CQMODE,0x00000000); // DSI0_CQMODE
    regmap_write(tc358860->regmap,DSI1_CQMODE,0x00000000); // DSI1_CQMODE
    // Command Transmission Before Video Start. (Depeds on LCD specification)
    // LCD Initialization
    /* Software Reset */
    regmap_write(tc358860->regmap,CQ_HEADER,0x81000105); // Short Pkt: b[7:0]=DataID,b31=En,b25=Long/Short,b24=LP/HS
    udelay(200);
    regmap_read(tc358860->regmap,CQ_STATUS,&status); // DSIG_CQ_STATUS
    printk("check if <bit0> = 1 ---%u\n", status);
    /* Set Pixel Format */
    regmap_write(tc358860->regmap,CQ_HEADER,0x81773A15); //Short Pkt: b[7:0]=DataID,b31=En,b25=Long/Short,b24=LP/HS
	udelay(200);
	regmap_read(tc358860->regmap,CQ_STATUS,&status); // DSIG_CQ_STATUS
    printk("check if <bit0> = 1 ---%u\n", status);
    /* Set Column Address */
    regmap_write(tc358860->regmap,CQ_HEADER,0x83000539); //Long Pkt: b[7:0]=DataID, b[23:8]=WC,b31=En,b25=Long/Short,b24=LP/HS
    regmap_write(tc358860->regmap,CQ_PAYLOAD,0x0400002A); // Payload data
    regmap_write(tc358860->regmap,CQ_PAYLOAD_MIRROR_0, 0x000000FF); //Payload data
    udelay(200);
    regmap_read(tc358860->regmap,CQ_STATUS,&status); // DSIG_CQ_STATUS
    printk("check if <bit0> = 1 ---%u\n", status);
    /* Set Page Address */
    regmap_write(tc358860->regmap,CQ_HEADER,0x83000539); //Long Pkt: b[7:0]=DataID, b[23:8]=WC,b31=En,b25=Long/Short,b24=LP/HS
    regmap_write(tc358860->regmap,CQ_PAYLOAD,0x0600002B); // Payload data
    regmap_write(tc358860->regmap,CQ_PAYLOAD_MIRROR_0,0x0000003F);// Payload data
    udelay(200);
    regmap_read(tc358860->regmap,CQ_STATUS,&status); // DSIG_CQ_STATUS
    printk("check if <bit0> = 1 ---%u\n", status);
    /* Set Tear On */
    regmap_write(tc358860->regmap,CQ_HEADER,0x81003515); //Short Pkt: b[7:0]=DataID,b31=En,b25=Long/Short,b24=LP/HS
    udelay(200);
    regmap_read(tc358860->regmap,CQ_STATUS,&status);// DSIG_CQ_STATUS
    printk("check if <bit0> = 1 ---%u\n", status);
    /* Set Tear Scanline */
    regmap_write(tc358860->regmap,CQ_HEADER,0x83000339); //Long Pkt: b[7:0]=DataID, b[23:8]=WC,b31=En,b25=Long/Short,b24=LP/HS
    regmap_write(tc358860->regmap,CQ_PAYLOAD,0x00000044); //Payload data
    udelay(200);
    regmap_read(tc358860->regmap,CQ_STATUS,&status); // DSIG_CQ_STATUS
    printk("check if <bit0> = 1 ---%u\n", status);
    /* Write Display Brightness */
    regmap_write(tc358860->regmap,CQ_HEADER,0x81FF5115); //Short Pkt: b[7:0]=DataID,b31=En,b25=Long/Short,b24=LP/HS
    udelay(200);
    regmap_read(tc358860->regmap,CQ_STATUS,&status); // DSIG_CQ_STATUS
    printk("check if <bit0> = 1 ---%u\n", status);
    /* Write Control Display */
    regmap_write(tc358860->regmap,CQ_HEADER,0x81245315); //Short Pkt: b[7:0]=DataID,b31=En,b25=Long/Short,b24=LP/HS
    udelay(200);
    regmap_read(tc358860->regmap,CQ_STATUS,&status); // DSIG_CQ_STATUS
    printk("check if <bit0> = 1 ---%u\n", status);
    /* Adaptive Brightness Control */
    regmap_write(tc358860->regmap,CQ_HEADER,0x81015515); // DSIG_CQ_HEADER
    udelay(200);
    regmap_read(tc358860->regmap,CQ_STATUS,&status); // DSIG_CQ_STATUS
    printk("check if <bit0> = 1 ---%u\n", status);
    /* Exit Sleep */
    regmap_write(tc358860->regmap,CQ_HEADER,0x81001105); // DSIG_CQ_HEADER
    udelay(200);
    regmap_read(tc358860->regmap,CQ_STATUS,&status); // DSIG_CQ_STATUS
    printk("check if <bit0> = 1 ---%u\n", status);
    mdelay(120);
    /* MCAP */
    regmap_write(tc358860->regmap,CQ_HEADER,0x8100B023); //Short Pkt: b[7:0]=DataID,b31=En,b25=Long/Short,b24=LP/HS
    udelay(200);
    regmap_read(tc358860->regmap,CQ_STATUS,&status); // DSIG_CQ_STATUS
    printk("check if <bit0> = 1 ---%u\n", status);
    
    /* Backlight Control 4 */
    regmap_write(tc358860->regmap,CQ_HEADER,0x83001429); //Long Pkt: b[7:0]=DataID, b[23:8]=WC,b31=En,b25=Long/Short,b24=LP/HS
    regmap_write(tc358860->regmap,CQ_PAYLOAD,0x48407DCE); // Payload data
    regmap_write(tc358860->regmap,CQ_PAYLOAD_MIRROR_0,0x88786756);// Payload data
    regmap_write(tc358860->regmap,CQ_PAYLOAD_MIRROR_1,0xC3B5A798);// Payload data
    regmap_write(tc358860->regmap,CQ_PAYLOAD_MIRROR_2,0xF2E9DED1);// Payload data
    regmap_write(tc358860->regmap,CQ_PAYLOAD_MIRROR_3,0x0004FFFA);// Payload data
    regmap_read(tc358860->regmap,CQ_STATUS,&status); // DSIG_CQ_STATUS
    printk("check if <bit0> = 1 ---%u\n", status);
}

static void tc358860_video_enable(struct tc358860_bridge *tc358860)
{
	regmap_write(tc358860->regmap,FUNC_MODE_0,0x80040010); //Set DSI clock mode
    regmap_write(tc358860->regmap,DSI0_CQMODE,0x80040010); // DSI0_CQMODE
    //regmap_write(tc358860->regmap,DSI1_CQMODE,0x80040010); // DSI1_CQMODE
    regmap_write(tc358860->regmap,(DSI0_VIDEO_START,0x00000001); // DSI0_VideoSTART
    //regmap_write(tc358860->regmap,(DSI1_VIDEO_START,0x00000001); // DSI1_VideoSTART
    /* Check if eDP video is coming */
    regmap_write(tc358860->regmap,(DPVIDEO_EN,0x00000001); // Set_DPVideoEn
    /* Command Transmission After Video Start. (Depends on LCD specification) */
    /* MCAP */
    regmap_write(tc358860->regmap,CQ_HEADER,0x8000B023); //Short Pkt: b[7:0]=DataID,b31=En,b25=Long/Short,b24=LP/HS
    udelay(35000);
    regmap_read(tc358860->regmap,CQ_STATUS,&status); // DSIG_CQ_STATUS
    printk("check if <bit0> = 1 ---%u\n", status);
    /* Interface Setting */
    regmap_write(tc358860->regmap,CQ_HEADER,0x8014B323); //Short Pkt: b[7:0]=DataID,b31=En,b25=Long/Short,b24=LP/HS
    udelay(35000);
    regmap_read(tc358860->regmap,CQ_STATUS,&status); // DSIG_CQ_STATUS
    printk("check if <bit0> = 1 ---%u\n", status);
    /* MCAP */
    regmap_write(tc358860->regmap,CQ_HEADER,0x8003B023); //Short Pkt: b[7:0]=DataID,b31=En,b25=Long/Short,b24=LP/HS
    udelay(35000);
    regmap_read(tc358860->regmap,CQ_STATUS,&status); // DSIG_CQ_STATUS
    printk("check if <bit0> = 1 ---%u\n", status);
    /* Set Display On */
    regmap_write(tc358860->regmap,CQ_HEADER,0x80002905); //Short Pkt: b[7:0]=DataID,b31=En,b25=Long/Short,b24=LP/HS
    udelay(35000);
    regmap_read(tc358860->regmap,CQ_STATUS,&status); // DSIG_CQ_STATUS
    printk("check if <bit0> = 1 ---%u\n", status);
    
    //RS2_END
}

static struct drm_encoder *tc358860_bridge_best_encoder(struct drm_connector *connector)
{
    struct tc358860_bridge *tc358860 = connector_to_tc358860(connector);

    return tc358860->bridge.encoder;
}

static const struct drm_connector_helper_funcs tc358860_connector_helper_funcs = {
    .get_modes = tc358860_bridge_get_modes,
    .best_encoder = tc358860_bridge_best_encoder,
};

static enum drm_connector_status tc358860_bridge_detect(struct drm_connector *connector,
                                    bool force)
{
    return connector_status_connected;
}

static void tc358860_bridge_destroy(struct drm_connector *connector)
{
    drm_connector_cleanup(connector);
}

static const struct drm_connector_funcs tc358860_connector_funcs = { 
    .dpms = drm_atomic_helper_connector_dpms,
    .fill_modes = drm_helper_probe_single_connector_modes,
    .detect = tc358860_bridge_detect,
    .destroy = tc358860_bridge_destroy,
    .reset = drm_atomic_helper_connector_reset,
    .atomic_duplicate_state = drm_atomic_helper_connector_duplicate_state,
    .atomic_destroy_state = drm_atomic_helper_connector_destroy_state,
};

static struct tc358860_bridge * bridge_to_tc358860(struct drm_bridge * bridge)
{
    return container_of(bridge, struct tc358860_bridge, bridge);
}

static int tc358860_bridge_attach(struct drm_bridge *bridge)
{
    struct tc358860_bridge *tc358860 = bridge_to_tc358860(bridge);
    int ret;

    if (!bridge->encoder) {
        DRM_ERROR("Parent encoder object not found");
        return -ENODEV;
    }

    tc358860->connector.port = bridge->of_node;
    tc358860->connector.polled = DRM_CONNECTOR_POLL_HPD;

    ret = drm_connector_init(bridge->dev, &tc358860->connector,
            &tc358860_connector_funcs, DRM_MODE_CONNECTOR_LVDS);
    if (ret) {
        DRM_ERROR("Failed to initialize connector with drm\n");
        return ret;
    }

    drm_connector_helper_add(&tc358860->connector,
                    &tc358860_connector_helper_funcs);
    drm_mode_connector_attach_encoder(&tc358860->connector,
                            bridge->encoder);
    if (tc358860->panel)
            drm_panel_attach(tc358860->panel, &tc358860->connector);
    return 0;
}

static void tc358860_bridge_post_disable(struct drm_bridge *bridge)
{
    struct tc358860_bridge *tc358860 = bridge_to_tc358860(bridge);

    if (tc358860->pre_enabled) {
        if (tc358860->panel && tc358860->panel->funcs && tc358860->panel->funcs->unprepare)
            drm_panel_unprepare(tc358860->panel);
        
        if (tc358860->gpio_rst)
            gpiod_direction_output(tc358860->gpio_rst, 1);
        
        if (tc358860->gpio_enable)
            gpiod_direction_output(tc358860->gpio_enable, 0);

    }

    return;
}

static void tc358860_bridge_disable(struct drm_bridge *bridge)
{
   struct tc358860_bridge *tc358860 = bridge_to_tc358860(bridge);
   int ret;

   if (!tc358860->enabled)
        return;
    
   if (tc358860->panel) {
       ret = drm_panel_disable(tc358860->panel);
       if (ret) {
            DRM_ERROR("failed to disable panel");
            return;
       }
   }
   tc358860->enabled = false;
   
}

static void tc358860_bridge_pre_enable(struct drm_bridge *bridge)
{
    struct tc358860_bridge *tc358860 = bridge_to_tc358860(bridge);
    int ret;

    if(tc358860->pre_enabled)
        return;

    if (tc358860->gpio_enable)
        gpiod_direction_output(tc358860->gpio_enable, 1);

    if (tc358860->gpio_rst) {
        gpiod_direction_output(tc358860->gpio_rst, 1);
        mdelay(120);
        gpiod_direction_output(tc358860->gpio_rst, 0);
        mdelay(120);
    }

    if (tc358860->panel && tc358860->panel->funcs && tc358860->panel->funcs->prepare)
        drm_panel_prepare(tc358860->panel);

    tc358860_io_voltage_setting(tc358860);
    tc358860_boot_sequence(tc358860);
    //add by sn03955
    tc358860_refclk_setting(tc358860);
    
    tc358860_dprx_cad_setting(tc358860);
    tc358860_setting_for_link_training(tc358860);
    tc358860_dsi_config(tc358860);
    tc358860_video_enable(tc358860);
    tc358860->pre_enabled = true; 
    
}

static void tc358860_bridge_enable(struct drm_bridge *bridge)
{
    struct tc358860_bridge *tc358860 = bridge_to_tc358860(bridge);
    int ret;

    if (tc358860->enabled)
        return;
    if (tc358860->panel && tc358860->panel->funcs && tc358860->panel->funcs->enable) {
        ret = drm_panel_enable(tc358860->panel);
        if (ret) {
            DRM_ERROR("failed to enable panel\n");
            return;
        }

    }
    tc358860->enabled = true;
}

struct drm_bridge_funcs tc358860_bridge_funcs = {
    .attach = &tc358860_bridge_attach,
    //.mode_fixup = &tc358860_bridge_mode_fixup;
    .disable =  &tc358860_bridge_disable,
    .post_disable = &tc358860_bridge_post_disable,
    .pre_enable = &tc358860_bridge_pre_enable,
    .enable = &tc358860_bridge_enable,
    
};

static const struct of_device_id tc358860_devices[] = { 
    {.compatible = "toshiba,tc358860",},                                                                                                                                         
    {}  
};

MODULE_DEVICE_TABLE(of, tc358860_devices);

int tc358860_probe(struct i2c_client *client, const struct i2c_device_id *id)
{

    struct tc358860_bridge *tc358860;
    struct device *dev = &client->dev;
    int ret;
    
    dev_info(&client->dev, "===>hgc:%s\n",__func__);
    tc358860 = devm_kzalloc(dev, sizeof(*tc358860), GFP_KERNEL);
    if (!tc358860)
        return -ENOMEM;
    tc358860->dev = dev;
    tc358860->client = client;
    i2c_set_clientdata(client, tc358860);

    ret = drm_of_find_panel_or_bridge(dev->of_node, 1, -1,
                        &tc358860->panel, NULL);
    if (ret)
        return ret;
    
    tc358860->gpio_rst = devm_gpiod_get_optional(dev, "reset-gpios", 0);
    if (IS_ERR(tc358860->gpio_rst)) {
        ret = PTR_ERR(tc358860->gpio_rst);
        dev_err(dev, "cannot get gpio_rst %d\n", ret);
        return ret;
    }

    tc358860->gpio_enable = devm_gpiod_get_optional(dev, "enable_gpios", 0);
    if (IS_ERR(tc358860->gpio_enable)) {
        ret = PTR_ERR(tc358860->gpio_enable);
        dev_err(dev, "cannot get gpio_enable %d\n", ret);
        return ret;
    }
	
	tc358860->regmap = devm_regmap_init_i2c(client, &tc358860_regmap_config);
	if (IS_ERR(tc358860->regmap)) {
        ret = PTR_ERR(tc358860->regmap);
        dev_err(dev, "failed to initialize regmap: %d\n", ret);
        return ret;
    }

    //tc358860->dual_channel = of_property_read_bool(dev->of_node, 
                              //"dual-channel");
    //tc358860->max_lane_count = id->driver_data;

    //if (of_property_read_u32(dev->of_node, "lane-count", 
    //                            &tc358860->lane_count)) {
    //    tc358860->lane_count = tc358860->max_lane_count;
    //} else if (tc358860->lane_count > tc358860->max_lane_count) {
    //    dev_info(dev, "lane-count property is too high, using max_lane_count\n");
    //    tc358860->lane_count = tc358860->max_lane_count;
    //}
    tc358860->bridge.funcs = &tc358860_bridge_funcs;
    tc358860->bridge.of_node = dev->of_node;
    ret = drm_bridge_add(&tc358860->bridge);
    if (ret) {
        DRM_ERROR("Failed to add bridge\n");
        return ret;
    }
    
    printk("tc358860_probe finish\n");
    return 0;
}

int tc358860_remove(struct i2c_client *client)
{
    return 0;
}

static const struct i2c_device_id tc358860_i2c_table[] = {

    {"tc358860", 2},
    {}
        
};
MODULE_DEVICE_TABLE(i2c, tc358860_i2c_table)


static struct i2c_driver tc358860_driver = {
    .id_table   = tc358860_i2c_table,
    .probe      = tc358860_probe,
    .remove     = tc358860_remove,
    .driver     = {
        .name   = "tc358860",
        .of_match_table = tc358860_devices,
    },
};

module_i2c_driver(tc358860_driver);

MODULE_AUTHOR("Guo chun HUANG <hero.huang@rock-chips.com>");
MODULE_DESCRIPTION("TOSHIBA TC358860 eDP-MIPI converter driver");
MODULE_LICENSE("GPL v2");
