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
import os
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
    def de_info(string, end='\n'):
        """
        测试类信息
        :param end:
        :param string:
        :return:
        """
        if USE_DEBUG:
            print(string, end=end)

    @staticmethod
    def pr_info(string, end='\n'):
        """
        通知类信息
        :param end:
        :param string:
        :return:
        """
        print(string, end=end)

    @staticmethod
    def er_info(string, end='\n'):
        """
        错误类信息
        :param end:
        :param string:
        :return:
        """
        print(string, end=end)

    @staticmethod
    def processBar():
        pass

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
                self.client.close()
                self._is_connected = False
            except paramiko.SSHException:
                self.er_info('there was any other error connecting or establishing an SSH session')
                self.client.close()
                self._is_connected = False
            except socket.error:
                self.er_info('A socket error occurred while connecting')
                self.client.close()
                self._is_connected = False
        else:
            self.er_info('IP or User or Passwd is none, check it!')
            self._is_connected = False
        func(self._is_connected)

    def checkLicenseAndCopyright(self, func, path):
        """
        检测license和copyright线程
        :param path: windows路径
        :param func:
        :return:
        """
        temp_thread = threading.Thread(target=self._checkLicenseAndCopyright, args=(func, path))
        temp_thread.setDaemon(True)
        temp_thread.start()

    def _checkLicenseAndCopyright(self, func, path):
        """
        检测文件是否可用
        :param func:
        :param path:
        :return:
        """
        self._is_checking = False
        if path == '':
            self.er_info('Windows path is none,check it!')
        if os.path.exists(path) and os.path.isdir(path):
            self._is_checking = True
            self.de_info('Windows path is legal, check done...')
        func(self._is_checking)

    def sendLinuxCmd(self, cmd):
        """
        :param cmd:     linux命令
        :return:        命令返回结果
        """
        stdin, stdout, stderr = self.client.exec_command(cmd)
        result = stdout.read()
        if not result:
            result = stderr.read()
        try:
            return result.decode()
        except UnicodeDecodeError:
            return result.decode('gb18030', 'ignore')

class Pscp(object):
    """
        pscp.exe
        用法: pscp [选项] [用户名@]主机:源 目标
          pscp [选项] 源 [其他源...] [用户名@]主机:目标
          pscp [选项] -ls [用户名@]主机:指定文件
            选项:
              -V        显示版本信息后退出
              -pgpfp    显示 PGP 密钥指纹后退出
              -p        保留文件属性
              -q        安静模式，不显示状态信息
              -r        递归拷贝目录
              -v        显示详细信息
              -load 会话名  载入保存的会话信息
              -P 端口   连接指定的端口
              -l 用户名 使用指定的用户名连接
              -pw 密码  使用指定的密码登录
              -1 -2     强制使用 SSH 协议版本
              -4 -6     强制使用 IPv4 或 IPv6 版本
              -C        允许压缩
              -i 密钥   认证使用的密钥文件
              -noagent  禁用 Pageant 认证代理
              -agent    启用 Pageant 认证代理
              -hostkey aa:bb:cc:...
                        手动指定主机密钥(可能重复)
              -batch    禁止所有交互提示
              -proxycmd 命令
                        使用 '命令' 作为本地代理
              -unsafe   允许服务端通配符(危险操作)
              -sftp     强制使用 SFTP 协议
              -scp      强制使用 SCP 协议
              -sshlog 文件
              -sshrawlog 文件 记录协议详细日志到指定文件
        """

    def __init__(self, pscp=None):
        self.pscp = pscp

    def windowToLinuxFile(self, window_path, Linux_path, Linux_ip, username, password):
        cmd = self.pscp + f'-pw {password} {window_path} {username}@{Linux_ip}:{Linux_path}'
        return cmd
        # os.system(cmd)

    def windowToLinuxDir(self, window_path, Linux_path, Linux_ip, username, password):
        cmd = self.pscp + f'-pw {password} -r {window_path} {username}@{Linux_ip}:{Linux_path}'
        # os.system(cmd)
        return cmd

    def linuxToWindowFile(self, Linux_path, window_path, Linux_ip, username, password):
        cmd = self.pscp + f' -pw {password} {username}@{Linux_ip}:{Linux_path} {window_path}'
        # os.system(cmd)
        return cmd

    def linuxToWindowDir(self, Linux_path, window_path, Linux_ip, username, password):
        cmd = self.pscp + f' -r -pw {password} {username}@{Linux_ip}:{Linux_path} {window_path}'
        # os.system(cmd)
        return cmd

