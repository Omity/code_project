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

import time
import tkinter as tk
import tkinter.font as tkf
import tkinter.ttk as ttk
import subprocess as sp
from BtnFuc import OpenSourceHelper, Pscp
from MainFrm import MainFrame
# 宏定义
USE_DEBUG = 1
# 版本号
VERSION = 'V1.0.0'
# 函数实现
def de_info(string, end='\n'):
    """
    测试类信息
    :param end:
    :param string:
    :return:
    """
    if USE_DEBUG:
        print(string, end=end)

def pr_info(string, end='\n'):
    """
    通知类信息
    :param end:
    :param string:
    :return:
    """
    print(string, end=end)

def er_info(string, end='\n'):
    """
    错误类信息
    :param end:
    :param string:
    :return:
    """
    print(string, end=end)
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
                er_info(e)
                er_info('Open linux failed!')
        elif self.frm_menu.copy_btn['text'] == 'exit':
            self._disconnect()

    def checkClick(self):
        """
        check按键点击区
        :return:
        """
        if self.frm_menu.check_btn['text'] == 'check':
            try:
                self.frm_menu.check_btn['text'] = 'exit'
                self.frm_menu.check_btn['bg'] = 'red'
                temp_path = self.frm_menu.windows_path_entry.get()
                self.ser = OpenSourceHelper()
                self.ser.checkLicenseAndCopyright(self.startChecking, temp_path)
            except Exception as e:
                er_info(e)
                pr_info('failed to check')
        else:
            self._checked()

    def setClick(self):
        pass

    def _connect(self):
        """
        连接成功后的设置
        :return:
        """
        pr_info('connect linux successfully')
        self._is_connected = True
        # self.frm_menu.copy_btn['text'] = 'exit'
        # self.frm_menu.copy_btn['bg'] = 'red'
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
        else:
            er_info('Open linux failed!')
            self._disconnect()

    def _changeFileName(self, file_list, reverse=True):
        """
        把linux内和windows端文件名有冲突的文件改名
        :return:
        """
        self.ser.pr_info('Try to modify illegal file name ...')
        if len(file_list) != 0:
            for i in range(len(file_list)):
                count = i
                temp = ''.join(['_', file_list[i].split('/')[-1]])
                if self._is_connected:
                    if reverse:        # 修改名字
                        temp_cmd = 'mv ' + file_list[i] + ' ' + '/'.join(file_list[i].split('/')[0:-1]) + '/' + temp
                        self.ser.sendLinuxCmd(temp_cmd)
                        self.ser.de_info(f'send command: {temp_cmd}')
                    else:              # 还原名字
                        temp_cmd = 'mv ' + '/'.join(file_list[i].split('/')[0:-1]) + '/' + temp + ' ' + file_list[i]
                        self.ser.sendLinuxCmd(temp_cmd)
                else:
                    for j in range(0, count):
                        temp_cmd = 'mv ' + '/'.join(file_list[j].split('/')[0:-1]) + '/' + temp + ' ' + file_list[j]
                        self.ser.sendLinuxCmd(temp_cmd)
                    return
        if self._is_connected:
            pr_info(f'{self._changeFileName.__name__}...done')

    def getFileList(self):
        """
        获取路径文件列表
        :return: file list
        """
        self.ser.pr_info('Try to get source file list ....')
        ignore_list = ['CON.*', 'PRN.*', 'AUX.*', 'NUL.*', 'COM1.*', 'COM2.*', 'COM3.*', 'COM4.*', 'COM5.*', 'COM6.*',
                       'COM7.*', 'COM8.*', 'COM9.*', 'LPT1.*', 'LPT2.*', 'LPT3.*', 'LPT4.*', 'LPT5.*', 'LPT6.*',
                       'LPT7.*', 'LPT8.*', 'LPT9.*']
        temp_lin_path = self.frm_menu.linux_path_entry.get()
        temp_cmd = ' '.join(['find', temp_lin_path, '-type f', '-iname', f'"{ignore_list[0]}"'])
        ignore_cmd1 = ''
        for i in ignore_list[1:]:
            ignore_cmd1 += ' '.join([' -o -iname', f'"{i}"'])
        self.ser.de_info(f'send command:{temp_cmd + ignore_cmd1}')
        self.ignore_file = self.ser.sendLinuxCmd(temp_cmd + ignore_cmd1).split()
        for i in self.ignore_file:
            self.ser.de_info(i)
        if self._is_connected:
            self._changeFileName(self.ignore_file)
        if self._is_connected:
            self.isFileExist()
        if self._is_connected:
            pr_info(f'{self.getFileList.__name__}...done')

    def isFileExist(self):
        """
        查询是否有文件不存在
        :return:
        """
        pr_info('Try to check if there is nonexistent file ...')
        temp_cmd = 'find ' + self.frm_menu.linux_path_entry.get() + ' -type f '
        search_list = self.ser.sendLinuxCmd(temp_cmd).split()
        # for i in search_list:
        #     de_info(i)
        for i in search_list:
            if self._is_connected:
                temp_cmd = 'cat ' + i
                self.ser.sendLinuxCmd(temp_cmd)
                if self.ser.sendLinuxCmd('echo $?').strip() != '0':
                    try:
                        self.ser.sendLinuxCmd(f'rm {i}')
                        de_info(f'delete: {i}')
                    except Exception as e:
                        er_info(e)
            else:
                return
        if self._is_connected:
            pr_info(f'{self.isFileExist.__name__}...done')

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
                    self.getFileList()
                    if self._is_connected:
                        self.pscp = Pscp(self.pscp_path)
                        self.pscpProcess(self.pscp.linuxToWindowDir(temp_lin_path, temp_win_path,
                                         self.frm_menu.ip_entry.get(),
                                         self.frm_menu.user_entry.get(),
                                         self.frm_menu.pd_entry.get()))
                    if self._is_connected:
                        pr_info('copy successfully')
                else:
                    de_info(f'{temp_lin_path} not exist, check it!')
            else:
                de_info('Windows path or Linux path is none, check it!')
        except Exception as e:
            er_info(e)
            return
        if self._is_connected:
            self._changeFileName(self.ignore_file, reverse=False)
            pr_info('copy file successfully')
        self._disconnect()

    def pscpProcess(self, cmd):
        """
        pscp进程,以subprocess运行,保证能捕获输出
        :param cmd:
        :return:
        """
        try:
            pr_info('Try to copy source ....')
            self.p = sp.Popen(cmd, stderr=sp.STDOUT, stdin=sp.PIPE, stdout=sp.PIPE)
            # 保留,因为pscp好像第一次使用时需要选择yes
            # self.p.stdin.write('y\r\n'.encode('GBK'))
            # self.p.stdin.flush()
            while True:
                if self._is_connected and self.p.poll() != 0:
                    pr_info(self.p.stdout.readline().decode('GBK').strip())
                    time.sleep(0.01)
                else:
                    self.p.kill()
                    break
            if self._is_connected:
                pr_info(f'{self.pscpProcess.__name__}...done')
        except Exception as e:
            er_info(e)

    def startChecking(self, is_checking):
        """
        开始检测license和copyright
        :param is_checking:
        :return:
        """
        if is_checking:
            self._checking()
        else:
            self._checked()

    def _checking(self):
        """
        可查询
        :return:
        """
        self._is_checking = True
        self.frm_menu.windows_path_entry['state'] = 'disabled'
        self.frm_menu.copy_btn['state'] = 'disabled'  # 防止操作同一目录
        self.frm_menu.sdk_btn['state'] = 'disabled'

    def _checked(self):
        """
        终止查询或者结束查询
        :return:
        """
        self._is_checking = False
        self.frm_menu.check_btn['text'] = 'check'
        self.frm_menu.check_btn['bg'] = 'lightblue'
        self.frm_menu.windows_path_entry['state'] = 'normal'
        self.frm_menu.copy_btn['state'] = 'normal'  # 防止操作同一目录
        self.frm_menu.sdk_btn['state'] = 'normal'


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
