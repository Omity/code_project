void tc358860_0730(void)
{
unsigned char rdatac;
//unsigned int rdatai;
unsigned long rdatal;
// RS1_START
// IO Voltahge Setting
i2c1_edp2dsi_write32(0x0800,0x00000001); // IOB_CTRL1
// Boot Settings
i2c1_edp2dsi_write32(0x1000,0x00006978); // BootWaitCount
i2c1_edp2dsi_write32(0x1004,0x00040904); // Boot Set0
i2c1_edp2dsi_write32(0x1008,0x0574000C); // Boot Set1
i2c1_edp2dsi_write32(0x100C,0x23000513); // Boot Set2
i2c1_edp2dsi_write32(0x1010,0x00C80020); // Boot Set3
i2c1_edp2dsi_write32(0x1014,0x00000003); // Boot Ctrl
Waitx1ms(1);
while((rdatal=i2c1_edp2dsi_read32(0x1018))!=0x00000006){;} // Check if 0x1018<bit2:0> is expected value
// Additional Setting for eDP
i2c1_edp2dsi_write8(0x8003,0x41); // Max Downspread
i2c1_edp2dsi_write8(0xB400,0x0D); // AL Mode Control Link
// DPRX CAD Register Setting
i2c1_edp2dsi_write8(0xB88E,0xFF); // Set CR_OPT_WCNT0
i2c1_edp2dsi_write8(0xB88F,0xFF); // Set CR_OPT_WCNT1
i2c1_edp2dsi_write8(0xB89A,0xFF); // Set CR_OPT_WCNT2
i2c1_edp2dsi_write8(0xB89B,0xFF); // Set CR_OPT_WCNT3
i2c1_edp2dsi_write8(0xB800,0x0E); // Set CDR_PHASE_LP_EN
i2c1_edp2dsi_write8(0xBB26,0x02); // RX_VREG_VALUE
i2c1_edp2dsi_write8(0xBB01,0x20); // RX_VREG_ENABLE
i2c1_edp2dsi_write8(0xB8C0,0xF1); // RX_CDR_LUT1
i2c1_edp2dsi_write8(0xB8C1,0xF1); // RX_CDR_LUT2
i2c1_edp2dsi_write8(0xB8C2,0xF0); // RX_CDR_LUT3
i2c1_edp2dsi_write8(0xB8C3,0xF0); // RX_CDR_LUT4
i2c1_edp2dsi_write8(0xB8C4,0xF0); // RX_CDR_LUT5
i2c1_edp2dsi_write8(0xB8C5,0xF0); // RX_CDR_LUT6
i2c1_edp2dsi_write8(0xB8C6,0xF0); // RX_CDR_LUT7
i2c1_edp2dsi_write8(0xB8C7,0xF0); // RX_CDR_LUT8
i2c1_edp2dsi_write8(0xB80B,0x00); // PLL_CP1P1
i2c1_edp2dsi_write8(0xB833,0x00); // PLL_CP1P2
i2c1_edp2dsi_write8(0xB85B,0x00); // PLL_CP1P3
i2c1_edp2dsi_write8(0xB810,0x00); // PLL_CP2P1
i2c1_edp2dsi_write8(0xB838,0x00); // PLL_CP2P2
i2c1_edp2dsi_write8(0xB860,0x00); // PLL_CP2P3
i2c1_edp2dsi_write8(0xB815,0x00); // PLL_CP2P4
i2c1_edp2dsi_write8(0xB83D,0x00); // PLL_CP2P5
i2c1_edp2dsi_write8(0xB865,0x00); // PLL_CP2P6
i2c1_edp2dsi_write8(0xB81A,0x00); // PLL_CP2P7
i2c1_edp2dsi_write8(0xB842,0x00); // PLL_CP2P8
i2c1_edp2dsi_write8(0xB86A,0x00); // PLL_CP2P9
i2c1_edp2dsi_write8(0xB81F,0x00); // PLL_CP3P1
i2c1_edp2dsi_write8(0xB847,0x00); // PLL_CP3P2
i2c1_edp2dsi_write8(0xB86F,0x00); // PLL_CP3P3
i2c1_edp2dsi_write8(0xB824,0x00); // PLL_CP4P1
i2c1_edp2dsi_write8(0xB84C,0x00); // PLL_CP4P2
i2c1_edp2dsi_write8(0xB874,0x00); // PLL_CP4P3
i2c1_edp2dsi_write8(0xB829,0x00); // PLL_CP4P4
i2c1_edp2dsi_write8(0xB851,0x00); // PLL_CP4P5
i2c1_edp2dsi_write8(0xB879,0x00); // PLL_CP4P6
i2c1_edp2dsi_write8(0xB82E,0x00); // PLL_CP5P7
i2c1_edp2dsi_write8(0xB856,0x00); // PLL_CP5P2
i2c1_edp2dsi_write8(0xB87E,0x00); // PLL_CP5P3
i2c1_edp2dsi_write8(0xBB90,0x10); // ctle_em_data_rate_control_0[7:0]
i2c1_edp2dsi_write8(0xBB91,0x0F); // ctle_em_data_rate_control_1[7:0]
i2c1_edp2dsi_write8(0xBB92,0xF6); // ctle_em_data_rate_control_2[7:0]
i2c1_edp2dsi_write8(0xBB93,0x10); // ctle_em_data_rate_control_3[7:0]
i2c1_edp2dsi_write8(0xBB94,0x0F); // ctle_em_data_rate_control_4[7:0]
i2c1_edp2dsi_write8(0xBB95,0xF6); // ctle_em_data_rate_control_5[7:0]
i2c1_edp2dsi_write8(0xBB96,0x10); // ctle_em_data_rate_control_6[7:0]
i2c1_edp2dsi_write8(0xBB97,0x0F); // ctle_em_data_rate_control_7[7:0]
i2c1_edp2dsi_write8(0xBB98,0xF6); // ctle_em_data_rate_control_8[7:0]
i2c1_edp2dsi_write8(0xBB99,0x10); // ctle_em_data_rate_control_A[7:0]
i2c1_edp2dsi_write8(0xBB9A,0x0F); // ctle_em_data_rate_control_B[7:0]
i2c1_edp2dsi_write8(0xBB9B,0xF6); // ctle_em_data_rate_control_0[7:0]
i2c1_edp2dsi_write8(0xB88A,0x03); // CR_OPT_CTRL
i2c1_edp2dsi_write8(0xB896,0x03); // EQ_OPT_CTRL
i2c1_edp2dsi_write8(0xBBD1,0x07); // ctle_em_contro_1
i2c1_edp2dsi_write8(0xBBB0,0x07); // eye_configuration_0
i2c1_edp2dsi_write8(0xB88B,0x04); // CR_OPT_MIN_EYE_VALID
i2c1_edp2dsi_write8(0xB88C,0x45); // CR_OPT_WCNT0_EYE
i2c1_edp2dsi_write8(0xB88D,0x05); // CT_OPT_WCNT1_EYE
i2c1_edp2dsi_write8(0xB897,0x04); // EQ_OPT_MIN_EYE_VALID
i2c1_edp2dsi_write8(0xB898,0xE0); // EQ_OPT_WCNT0_FEQ
i2c1_edp2dsi_write8(0xB899,0x2E); // EQ_OPT_WCNT1_FEQ
i2c1_edp2dsi_write8(0x800E,0x00); // TRAINING_AUX_RD_INTERVAL
i2c1_edp2dsi_write32(0x1014,0x00000007); // Boot Ctrl
Waitx1ms(1);
while((rdatal=i2c1_edp2dsi_read32(0x1018))!=0x00000007){Waitx1us(100);} // Check if 0x1018<bit2:0> is expected value
// eDP Settings for Link Training
while((rdatac=i2c1_edp2dsi_read8(0xB631))!=0x01){Waitx1us(100);} // Check if 0xB631<bit1:0>=01b.
i2c1_edp2dsi_write8(0x8000,0x11); // DPCD Rev
i2c1_edp2dsi_write8(0x8001,0x0A); // Max Link Rate
i2c1_edp2dsi_write8(0x8002,0x04); // Max Lane Count
i2c1_edp2dsi_write8(0xB608,0x0B); // Set AUXTXHSEN
i2c1_edp2dsi_write8(0xB800,0x1E); // Set CDR_PHASE_LP_EN
i2c1_edp2dsi_write8(0x8700,0x00); // DPCD 0700h
// Dummy Color Bar Setting
i2c1_edp2dsi_write32(0x5014,0x016A0000); // Monitor Signal Selection
i2c1_edp2dsi_write32(0x008C,0x00004000); // GPIOOUTMODE
i2c1_edp2dsi_write32(0x0080,0x00000008); // GPIOC
// GPIO setting for HPD (When HPD is controlled by GPIO2)
i2c1_edp2dsi_write32(0x0804,0x00000000); // IOB_CTRL2
i2c1_edp2dsi_write32(0x0084,0x00000000); // GPIOO
i2c1_edp2dsi_write32(0x0080,0x0000000F); // GPIOC
i2c1_edp2dsi_write32(0x0084,0x00000004); // GPIOO
// Start Link Training
// RS1_END
// By detecting VSYNC monitor output on GPIO
// RS2_START
// After Link Training finishes
// Check Link Training Status
Waitx1ms(100);
while((rdatac=i2c1_edp2dsi_read8(0x8202))!=0x77){Waitx1ms(100);} // Check if 0x8202 is expected value.
while((rdatac=i2c1_edp2dsi_read8(0x8203))!=0x77){Waitx1ms(100);} // Check if 0x8203 is expected value.
while(((rdatac=i2c1_edp2dsi_read8(0x8204)) & 0x01)!=0x01){Waitx1ms(10);} // Check if 0x8204 is expected value.
// Timing Re-Setting
i2c1_edp2dsi_write16(0x0108,0x0574); // VPCTRL_LEFT
i2c1_edp2dsi_write16(0x011C,0x0574); // VPCTRL_RIGHT
i2c1_edp2dsi_write16(0x0152,0x000C); // DPRX_VTIIM2
i2c1_edp2dsi_write32(0x4070,0x00000513); // DSI_APF_VDELAYCNT
i2c1_edp2dsi_write16(0x408C,0x0020); // DSI_HSYNC_WIDTH
i2c1_edp2dsi_write16(0x4090,0x00C8); // DSI_HBPR
i2c1_edp2dsi_write32(0x4154,0x00000007); // PPI_DPHY_LPTXTIMECNT
i2c1_edp2dsi_write32(0x4158,0x00280005); // PPI_DPHY_TCLK_HEADERCNT
i2c1_edp2dsi_write32(0x415C,0x000D0006); // PPI_DPHY_TCLK_TRAILCNT
i2c1_edp2dsi_write32(0x4160,0x000E0007); // PPI_DPHY_THS_HEADERCNT
i2c1_edp2dsi_write32(0x4164,0x00004268); // PPI_DPHY_TWAKEUPCNT
i2c1_edp2dsi_write32(0x4168,0x0000000E); // PPI_DPHY_TCLK_POSTCNT
i2c1_edp2dsi_write32(0x416C,0x000D0009); // PPI_DPHY_THSTRAILCNT
i2c1_edp2dsi_write32(0x4178,0x00070007); // PPI_DSI_BTA_COUNT
// DSI Start
i2c1_edp2dsi_write32(0x407C,0x00000081); // DSI_DSITX_MODE
i2c1_edp2dsi_write32(0x4050,0x00000000); // DSI_FUNC_MODE
i2c1_edp2dsi_write32(0x401C,0x00000001); // DSI_DSITX_START
Waitx1us(100);
while((rdatal=i2c1_edp2dsi_read32(0x4060))!=0x00000003){Waitx1us(100);} // Check if 0x2060/4060<bit1:0>=11b.
// GPIO setting for LCD control.  (Depends on LCD specification and System configuration)
i2c1_edp2dsi_write32(0x0084,0x0000000C); // GPIOO
i2c1_edp2dsi_write32(0x0084,0x00000004); // GPIOO
Waitx1ms(5);
i2c1_edp2dsi_write32(0x0084,0x0000000C); // GPIOO
// DSI Hs Clock Mode
i2c1_edp2dsi_write32(0x4050,0x00000020); // DSI_FUNC_MODE
i2c1_edp2dsi_write32(0x2A10,0x00000000); // DSI0_CQMODE
i2c1_edp2dsi_write32(0x3A10,0x00000000); // DSI1_CQMODE
// Command Transmission Before Video Start. (Depeds on LCD specification)
// LCD Initialization
// Exit Sleep
i2c1_edp2dsi_write32(0x42FC,0x83000329); // DSIG_CQ_HEADER
i2c1_edp2dsi_write32(0x4300,0x00160010); // DSIG_CQ_HEADER
Waitx1us(200);
i2c1_edp2dsi_read32(0x4200); // DSIG_CQ_STATUS
i2c1_edp2dsi_write32(0x42FC,0x83000005); // DSIG_CQ_HEADER
Waitx1us(200);
i2c1_edp2dsi_read32(0x4200); // DSIG_CQ_STATUS
i2c1_edp2dsi_write32(0x42FC,0x83000329); // DSIG_CQ_HEADER
i2c1_edp2dsi_write32(0x4300,0x00000110); // DSIG_CQ_HEADER
Waitx1us(200);
i2c1_edp2dsi_read32(0x4200); // DSIG_CQ_STATUS
i2c1_edp2dsi_write32(0x42FC,0x81000005); // DSIG_CQ_HEADER
Waitx1us(200);
i2c1_edp2dsi_read32(0x4200); // DSIG_CQ_STATUS
i2c1_edp2dsi_write32(0x42FC,0x83000329); // DSIG_CQ_HEADER
i2c1_edp2dsi_write32(0x4300,0x00070710); // DSIG_CQ_HEADER
Waitx1us(200);
i2c1_edp2dsi_read32(0x4200); // DSIG_CQ_STATUS
i2c1_edp2dsi_write32(0x42FC,0x81000005); // DSIG_CQ_HEADER
Waitx1us(200);
i2c1_edp2dsi_read32(0x4200); // DSIG_CQ_STATUS
i2c1_edp2dsi_write32(0x42FC,0x81001105); // DSIG_CQ_HEADER
Waitx1us(200);
i2c1_edp2dsi_read32(0x4200); // DSIG_CQ_STATUS
i2c1_edp2dsi_write32(0x42FC,0x81002905); // DSIG_CQ_HEADER
Waitx1us(200);
i2c1_edp2dsi_read32(0x4200); // DSIG_CQ_STATUS
i2c1_edp2dsi_write32(0x4050,0x00000020); // DSI_FUNC_MODE
i2c1_edp2dsi_write32(0x2A10,0x80040010); // DSI0_CQMODE
i2c1_edp2dsi_write32(0x3A10,0x80040010); // DSI1_CQMODE
i2c1_edp2dsi_write32(0x2A04,0x00000001); // DSI0_VideoSTART
i2c1_edp2dsi_write32(0x3A04,0x00000001); // DSI1_VideoSTART
// Check if eDP video is coming
i2c1_edp2dsi_write32(0x0154,0x00000001); // Set_DPVideoEn
// Command Transmission After Video Start. (Depends on LCD specification)
// RS2_END
}