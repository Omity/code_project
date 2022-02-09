#!/bin/bash

################################################################################
# SEARCH LICENSE FOR CODE
# 		Author: sn03955@rigol.com
#		Data: 2021-9-29
################################################################################
# 		History:
#       V1.0:
#
################################################################################

# 版本号
echo "version 1.0"

# 输出文件名
TAGS_NAME=License_$(date +%Y_%m_%d_%H_%M)
# 指定扫描路径 紧跟sh文件后,即在shell里输入license_scan_v1.0.sh 路径
# 如果不跟路径则默认为当前路径
if [ "$1"x == ""x ];then
	ROOT_DIR_TO_SCAN=./
else
	ROOT_DIR_TO_SCAN=$1
fi
echo "ROOT_DIR_TO_SCAN is $ROOT_DIR_TO_SCAN"

# 创建输出文件夹
if [ -d ./License_*  ];then
	echo "start this part"
	rm -r ./License_*
fi
mkdir ./$TAGS_NAME


# 忽略的文件名
#FILE_IGNORE="*.git*"

#find $ROOT_DIR_TO_SCAN -type f ! \( -path "**.png*" -o -path $FILE_IGNORE \) -name "HEAD"> ./le.txt

echo "开始扫描目录文件夹...."
# 第一步:扫描目录下的所有文件夹
find $ROOT_DIR_TO_SCAN -maxdepth 1 -type d > ./$TAGS_NAME/scan_all_dir.txt

# 第二步:开始分析每个目录下的文件夹
cat ./$TAGS_NAME/scan_all_dir.txt | while read line
	do
		echo $line
		if [ ];then
	done

	



