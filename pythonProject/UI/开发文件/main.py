# !/usr/bin/env python3
# _*_ coding: utf-8 _*_
"""
源文件名: main.py
功能描述: 程序主函数
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
import tkinter as tk
from MainFrm import MainFrame
import tkinter.font as tkf
import tkinter.ttk as ttk
from BtnFuc import OpenSourceHelper, Pscp
import subprocess as sp
# 宏定义
USE_DEBUG = 1
# 版本号
VERSION = 'V1.0.0'
# 函数实现
def de_info(string):
    """
    测试类信息
    :param string:
    :return:
    """
    if USE_DEBUG:
        print(string)

def pr_info(string):
    """
    通知类信息
    :param string:
    :return:
    """
    print(string)

def er_info(string):
    """
    错误类信息
    :param string:
    :return:
    """
    print(string)
# 类实现

class MainOpenSource(MainFrame):

    def __init__(self, master=None):
        super(MainOpenSource, self).__init__(master)

        self.root = master
        self._is_connected = False
        # 更新版本,目前方法比较笨,后续会重新思考
        self.version = VERSION
        self.pscp_path = '..\\tools\\pscp.exe'
        self.updateVer()

    def copyClick(self):
        """
        copy按键点击区
        :return:
        """
        if self.frm_menu.copy_btn['text'] == 'copy':
            # noinspection PyBroadException
            try:
                self.frm_menu.copy_btn['text'] = 'exit'
                self.frm_menu.copy_btn['bg'] = 'red'
                temp_ip = self.frm_menu.ip_entry.get()
                temp_user = self.frm_menu.user_entry.get()
                temp_passwd = self.frm_menu.pd_entry.get()
                self.ser = OpenSourceHelper(ip=temp_ip,
                                            user=temp_user,
                                            pd=temp_passwd)
                self.ser.connectLinux(self.startCopyFile)
            except Exception as e:
                self.ser.er_info(e)
                self.ser.er_info('Open linux failed!')
        elif self.frm_menu.copy_btn['text'] == 'exit':
            self._disconnect()

    def _connect(self):
        """
        连接成功后的设置
        :return:
        """
        pr_info('connect linux successfully')
        self._is_connected = True
        self.frm_menu.copy_btn['text'] = 'exit'
        self.frm_menu.copy_btn['bg'] = 'red'
        self.frm_menu.check_btn['state'] = 'disabled'
        self.frm_menu.sdk_btn['state'] = 'disabled'
        self.frm_menu.choose_path_btn['state'] = 'disabled'
        self.frm_menu.connect_status['text'] = '已连接'
        self.frm_menu.connect_status['fg'] = 'green'
        self.frm_menu.ip_entry['state'] = 'disabled'
        self.frm_menu.user_entry['state'] = 'disabled'
        self.frm_menu.pd_entry['state'] = 'disabled'
        self.frm_menu.windows_path_entry['state'] = 'disabled'
        self.frm_menu.linux_path_entry['state'] = 'disabled'

    def _disconnect(self):
        """
        关闭连接后的设置
        :return:
        """
        self._is_connected = False
        self.frm_menu.copy_btn['text'] = 'copy'
        self.frm_menu.copy_btn['bg'] = 'lightblue'
        self.frm_menu.connect_status['text'] = '未连接'
        self.frm_menu.connect_status['fg'] = 'red'
        self.frm_menu.check_btn['state'] = 'normal'
        self.frm_menu.sdk_btn['state'] = 'normal'
        self.frm_menu.choose_path_btn['state'] = 'normal'
        self.frm_menu.ip_entry['state'] = 'normal'
        self.frm_menu.user_entry['state'] = 'normal'
        self.frm_menu.pd_entry['state'] = 'normal'
        self.frm_menu.windows_path_entry['state'] = 'normal'
        self.frm_menu.linux_path_entry['state'] = 'normal'
        pr_info('exit successfully')

    def startCopyFile(self, is_connected):
        """
        复制函数
        :param is_connected: 连接状态
        :return:
        """
        if is_connected:    # 说明连接一次成功,这个判断仅会运行一次
            self._connect()
            self._setCurrLogin()  # 记录一次登录信息
            self.usePscp()
            pr_info('exit successfully')
        else:
            er_info('Open linux failed!')
            self._disconnect()

    def usePscp(self):
        """
        调用pscp进行复制
        :return:
        """
        try:
            temp_win_path = self.frm_menu.windows_path_entry.get()
            temp_lin_path = self.frm_menu.linux_path_entry.get()
            if temp_win_path != '' and temp_lin_path != '':
                temp_cmd = ' '.join(['find', '/'.join(temp_lin_path.split('/')[:-1]),
                                     '-maxdepth 1 -type d -name', temp_lin_path.split('/')[-1]])
                de_info(f'send command: {temp_cmd}')
                if self.ser.sendLinuxCmd(temp_cmd) != '':  # 返回值不为空
                    "这里保留一步,后续加入验证是否有不存在的文件,因为这会导致pscp的复制错误从而导致整个复制错乱"
                    self.pscp = Pscp(self.pscp_path)
                    self.pscpProcess(self.pscp.linuxToWindowDir(temp_lin_path, temp_win_path,
                                     self.frm_menu.ip_entry.get(),
                                     self.frm_menu.user_entry.get(),
                                     self.frm_menu.pd_entry.get()))

                    pr_info('copy successfully')
                else:
                    de_info(f'{temp_lin_path} not exist, check it!')
            else:
                de_info('Windows path or Linux path is none, check it!')
        except Exception as e:
            er_info(e)
        self._disconnect()

    def pscpProcess(self, cmd):
        """
        pscp进程,以subprocess运行,保证能捕获输出
        :param cmd:
        :return:
        """
        try:
            self.p = sp.Popen(cmd, stderr=sp.PIPE, stdin=sp.PIPE, stdout=sp.PIPE)
            # 保留,因为pscp好像第一次使用时需要选择yes
            # p.stdin.write('y\r\n'.encode('GBK'))
            # p.stdin.flush()
            for i in self.p.stdout.readlines():
                if self._is_connected:
                    pr_info(i.decode('GBK'))
                else:
                    self.p.kill()
        except Exception as e:
            er_info(e)

    def checkClick(self):
        pr_info('new test')


if __name__ == '__main__':
    root = tk.Tk()
    root.columnconfigure(0, weight=1)
    root.rowconfigure(0, weight=1)
    root.geometry()
    root.title("Open Source Tools")

    monaco_font = tkf.Font(family="Monaco", size=16)
    root.option_add("*TCombobox*Listbox*background", "#292929")
    root.option_add("*TCombobox*Listbox*foreground", "#FFFFFF")
    root.option_add("*TCombobox*Listbox*font", monaco_font)

    root.configure(bg="#292929")
    combo_style = ttk.Style()
    combo_style.theme_use('default')
    combo_style.configure("TCombobox",
                          selectbackground="#292929",
                          fieldbackground="#292929",
                          background="#292929",
                          foreground="#FFFFFF")

    app = MainOpenSource(root)
    root.mainloop()
