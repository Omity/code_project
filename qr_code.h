/******************************************************************************
  * File       : qr_code.h
  * 
  * Copyright (c) 2011 by Li.Recan < 721317716@qq.com >
  * 
  * DESCRIPTION: API for creating qrcode by C code.
  * 
  * Modification history
  * --------------------------------------------------------------------------
  * Date         Version  Author       History
  * --------------------------------------------------------------------------
  * 2016-10-15   1.0.0    Li.Recan     written
******************************************************************************/
  
#ifndef __QR_CODE__
#define __QR_CODE__
 
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
 
#define QRCODE_MAX_LEN        500 //max string len for creating qrcode
 
typedef enum 
{
    ZINT_OK                 = 0,
    ZINT_ERR_INV_DATA       = -1, //input invalid data
    ZINT_ERR_TOO_LONG       = -2, //len for input data is too long    
    ZINT_ERR_INV_CODE_ID    = -3,//the code type is not supported by zint
    ZINT_ERR_MEMORY         = -4, //malloc memory error in zint lib
    ZINT_ERR_FILE_NAME      = -5, //qrcode file isn'y end in .png, .eps or .svg.
    ZINT_ERR_LIB_RET        = -6, //zint lib ret error, real ret code should be \
                                    zint api ret code
}ZINT_RET_CODE;
 
/******************************************************************************
Descpribe: Create Qrcode API with C Code by calling zint lib.
Input    : pQrCodeData, the qrcode data buf
           QrcodeLen, the len of qrcode data, but it can be 0
           pQrCodeFile, the output file name of qrcode, it can be NULL           
Output   : pZintRet, to store the ret code from linzint. 
Return   : 0 is ok, and other values are fail. See the meanings in enum ZINT_RET_CODE
Notes    : pQrCodeFile, Must end in .png, .eps or .svg. when isn,t NULL string.
******************************************************************************/
ZINT_RET_CODE Zint_Create_QrCode(uint8_t *pQrCodeData, int QrcodeLen, \
                                 char *pQrCodeFile, int *pZintRet);

#define Debuging(fmt, arg...)       printf("[%20s, %4d] "fmt, __FILE__, __LINE__, ##arg)
 
#ifdef __cplusplus
}
#endif
 
#endif /* __QR_CODE__ */
