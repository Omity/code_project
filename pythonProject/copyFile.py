#!/usr/bin/env python
# _*_ coding: utf-8 _*_
"""
源文件名：copyFile.py
功能描述：用于copy文件，linux-->windows
作者：ShuJie
版本：V0.1
创建时间：2021/12/9 20:49

修改历史：
修改时间：
版本号：
修改人：
修改内容：

备注： 本程序需要linux端满足可以上网, Windows端能ping通linux，且linux端安装了openssh-server和openssh-client
      python版本使用python3.9
"""
# 库引入
import os
import os.path
import re

import paramiko
# 变量声明
SOURCE_FILE_NAME = "August_daily_code"
LINUX_IP = "192.168.171.128"
LINUX_LOGIN_USER = "shujie"
LINUX_LOGIN_CODE = "0"
LINUX_FIND_SOURCE_CMD = 'find ~ -type d -name ' + SOURCE_FILE_NAME      # 默认文件建立在home目录下, 如果不是需调整
WINDOWS_LOCAL_PATH = 'C:\\Study_log\\sdk'

# 函数定义


def get_file_path(path1, file_list, dir_list):   # 递归
    list_1 = os.listdir(path1)  # 当前目录下有哪些文件和子目录
    for i in range(len(list_1)):
        list_1[i] = os.path.join(path1, list_1[i])  # 将目录名和文件或文件名组成一个路径
        if os.path.isfile(list_1[i]):   # 如果该条路径下为一个文件
            file_list.append(list_1[i])
        else:
            dir_list.append(list_1[i])
            get_file_path(list_1[i], file_list, dir_list)


def mk_dir(list4, path2):
    list4.sort(key=lambda x: x.count('\\'))
    files_name = list4[0]
    files_name = files_name[files_name.rfind('\\')+1:]
    path2 = path2+'\\'+files_name

    for i in range(len(list4)):
        str_1 = list4[i]
        str_2 = path2+str_1[str_1.find(files_name)+len(files_name):]
        list4[i] = str_2
        os.mkdir(list4[i])


def handle_list2(list4, list2, path2, key_word):
    files_name = list4[0]
    files_name = files_name[files_name.rfind('\\') + 1:]
    path2 = path2+'\\'+files_name
    list2.sort(key=lambda x: x.count('\\'))

    for i in range(len(list2)):
        if key_word == os.path.splitext(list2[i])[1]:
            str_1 = list2[i]

            str_2 = path2+str_1[str_1.find(files_name)+len(files_name):]

            print('正在复制文件%s到%s当中' % (list2[i], str_2))
            with open(list2[i], 'rb') as f:
                content = f.read()
            with open(str_2, 'wb') as f:
                f.write(content)


def send_linux_cmd(sshObj, cmd):
    """

    :param sshObj:  SSH实例
    :param cmd:     linux命令
    :return:        命令返回结果
    """
    stdin, stdout, stderr = sshObj.exec_command(cmd)
    result = stdout.read()
    if not result:
        result = stderr.read()

    return result.decode()


def get_source_code_from_linux():
    # 获取SSHClient实例
    client = paramiko.SSHClient()
    client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    # 连接SSH服务端

    client.connect(LINUX_IP, username=LINUX_LOGIN_USER, password=LINUX_LOGIN_CODE)

    # 获取指定目录的linux绝对路径
    # stdin, stdout, stderr = client.exec_command(LINUX_FIND_SOURCE_CMD)
    source_path = send_linux_cmd(client, LINUX_FIND_SOURCE_CMD).replace('\n', '')
    print(source_path)
    if len(source_path.split()) > 1:   # 如果大于1说明找到的文件不止一个
        print('查询的文件目录不唯一，请确认文件路径是否是你想要的 ^_^')
        return -1

    search_file_cmd = " ".join(["find", source_path])
    search_empty_file_cmd = " ".join(["find", source_path, "-empty"])
    return_file_list = send_linux_cmd(client, search_file_cmd).split()
    return_empty_file_list = send_linux_cmd(client, search_empty_file_cmd).split()
    for i in range(len(return_empty_file_list)):
        return_empty_file_list[i] = re.sub(source_path, '', return_empty_file_list[i])
    for i in return_file_list:
        print(i)
    for i in return_empty_file_list:
        print(i)
    # 查找指定目录下的所有文件

    # 获取Transport实例
    # tran = client.get_transport()
    # # 获取SFTP实例
    # sftp = paramiko.SFTPClient.from_transport(tran)
    #
    # remotePath = '/home/GJ/Desktop/test/test.c'
    # localPath = 'E:\\test\\test.c'
    #
    # sftp.get(remotePath, localPath)
    client.close()

    # print(result.decode())


get_source_code_from_linux()
