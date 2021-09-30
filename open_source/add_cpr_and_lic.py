# 普源精电科技技术有限公司版权所有 (2021 - )

# !/usr/bin/env python3
# _*_ coding: utf-8 _*_
"""
源文件名: add_cpr_and_lic.py
功能描述: 为文件添加copyright和license
作者: ShuJie
版本: Ver1.0
创建日期: 2021/9/30 10:17

修改历史:
修改日期：
版本号：
修改人：
修改内容：
"""

import os
import codecs
import chardet
import time
from tqdm import tqdm

USE_DEBUG = 1      # 调试信息


# 判断文件编码
def checkFileEncode(src):
    with open(src, 'rb') as f:
        return chardet.detect(f.read()).get('encoding')


def ReadFile(filePath, encoding=None):
    if encoding is not None:
        with codecs.open(filePath, "r", encoding) as f:
            return f.read()


def WriteFile(filePath, u, encoding="gbk"):
    with codecs.open(filePath, "wb") as f:
        f.write(u.encode(encoding, errors="ignore"))


def UTF8_2_GBK(src, dst=None, encode=None):
    """

    :param encode:   源文件编码
    :param src:      源目录list
    :param dst:      输出目录list
    :return:         无
    """
    if dst is None:    # 默认输出位置为源位置
        dst = src
    if USE_DEBUG:
        print('转换开始...')
    for i in tqdm(range(len(src))):
        content = ReadFile(src[i], encoding=encode)
        WriteFile(dst[i], content, encoding="gb18030")


# 查找指定类型文件
def findFileInSuffix(suffix, rootDir=None, ignore=None):
    """

    :param rootDir: 指定目录
    :param suffix:  指定类型, 例如txt, c, cpp
    :param ignore:  忽略的目录
    :return:        结果路径list
    """
    if rootDir is None:        # 路径为空, 默认当前路径
        rootDir = './'
        rootName = 'curr'      # 当前路径
    else:
        rootName = rootDir.split('\\')[-1]
    if ignore is None:
        ignore = ['.git', 'venv', '.repo', '.idea']    # 默认忽略git, repo
    else:
        ignore.append('.git')
        ignore.append('.repo')
    if USE_DEBUG:
        print('当前搜索路径:', rootDir)
    result = []

    txtName = rootName + '_file_in_suffix.txt'      # 本函数输出记录文件
    if os.path.exists(txtName):
        if time.time() - os.stat(txtName).st_mtime > 30 * 60:              # 修改时间大于30分钟才会去覆盖文件
            # 修改时间大于30分钟, 覆盖文件
            if USE_DEBUG:
                print('覆盖文件:', txtName)
            with open(txtName, 'w') as f:
                for name in suffix:
                    suffixName = "." + name
                    walk = os.walk(rootDir)
                    for root, dirs, files in walk:
                        if len(files) < 1:
                            continue
                        for i in ignore:
                            if i in dirs:
                                dirs.remove(i)
                        for file in files:
                            if USE_DEBUG:
                                print('reading', os.path.join(root, file))
                            fileName, serSuffixName = os.path.splitext(file)
                            if serSuffixName == suffixName:
                                result.append(os.path.join(root, file))
                                f.write(os.path.join(root, file) + '\n')
        else:
            # 修改时间不足, 直接读文件
            if USE_DEBUG:
                print('读取文件:', txtName)
            with open(txtName, 'r') as f:
                line = f.readlines()
                for i in line:
                    result.append(i)
    # 输出文件不存在, 创建
    else:
        with open(txtName, 'w') as f:
            for name in suffix:
                suffixName = "." + name
                walk = os.walk(rootDir)
                for root, dirs, files in walk:
                    if len(files) < 1:
                        continue
                    for i in ignore:
                        if i in dirs:
                            dirs.remove(i)
                    for file in files:
                        if USE_DEBUG:
                            print('reading', os.path.join(root, file))
                        fileName, serSuffixName = os.path.splitext(file)
                        if serSuffixName == suffixName:
                            result.append(os.path.join(root, file))
                            f.write(os.path.join(root, file) + '\n')
    return result


# 查找指定字符串
def findFileInString(fileList, *string):

    """

    :param fileList:  查找的文件目录
    :param string:    指定的字符串list
    :return:          路径及字符串内容list
    """
    result = []
    for i in tqdm(range(len(fileList))):
        code = checkFileEncode(fileList[i])
        with open(fileList[i], 'r', encoding=code, errors='ignore') as f:    # 忽略中文带来的影响
            for line in f.readlines():
                for j in string:
                    if j in line:
                        result.append(fileList[i] + ':' + line)
    return result