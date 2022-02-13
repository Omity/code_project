#!/usr/bin/env python
# _*_ coding: utf-8 _*_
"""
源文件名：ui_to_py.py
功能描述：
作者：ShuJie
版本：V0.1
创建时间：2022/2/12 22:49

修改历史：
修改时间：
版本号：
修改人：
修改内容：
"""

# 库引入
import os
import os.path

# 变量声明
directory = './'

# 函数定义

# 类定义

# UI文件所在的路径


# 列出目录下的所有UI文件
def listUiFile():
    a_list = []
    files = os.listdir(directory)
    for filename in files:
        if os.path.splitext(filename)[1] == '.ui':
            a_list.append(filename)
    return a_list


# 把扩展名为.ui的文件改成扩展名为.py的文件
def transPyFile(filename):
    return os.path.splitext(filename)[0] + '.py'


# 调用系统命令把UI文件转换成Python文件
def runMain():
    a_list = listUiFile()
    for uiFile in a_list:
        pyFile = transPyFile(uiFile)
        cmd = 'pyuic5 -o {pyfile} {uifile}'.format(pyfile=pyFile, uifile=uiFile)
        os.system(cmd)


if __name__ == "__main__":
    runMain()
