# !/usr/bin/env python3
# _*_ coding: utf-8 _*_
"""
源文件名: BtnFunc.py
功能描述:  主要实现按键的一些功能。测试模块,后期会嵌入其他文件中
作者: ShuJie
版本: Ver1.0
创建日期: 2021/12/18 15:32

修改历史:
修改日期：
版本号：
修改人：
修改内容：
"""

# 导入的包
import paramiko
import socket
import threading
# 宏定义
USE_DEBUG = 1
# 版本号

# 函数实现

# 类实现

class OpenSourceHelper(object):

    def __init__(self, ip=None, user=None, pd=None):
        self.ip = ip
        self.user = user
        self.pd = pd

        # 获取SSHClient实例
        self.client = paramiko.SSHClient()
        self.client.set_missing_host_key_policy(paramiko.AutoAddPolicy())

    @staticmethod
    def de_info(string):
        """
        测试类信息
        :param string:
        :return:
        """
        if USE_DEBUG:
            print(string)

    @staticmethod
    def pr_info(string):
        """
        通知类信息
        :param string:
        :return:
        """
        print(string)

    @staticmethod
    def er_info(string):
        """
        错误类信息
        :param string:
        :return:
        """
        print(string)

    def connectLinux(self, func):
        """
        连接linux线程
        :param func:
        :return:
        """
        temp_thread = threading.Thread(target=self._connectLinux, args=(func, ))
        temp_thread.setDaemon(True)
        temp_thread.start()

    def _connectLinux(self, func):
        """
        测试linux连接, 由于复制走的另一流程, 所以不循环检测连接状态,只检测一次
        :param func: 回调函数
        :return:
        """
        self._is_connected = True
        self.de_info(f'get ip:{self.ip}')
        self.de_info(f'get user:{self.user}')
        self.de_info(f'get pd:{self.pd}')
        if self.ip != '' and self.user != '' and self.pd != '':
            try:
                self.pr_info('Try to build SSH connect ...')
                self.client.connect(self.ip, username=self.user, password=self.pd)
            except paramiko.ssh_exception.AuthenticationException:
                self.er_info('authentication failed')
                self._is_connected = False
            except paramiko.SSHException:
                self.er_info('there was any other error connecting or establishing an SSH session')
                self._is_connected = False
            except socket.error:
                self.er_info('A socket error occurred while connecting')
                self._is_connected = False
        else:
            self.er_info('IP or User or Passwd is none, check it!')
            self._is_connected = False
        self.client.close()
        func(self._is_connected)
