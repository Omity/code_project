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

      pscp下载:https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html
"""
# 库引入
import os
import os.path
import re
import socket
import paramiko
import datetime
from tqdm import tqdm
# 变量声明
USE_DEBUG = 0
USE_LINUX_FIND = 0                                      # 使用find命令获取工程的linux绝对路径
SOURCE_FILE_NAME = "sousa_os"                            # 工程目录
LINUX_IP = "192.168.136.137"                            # 虚拟机ip
LINUX_LOGIN_USER = "GJ"                                 # 当前登录用户名
LINUX_LOGIN_CODE = "0"                                  # 登录密码
LINUX_FIND_SOURCE_CMD = 'find ~ -type d -name ' + SOURCE_FILE_NAME      # 默认文件建立在home目录下, 如果不是需调整
LINUX_SOURCE_PATH = '/home/GJ/Desktop/' + SOURCE_FILE_NAME + '/out'
WINDOWS_LOCAL_PATH = 'E:\\sousa_os'
LOG_FILE = 'copy_log.txt'
TEST_LOG = 'source_list.txt'
PSCP_PATH = 'E:\\tools\\pscp.exe'

# 函数实现

def de_info(string):
    if USE_DEBUG:
        print(string)

def pr_info(string):
    print(f'\033[0;32m{string}\033[0m')

def er_info(string):
    print(f'\033[0;31m{string}\033[0m')

def make_log(txt, string):
    with open(txt, 'a') as f:
        get_time = "[" + datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S') + "]"
        f.write(" ".join([get_time, string, '\n']))

# 类实现
class GetSourceFromLinux:

    def __init__(self):
        # 获取SSHClient实例
        self.client = paramiko.SSHClient()
        self.client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        self.local_path = WINDOWS_LOCAL_PATH

    # 连接SSH服务端
    def connect(self):
        try:
            self.client.connect(LINUX_IP, username=LINUX_LOGIN_USER, password=LINUX_LOGIN_CODE)
            pr_info('尝试建立SSH连接....')
        except paramiko.ssh_exception.AuthenticationException:
            er_info('authentication failed')
            self.client.close()
            return
        except paramiko.SSHException:
            er_info('there was any other error connecting or establishing an SSH session')
            self.client.close()
            return
        except socket.error:
            er_info('A socket error occurred while connecting')
            self.client.close()
            return
        pr_info('连接linux成功!')

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
        # pr_info('复制文件成功!')

    @staticmethod
    def pscp_copy_file(source, locate):
        # for i in tqdm(range(len(source))):
        cmd = PSCP_PATH + f' -r -pw {LINUX_LOGIN_CODE} {LINUX_LOGIN_USER}@{LINUX_IP}:{source} {locate}'
        os.system(cmd)

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
        self.client.close()
        # 复制文件夹
        self.pscp_copy_file(return_file_list, windows_path_list)
        # self.copy_file(return_file_list, windows_path_list)

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

    def __init__(self, path):
        self.pscp_path = path

    def windowToLinuxFile(self, window_path, Linux_path, Linux_ip, username, password):
        cmd = self.pscp_path + f'-pw {password} {window_path} {username}@{Linux_ip}:{Linux_path}'
        os.system(cmd)

    def windowToLinuxDir(self, window_path, Linux_path, Linux_ip, username, password):
        cmd = self.pscp_path + f'-pw {password} -r {window_path} {username}@{Linux_ip}:{Linux_path}'
        os.system(cmd)

    def linuxToWindowFile(self, Linux_path, window_path, Linux_ip, username, password):
        cmd = self.pscp_path + f' -pw {password} {username}@{Linux_ip}:{Linux_path} {window_path}'
        os.system(cmd)

    def linuxToWindowDir(self, Linux_path, window_path, Linux_ip, username, password):
        cmd = self.pscp_path + f' -r -pw {password} {username}@{Linux_ip}:{Linux_path} {window_path}'
        os.system(cmd)

class CopyFile(object):

    def __init__(self, path, linux_user, linux_pd, linux_ip):
        self.user = linux_user
        self.passwd = linux_pd
        self.ip = linux_ip
        self.pscp = Pscp(path)

    def copyFile(self, source, locate):
        de_info(f'In this part, start to copy files from linux: {self.ip}')
        self.pscp.linuxToWindowDir(source, locate, self.ip, self.user, self.passwd)


cp = CopyFile(PSCP_PATH, LINUX_LOGIN_USER, LINUX_LOGIN_CODE, LINUX_IP)
cp.copyFile(LINUX_SOURCE_PATH, WINDOWS_LOCAL_PATH)
