#include <stdio.h>

// Project Header
#include "qr_code.h"
 
int main(int argc, char *argv[])
{
    int ZintLibRet           = 0; //ret code from zint lib
    ZINT_RET_CODE ZintRet    = 0; //ret code from zint_code api
    char QrcodeData[]        = "I love zint lib. 测试一下gbk编码 ...";
    char QrcodeDataDef[]     = "This's default qrcode file name : out.png ";
    char QrcodeFile[]        = "MyQrcode.png"; // Must end in .png,.eps or.svg 
         
    //test with inputing qrcode_file name
    ZintRet = Zint_Create_QrCode((uint8_t*)QrcodeData, 0, QrcodeFile, \
                                  &ZintLibRet);
    if(ZINT_OK != ZintRet)
    {
        Debuging("Create qrcode err, ZintRet = %d, ZintLibRet = %d\n", \
                  ZintRet, ZintLibRet);
    }
    else
    {
        Debuging("Create qrcode OK ! View qrcode file : %s in cur path. ZintRet\
                  = %d, ZintLibRet = %d\n", QrcodeFile, ZintRet, ZintLibRet);
    }
    
    //test without inputing qrcode_file name
    ZintRet = Zint_Create_QrCode((uint8_t*)QrcodeDataDef, 0, NULL, &ZintLibRet);
    if(ZINT_OK != ZintRet)
    {
        Debuging("Create qrcode err, ZintRet = %d, ZintLibRet = %d\n", ZintRet, \
                                                                     ZintLibRet);
    }
    else
    {
        Debuging("Create qrcode OK ! View qrcode file : out.png in cur path. \
                  ZintRet = %d, ZintLibRet = %d\n", ZintRet, ZintLibRet);
    }
    
    return 0;
}
