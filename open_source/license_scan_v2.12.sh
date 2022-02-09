#!/bin/bash

#############################################################
# SEARCH LICENSE FOR CODE
#       Author : huangjc@rock-chips.com
#       Data   : 2015-03-24
#############################################################
#      History:
#      V2.8-vpu:
#      V2.9:  add some aosp android-5.1.1_r1 omit keywords
#      V2.10: add some uboot/kernel omit keywords
#      V2.11: add scan for GooglePlay etc
#      V2.12: add some uboot/kernel omit path
#
#
###########################################################

echo "version 2.12"

#rm ../License_test -rf
TAGS_NAME=License_$(date +%Y_%m_%d_%H_%M)
mkdir ../$TAGS_NAME

if [ "$1"x == ""x  ];then
  ROOT_DIR_TO_SCAN=./
  
else
  ROOT_DIR_TO_SCAN=$1
fi
echo "ROOT_DIR_TO_SCAN is $ROOT_DIR_TO_SCAN"

GMS_KEYWORDS="GooglePlay|Video|GmsCore|GoogleServicesFramework"
#scan GMS module
#find $ROOT_DIR_TO_SCAN -type f ! \( -path "**.png*" -o -path "*.git*" -o -path "*./external/lldb*" -o -path "*./out*" -o -path "**.html*" -o -path "**.map*" -o -path "*.repo*" -o -path "*.jpg*" -o -path "*.o*" -o -path "*.zip*" -o -path "*License_search.sh*" \) -name "*.apk" |xargs grep -Er "GooglePlay|Video|GmsCore|GoogleServicesFramework" > ./../$TAGS_NAME/GMS_scan.txt

find $ROOT_DIR_TO_SCAN  -type f ! -iregex '.*\.(repo\|git\|png\|jpg\*\|zip\|out\|html\|mk\|sh' -name "*.apk" |xargs grep -Er $GMS_KEYWORDS > ./../$TAGS_NAME/GMS_scan.txt


##################开始LICENSE扫描

echo "start search license from all code...."

#1.扫描各模块目录，拷贝原始LICENSE文件
#find $ROOT_DIR_TO_SCAN -name LICENSE | cpio -pdv ./../$TAGS_NAME
#find $ROOT_DIR_TO_SCAN -name LICENSE.TXT | cpio -pdv ./../$TAGS_NAME
#find $ROOT_DIR_TO_SCAN -name NOTICE | cpio -pdv ./../$TAGS_NAME
#find $ROOT_DIR_TO_SCAN -name NOTICE.TXT | cpio -pdv ./../$TAGS_NAME
#find $ROOT_DIR_TO_SCAN -name MODULE_LICENSE* | cpio -pdv ./../$TAGS_NAME

#2.扫描所有源代码文件。。。
   find $ROOT_DIR_TO_SCAN -type f ! \( -path "**.png*" -o -path "*.git*" -o -path "*./external/lldb*" -o -path "*./out*" -o -path "**.html*" -o -path "**.map*" -o -path "*.repo*" -o -path "*.jpg*" -o -path "*.o*" -o -path "*.zip*" -o -path "*License_search.sh*" \) |xargs grep -Er "Licensed|Copyright" > ./../$TAGS_NAME/Licensed_allCode.txt

   find $ROOT_DIR_TO_SCAN -type f ! \( -path "**.png*" -o -path "*.git*" -o -path "*./external/lldb*" -o -path "*./out*" -o -path "**.html*" -o -path "**.map*" -o -path "*.repo*" -o -path "*.jpg*" -o -path "*.o*" -o -path "*.zip*" -o -path "*License_search.sh*" \) |xargs grep -Er "Licensed|Copyright" -L > ./../$TAGS_NAME/UnLicense_allCode.txt

#cp external/lldb/LICENSE.TXT ./../$TAGS_NAME/external_lldb_LICENSE.TXT

echo "=======finsh create Licensed and UnLicense file list..."
###################开始筛选

#1.检查平台manifests匹配
cp .repo/manifest.xml ./../$TAGS_NAME/manifests.xml

#2.根据代码仓库筛选，筛掉谷歌原生代码目录，保留无License文件列表
echo "=======Start create UnLicense filter list..."
if [ ! -f ./../$TAGS_NAME/manifests.xml ]
then
         echo "manifests.xml not fount! default rk3288.xml "
#     cp .repo/manifests/rk3288.xml ./../$TAGS_NAME/manifests.xml
else

  sed -i "/remote=\"rk\"/d" ./../$TAGS_NAME/manifests.xml
  sed -i "/version=\"1.0\"/,/\<project/d" ./../$TAGS_NAME/manifests.xml
  sed -i "/\<copyfile/d" ./../$TAGS_NAME/manifests.xml
  sed -i "/version=\"1.0\"/d" ./../$TAGS_NAME/manifests.xml

  cat ./../$TAGS_NAME/manifests.xml |awk -F "path=\"|\"" '{print $2}' > ./../$TAGS_NAME/UnRK_manifests.xml
  rm ./../$TAGS_NAME/manifests.xml

  cp ./../$TAGS_NAME/UnLicense_allCode.txt ./../$TAGS_NAME/UnLicense_suspicion.txt

  cat ./../$TAGS_NAME/UnRK_manifests.xml | while read line
  do
    # echo $line
    int=$(echo $line | awk -F"/" '{print NF-1}')
    if [ "$int"x == "0"x  ];then
        sed -i "/$line/d" ./../$TAGS_NAME/UnLicense_suspicion.txt
    else
        line1=$(echo $line | awk 'gsub("/","\\/",$0)')
        sed -i "/$line1/d" ./../$TAGS_NAME/UnLicense_suspicion.txt
    fi
  done 
fi

##3.开始过滤有License文件
 #生成已知License文件列表
echo "=======Start create we know License list..."
cat ./../$TAGS_NAME/Licensed_allCode.txt |grep -E " Rockchip |Android Open Source|Apache License|MPL|GPL|LGPL|BSD" > ./../$TAGS_NAME/Licensed_Konw_list.txt
 
 #生成有但未明确License文件列表
echo "=======Start create unknow License list..."
cat ./../$TAGS_NAME/Licensed_allCode.txt |grep -Ev "Android Open Source" > ./../$TAGS_NAME/Licensed_UnKonw_list_1.txt
cat ./../$TAGS_NAME/Licensed_UnKonw_list_1.txt |grep -Ev "Apache License" > ./../$TAGS_NAME/Licensed_UnKonw_list_2.txt
cat ./../$TAGS_NAME/Licensed_UnKonw_list_2.txt |grep -Ev "MPL" > ./../$TAGS_NAME/Licensed_UnKonw_list_3.txt
cat ./../$TAGS_NAME/Licensed_UnKonw_list_3.txt |grep -Ev "GPL" > ./../$TAGS_NAME/Licensed_UnKonw_list_4.txt
cat ./../$TAGS_NAME/Licensed_UnKonw_list_4.txt |grep -Ev "LGPL" > ./../$TAGS_NAME/Licensed_UnKonw_list_5.txt
cat ./../$TAGS_NAME/Licensed_UnKonw_list_5.txt |grep -Ev "BSD" > ./../$TAGS_NAME/Licensed_UnKonw_list_6.txt
cat ./../$TAGS_NAME/Licensed_UnKonw_list_6.txt |grep -Ev " Rockchip " > ./../$TAGS_NAME/Licensed_suspicion.txt
rm ./../$TAGS_NAME/Licensed_UnKonw_list_*

sed -i "/libcore\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/bionic\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/developers\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/build\/tools\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/sdk\/eclipse\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/prebuilts\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/ndk\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/bootable\/recovery\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/Binary file /d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/hardware\/ril\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/hardware\/libhardware\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/hardware\/libhardware_legacy\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/external\/broadcom_bplus\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/external\/busybox\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/external\/ntfs-3g\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/external\/pcre\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/external\/e2fsprogs\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/external\/ppp\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/external\/skia\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/external\/bluetooth_bplus\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/external\/bluetooth_rtk\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/external\/bluetooth\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/external\/rk-pcba-test\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/external\/rp-pppoe\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/external\/io\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/external\/mmc-utils\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/external\/wlan_loader\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/external\/wpa_supplicant_8\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/external\/wpa_supplicant_8_rtl\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
sed -i "/external\/wpa_supplicant_8_esp\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
#sed -i "/external\/pdfium\//d" ./../$TAGS_NAME/Licensed_suspicion.txt
if [ -f ./../$TAGS_NAME/UnRK_manifests.xml ]
 then
 cat ./../$TAGS_NAME/UnRK_manifests.xml | while read line
  do
    # echo $line
    int=$(echo $line | awk -F"/" '{print NF-1}')
    if [ "$int"x == "0"x  ];then
        sed -i "/$line/d" ./../$TAGS_NAME/Licensed_suspicion.txt
    else
        line1=$(echo $line | awk 'gsub("/","\\/",$0)')
        sed -i "/$line1/d" ./../$TAGS_NAME/Licensed_suspicion.txt
    fi
  done
fi
########################开始筛选RK代码已确认模块
if [ -f RK_Know_License.txt ]
 then
 cat RK_Know_License.txt | while read line
  do
    # echo $line
    int=$(echo $line | awk -F"/" '{print NF-1}')
    if [ "$int"x == "0"x  ];then
        sed -i "/$line/d" ./../$TAGS_NAME/Licensed_suspicion.txt
    else
        line1=$(echo $line | awk 'gsub("/","\\/",$0)')
        sed -i "/$line1/d" ./../$TAGS_NAME/Licensed_suspicion.txt
    fi
  done
fi


##################开始扫描含敏感关键词文件
echo "start search not allow code files...."
   find $ROOT_DIR_TO_SCAN -type f ! \( -path "**.p**" -o -path "*.git*" -o -path "*./external/lldb*" -o -path "*./out*" -o -path "**.html*" -o -path "**.m**" -o -path "*.repo*" -o -path "*.jpg*" -o -path "*.o*" -o -path "*.zip*" -o -path "*License_search.sh*" -o -path "**.s**" -o -path "**.e**" -o -path "**.j**" -o -path "**.a
**" -o -path "**.r**" -o -path "**.y**" -o -path "**.i**" -o -path "**.H**" \) |xargs grep -Er "vp6|realvideo|DTS|AC3|DIVX|wmv|RMVB|HSIC|On2 VP6|SORENSON SPARK|DIV3/DIV4" > ./../$TAGS_NAME/scan_patent_srcFile.txt

#sed -i "/kernel\//d" ./../$TAGS_NAME/scan_patent_srcFile.txt
#sed -i "/u-boot\//d" ./../$TAGS_NAME/scan_patent_srcFile.txt

sed -i "/prebuilts\//d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/external\/wpa_supplicant_8/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/Binary file /d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/libcore\//d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/frameworks\/base\/docs/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/frameworks\/base\/tools/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/frameworks\/base\/api/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/hardware\/broadcom\//d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/art\//d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/bionic\//d" ./../$TAGS_NAME/scan_patent_srcFile.txt
######## ADTS: AAC Audio Data Transport Stream 2 format ###############
sed -i "/ADTS/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
######## AOSP 5.1 define：system/core/include/system/audio.h ###############
sed -i "/AUDIO_FORMAT_AC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/AUDIO_FORMAT_E_AC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/OMX_AUDIO_PARAM_ANDROID_AC3TYPE/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/OMX_AUDIO_PARAM_ANDROID_EAC3TYPE/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/ENCODING_AC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/ENCODING_E_AC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/MEDIA_MIMETYPE_AUDIO_AC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/DTSDeltaLength/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/setAC3Format/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/PTS_DTS_flags/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/MEDIA_MIMETYPE_AUDIO_AAC_ADTS/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/STREAMTYPE_AC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/AAC_ADTS/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/IsSeeminglyValidADTSHeader/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/MEDIA_MIMETYPE_AUDIO_AC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/setupAC3Codec/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/VO_AUDIO_CodingAC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/OMX_AUDIO_CodingAndroidAC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/MEDIA_MIMETYPE_AUDIO_EAC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/dequeueAccessUnitAC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/AMEDIAFORMAT_KEY_IS_ADTS/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/OUTPUT_FORMAT_AAC_ADTS/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/mIsADTS/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/isADTS/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/_MAKE_CODEC_ID/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
#aosp android-5.1.1_r1: frameworks/av/media/libstagefright/mpeg2ts/ESQueue.cpp
sed -i "/ElementaryStreamQueue::AC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/case AC3:/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
#aosp android-5.1.1_r1: frameworks/av/media/libstagefright/mpeg2ts/ATSParser.h
sed -i "/PTS\/DTS/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
#aosp android-5.1.1_r1: frameworks/av/media/libstagefright/ACodec.cpp
sed -i "/AC3 encoding is not supported/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
#delete struct  ElementaryStreamQueue definition which containning keywords: AC3
#sed -i "/struct ElementaryStreamQueue/,/PCM_AUDIO/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/ESQueue.h\:/, /AC3\,/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/STREAMTYPE_LPCM_AC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/setupEAC3Codec/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/OMX_AUDIO_CodingAndroidEAC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/parseAC3SyncFrame/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/IsSeeminglyValidAC3Header/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/parseAC3SyncFrame/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/ALOG/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
#remove all comments
#remove aosp frameworks/av/media/libstagefright/mpeg2ts/ATSParser.cpp
#remove aosp frameworks/av/media/libstagefright/mpeg2ts/MPEG2PSExtractor.cpp
sed -i "/\/\//d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/uint64_t PTS/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/DTS \=/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/DTS \|\=/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/PHY address for EMAC3/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt

#remove uboot/kernel keywords
sed -i "/WDTST_/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/DQ_ODTSel/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/CONFIG_SYS_FM1_DTSEC1_MDIO_ADDR/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/FM1_DTS/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/RGMII DTS/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/DTSEC/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/EMAC3 link up/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/Set phy of EMAC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/EC_AC3_DATA_SELN/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/EMU_DST_DOCK_DAC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/Dock DAC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/EMU10K1_SET_AC3_IEC958/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/HPI_FORMAT_DOLBY_AC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/AUDIO_CODING_TYPE_/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/OXYGEN_PLAY_DAC/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/D_DTS/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/AFMT_AC3/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/AK4117_DTS/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/TLV320DAC/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/DAC33_/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/DA732X_REG_DAC/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/HDPVR_FLAG_AC3_CAP/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/V4L2_CID_MPEG_AUDIO/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/CX2341X_AUDIO_ENCODING_METHOD_AC3/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/CONFIG_DTS/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/BCM_GMAC3/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/EDCA_AC3_CFG/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/SDIO_DTSx_SET_TYPE/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/BRDAC/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/DPOT_RDAC/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/L2DTSLOTSIZE/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/DTSaddress/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/addressDTS/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/DTSLOT/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/RDTSC/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/SDIO_DRIVE_DTS/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/NDFC_CCR_ARAC3/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/EP93XX_DMA_AAC3/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/RIO_DID_IDTSPS1616/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/SCTP_CMD_REPORT_FWDTSN/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/SCTP_CMD_PROCESS_FWDTSN/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/_PARAM_QOS_ADDTS_REQ_INFO/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/BNX2_/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/case AC3_VO/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/B2056_TX_GMBB_IDAC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/X86_FEATURE_LFENCE_RDTSC/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/RDTSC/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/INTERCEPT_RDTSC/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/CONFIG_MVIAC3_2/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/MODULE_PROC_FAMILY/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/SUN4V_DTSB_MISS/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/CHSC_AC3_FORMAT2_CQ_AVAILABLE/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/Microblaze/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/ifpsp060/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/fpsp040/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/DBCR1_IAC3US/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/SPRN_IAC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/DBCR_IAC34MODE/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/DBSR_IAC3/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/DESTDIR/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/__DTS_IMX/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/HCFG_AC3ENABLE/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/AK4113_DTS/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/SC_OSC_DIVXY/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
#remove kernel/document
sed -i '/kernel\/Documentation/d'  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/u\-boot\/README/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/u\-boot\/board\/compulab/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/u\-boot\/board\/gumstix/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/u\-boot\/board\/samsung/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/u\-boot\/board\/ti/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/u\-boot\/board\/freescale/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/u\-boot\/arch\/mips/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/u\-boot\/arch\/powerpc/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/u\-boot\/arch\/mips/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/u\-boot\/arch\/mips/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/u\-boot\/arch\/mips/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/u\-boot\/arch\/powerpc\/include/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/u\-boot\/arch\/arm\/dts/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/u\-boot\/arch\/arm\/include\/asm\/arch\-omap4/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/u\-boot\/arch\/arm\/include\/asm\/arch\-tegra/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/u\-boot\/arch\/arm\/include\/asm\/arch\-exynos/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/u\-boot\/arch\/arm\/include\/asm\/arch\-omap5/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/arch\/powerpc/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/arch\/arm\/mach\-iop13xx/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/arch\/arm\/mach\-mmp/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/arch\/arm\/mach\-shmobile/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/arch\/arm\/mach\-netx/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/arch\/arm\/mach\-exynos/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/arch\/arm\/boot\/dts/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/arch\/arm\/mach\-omap2/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/arch\/arm\/mach\-u300/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/arch\/arm\/mach\-cns3xxx/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/include\/linux\/usb\/audio-v2.h/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/include\/uapi\/sound\/compress_params.h/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/drivers\/staging\/crystalhd\/bc_dts_defs.h/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/drivers\/staging\/keucr/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/drivers\/ata\/libata-core/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/drivers\/mtk_wcn_combo\/drv_wlan/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/tools\/power\/x86\/turbostat/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/drivers\/misc/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/drivers\/net\/ethernet\/atheros\/atl1c/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/drivers\/net\/ethernet\/broadcom\/bnx2x/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/drivers\/net\/wireless\/rt2x/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/drivers\/watchdog\/it87_wdt/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/drivers\/ide\/hpt366/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/drivers\/media\/common\/cx2341x/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/drivers\/phy\/phy\-exynos/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/drivers\/hwmon\/w83795/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/drivers\/mfd/omap/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/drivers\/usb/phy\/phy\-samsung/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/arch\/mips\/include/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/arch\/frv\/include/d" ./../$TAGS_NAME/scan_patent_srcFile.txt

#warning!
sed -i "/kernel\/fs\/nls\/nls_/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/HDMI_AUDIO_AC3/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/HDMI_AUDIO_DTS/d"  ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/HDMI_AUDIO_E_AC3/d"   ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/HDMI_AUDIO_DTS_HD/d"   ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/sound\/soc\/codecs/d"   ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/sound\/pci\/d"   ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/kernel\/arch\/sh\/kernel\/cpu\/sh/d"   ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/system\/core\/libsparse\//d" ./../$TAGS_NAME/scan_patent_srcFile.txt
sed -i "/development\//d" ./../$TAGS_NAME/scan_patent_srcFile.txt

if [ -f ./../$TAGS_NAME/UnRK_manifests.xml ]
 then
 cat ./../$TAGS_NAME/UnRK_manifests.xml | while read line
  do
    # echo $line
    int=$(echo $line | awk -F"/" '{print NF-1}')
    if [ "$int"x == "0"x  ];then
        sed -i "/$line/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
    else
        line1=$(echo $line | awk 'gsub("/","\\/",$0)')
        sed -i "/$line1/d" ./../$TAGS_NAME/scan_patent_srcFile.txt
    fi
  done
fi

################## start search doc and pdf file
################## pre-condition: apt-get install catdoc, poppler-utils
#touch ./../$TAGS_NAME/scan_patent_docFile.txt
#touch ./../$TAGS_NAME/scan_patent_pdfFile.txt
#
#echo "start search doc and pdf file...."
#for i in `find . -type f \( -path "**.doc" -o -path "**.docx" \)`
#do 
# echo $i
# str=`catdoc $i |grep "vp6|realvideo|DTS|AC3|DIVX|wmv|RMVB|HSIC|On2 VP6|SORENSON SPARK"`
# if [ -n "$str" ] ;then
#  echo -n "$i:" >> ./../$TAGS_NAME/scan_patent_docFile.txt
#  echo -e $str >> ./../$TAGS_NAME/scan_patent_docFile.txt
# fi
#done
#
#echo "start search pdf file...."
#for i in `find $ROOT_DIR_TO_SCAN -name "*.pdf"`
#do 
# echo $i
# str=`pdftotext $i - |grep "vp6|realvideo|DTS|AC3|DIVX|wmv|RMVB|HSIC|On2 VP6|SORENSON SPARK"`
# if [ -n "$str" ] ;then
#  echo -n "$i:" >> ./../$TAGS_NAME/scan_patent_pdfFile.txt
#  echo -e $str >> ./../$TAGS_NAME/scan_patent_pdfFile.txt
# fi
#done

#################

#多线程处理
#SEND_THREAD_NUM=13   #设置线程数，在这里所谓的线程，其实就是几乎同时放入后台（使用&）执行的进程。
#tmp_fifofile="/tmp/$$.fifo" # 脚本运行的当前进程ID号作为文件名
#mkfifo "$tmp_fifofile" # 新建一个随机fifo管道文件
#exec 6<>"$tmp_fifofile" # 定义文件描述符6指向这个fifo管道文件
#rm "$tmp_fifofile"
#for ((i=0;i<$SEND_THREAD_NUM;i++));do
#echo # for循环 往 fifo管道文件中写入13个空行
#done >&6

#cat ./../$TAGS_NAME/filelist_Lice1.txt | while read line
#do
# read -u6 # 从文件描述符6中读取行（实际指向fifo管道

# {
# int=$(echo $line | awk -F"/" '{print NF-1}')
# if [ "$int"x == "0"x  ];then
#     sed -i "/$line/d" ./../$TAGS_NAME/Licensed_suspicion.txt
# else
#     line1=$(echo $line | awk 'gsub("/","\\/",$0)')
#     sed -i "/$line1/d" ./../$TAGS_NAME/Licensed_suspicion.txt
# fi
# echo >&6
# } &
#pid=$! #打印最后一个进入后台的子进程id 
#echo $pid
#done

#wait
#exec 6>&-

#rm ./../$TAGS_NAME/filelist_Lice1.txt
###########end
rm ./../$TAGS_NAME/UnRK_manifests.xml
rm ./../$TAGS_NAME/sed* -rf
tar -zcvf ./../Report_Basic_$TAGS_NAME.tar.gz ./../$TAGS_NAME
echo "Search source code for license successfully"
echo "=====report is in ./../Report_Basic_$TAGS_NAME.tar.gz====="
