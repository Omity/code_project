#!/usr/bin/env python
# _*_ coding: utf-8 _*_
"""
源文件名：copyFile.py
功能描述：用于copy文件，linux-->windows
作者：ShuJie
版本：V0.1
创建时间：2021/12/9 20:49

修改历史： 修改历史1
修改时间： 2021年12月11日
版本号：   V0.2
修改人：   舒杰
修改内容： 修改linux路径，取消自动获取，采用直接给出的绝对路径

备注： 本程序需要linux端满足可以上网, Windows端能ping通linux，且linux端安装了openssh-server和openssh-client
      python版本使用python3.9
"""
# 库引入
import datetime
import os
import os.path
import re
import paramiko
from tqdm import tqdm
# 变量声明
USE_DEBUG = 1
USE_LINUX_FIND = 0                                      # 使用find命令获取工程的linux绝对路径
SOURCE_FILE_NAME = "test"                            # 工程目录
LINUX_IP = "192.168.171.128"                            # 虚拟机ip
LINUX_LOGIN_USER = "shujie"                                 # 当前登录用户名
LINUX_LOGIN_CODE = "0"                                  # 登录密码
LINUX_FIND_SOURCE_CMD = " ".join(['find ~ -type d -name', SOURCE_FILE_NAME])      # 默认文件建立在home目录下, 如果不是需调整
LINUX_SOURCE_PATH = "/".join(['/home', LINUX_LOGIN_USER, SOURCE_FILE_NAME])       # 默认在home目录下
WINDOWS_LOCAL_PATH = 'C:\\Work_Rigol\\test'
LOG_FILE = 'copyLog.txt'

# 函数实现

def de_info(string):
    if USE_DEBUG:
        print(string)

def pr_info(string):
    print(f'\033[0;32m{string}\033[0m')

def er_info(string):
    print(f'\033[0;31m{string}\033[0m')

def make_log(txt, string):
    get_time = " ".join(["[", datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S'), "]"])
    with open(txt, 'a') as f:
        f.write(" ".join([get_time, string, '\n']))

class GetSourceFromLinux:

    def __init__(self):
        # 获取SSHClient实例
        self.client = paramiko.SSHClient()
        self.client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        self.local_path = WINDOWS_LOCAL_PATH
        # 连接SSH服务端
        self.client.connect(LINUX_IP, username=LINUX_LOGIN_USER, password=LINUX_LOGIN_CODE)

    def connect(self):
        try:
            # 连接SSH服务端
            self.client.connect(LINUX_IP, username=LINUX_LOGIN_USER, password=LINUX_LOGIN_CODE)
            pr_info('尝试连接linux...')
        except paramiko.SSHException:
            er_info('failed in SSH2 protocol negotiation or logic errors')
            self.client.close()
            return
        pr_info('连接成功!')

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
        """

        :param tar_list: 目录列表
        :return:         无
        """
        count = 0      # 计数器
        pr_info('创建文件目录中...')
        for i in tqdm(range(len(tar_list))):
            count += 1
            if not os.path.exists(tar_list[i]):
                try:
                    os.makedirs(tar_list[i])
                except OSError:
                    make_log(LOG_FILE, f'create file path {tar_list[i]} failed')
                    er_info(f'create {tar_list[i]} failed!')
                    count -= 1
        if count == len(tar_list):
            pr_info('创建目录成功!')
        else:
            er_info('因为某些原因, 缺失了一部分文件目录, 请检查log!')

    def copy_file(self, source, locate):
        """

        :param source: 复制源目录
        :param locate: 复制目标目录
        :return:       无
        """

        pr_info('复制文件中....')
        # 获取Transport实例
        tran = self.client.get_transport()
        # 获取SFTP实例
        sftp = paramiko.SFTPClient.from_transport(tran)
        for i in tqdm(range(len(source))):
            try:
                sftp.get(source[i], locate[i])
            except IOError:
                make_log(LOG_FILE, f'copy file {source[i]} failed')
                er_info(f'copy file {source[i]} failed!')
                return
        pr_info('复制文件成功!')

    def get_source_code_from_linux(self):
        """
        向linux获取工程文件
        """

        if USE_LINUX_FIND:
            # 获取指定目录的linux绝对路径
            source_path = self.send_linux_cmd(LINUX_FIND_SOURCE_CMD).replace('\n', '')
            if len(source_path.split()) > 1:   # 如果大于1说明找到的文件不止一个
                make_log(LOG_FILE, '查询的文件目录不唯一，请确认文件路径是否是你想要的 ^_^')
                er_info('查询的文件目录不唯一，请确认文件路径是否是你想要的 ^_^')
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
        self.mk_dir(return_dir_list)
        # 复制文件夹
        self.copy_file(return_file_list, windows_path_list)

        self.client.close()


client = GetSourceFromLinux()
client.connect()
client.get_source_code_from_linux()
