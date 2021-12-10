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
from tqdm import tqdm
# 变量声明
USE_DEBUG = 0
USE_LINUX_FIND = 0                                      # 使用find命令获取工程的linux绝对路径
SOURCE_FILE_NAME = "sousa_os"                            # 工程目录
LINUX_IP = "192.168.136.137"                            # 虚拟机ip
LINUX_LOGIN_USER = "GJ"                                 # 当前登录用户名
LINUX_LOGIN_CODE = "0"                                  # 登录密码
LINUX_FIND_SOURCE_CMD = 'find ~ -type d -name ' + SOURCE_FILE_NAME      # 默认文件建立在home目录下, 如果不是需调整
LINUX_SOURCE_PATH = '/home/GJ/Desktop/' + SOURCE_FILE_NAME
WINDOWS_LOCAL_PATH = 'E:\\sousa_os'

# 函数实现

def de_info(string):
    if USE_DEBUG:
        print(string)

def pr_info(string):
    print(f'\033[0;32m{string}\033[0m')


class GetSourceFromLinux:

    def __init__(self):
        # 获取SSHClient实例
        self.client = paramiko.SSHClient()
        self.client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        self.local_path = WINDOWS_LOCAL_PATH
        # 连接SSH服务端
        self.client.connect(LINUX_IP, username=LINUX_LOGIN_USER, password=LINUX_LOGIN_CODE)

    def send_linux_cmd(self, cmd):

        """
        :param cmd:     linux命令
        :return:        命令返回结果
        """
        stdin, stdout, stderr = self.client.exec_command(cmd)
        result = stdout.read()
        if not result:
            result = stderr.read()

        return result.decode()

    @staticmethod
    def mk_dir(tar_list):
        for i in tqdm(range(len(tar_list))):
            if not os.path.exists(tar_list[i]):
                os.makedirs(tar_list[i])

    def get_source_code_from_linux(self):
        """
        向linux获取工程文件
        """

        if USE_LINUX_FIND:
            # 获取指定目录的linux绝对路径
            source_path = self.send_linux_cmd(LINUX_FIND_SOURCE_CMD).replace('\n', '')
            if len(source_path.split()) > 1:   # 如果大于1说明找到的文件不止一个
                print('查询的文件目录不唯一，请确认文件路径是否是你想要的 ^_^')
                return -1
        else:
            source_path = LINUX_SOURCE_PATH

        search_dir_cmd = " ".join(["find", source_path, "-type d"])
        search_file_cmd = " ".join(["find", source_path, "-type f"])
        pr_info('get source path list ....')
        return_file_list = self.send_linux_cmd(search_file_cmd).split()
        windows_path_list = ['0' for x in range(0, len(return_file_list))]
        pr_info('get source dir list ....')
        return_dir_list = self.send_linux_cmd(search_dir_cmd).split()
        # 更换成windows端的路径
        for i in range(len(return_dir_list)):
            return_dir_list[i] = self.local_path + re.sub(source_path, '', return_dir_list[i]).replace('/', '\\')
        # 保留文件路径以供Windows端使用
        for i in range(len(return_file_list)):
            windows_path_list[i] = self.local_path + re.sub(source_path, '', return_file_list[i]).replace('/', '\\')
        for i in return_file_list:
            de_info(i)
        for i in return_dir_list:
            de_info(i)
        # 创建空文件目录
        pr_info('创建文件目录中...')
        self.mk_dir(return_dir_list)
        pr_info('创建目录成功!')
        # 复制文件夹
        pr_info('复制文件中....')
        # 获取Transport实例
        tran = self.client.get_transport()
        # 获取SFTP实例
        sftp = paramiko.SFTPClient.from_transport(tran)
        for i in tqdm(range(len(return_file_list))):
            sftp.get(return_file_list[i], windows_path_list[i])
        pr_info('复制成功!')
        self.client.close()

client = GetSourceFromLinux()
client.get_source_code_from_linux()

