# !/usr/bin/env python3
# _*_ coding: utf-8 _*_
"""
源文件名: MainFrm.py
功能描述: UI界面的主函数
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
import time
import threading
import datetime
import tkinter as tk
import tkinter.ttk as ttk
import tkinter.font as tkf
from MenuFrm import MenuFrame
from PIL import Image, ImageTk    # tk自带的仅支持GIF、PGM和PPM
# 宏定义

# 版本号
VERSION = ''
# 函数实现
def getImage(file):
    im = Image.open(file)
    return ImageTk.PhotoImage(im)

# 类实现

class MainFrame(object):
    """
    主UI
    """

    def __init__(self, master=None):
        self.root = master

        self.clear_icon = getImage('../image/clear.jpg')
        self.loginInfo = './temp_loginInfo.txt'
        self.version = VERSION
        self.login_mark_time = 15

        self.createFrame()

    def createFrame(self):
        """
        创建主窗口
        :return:
        """
        self.frm_main = tk.LabelFrame(self.root)
        self.frm_main.pack(fill='both', expand=1)

        self.createFrmMain()
        self.initMenuFrame()

    def createFrmMain(self):
        """
        创建Menu
        :return:
        """
        self.frm_menu = MenuFrame(self.frm_main)
        self.frm_menu.frm.pack(fill='both', expand=1)

    def initMenuFrame(self):
        """
        初始化窗口的一些参数
        :return:
        """
        self.getLastLogin()
        self.frm_menu.version_label['text'] = self.version
        self.frm_menu.copy_btn['command'] = self.copyClick
        self.frm_menu.check_btn['command'] = self.checkClick
        self.frm_menu.sdk_btn['command'] = self.sdkClick
        self.frm_menu.out_clear_btn['image'] = self.clear_icon
        self.frm_menu.out_text.tag_config('green', foreground="#228B22")
        self.startTime()

    def getLastLogin(self):
        """
        获取上次登录信息
        :return:
        """
        try:
            file_time = os.path.getmtime(self.loginInfo)
            current = time.time()
            if current - file_time < self.login_mark_time * 60:      # 15分钟以后的登录信息将不要
                with open(self.loginInfo, 'r') as f:
                    temp = f.readlines()
                self.frm_menu.ip_entry.insert(0, temp[0].split(':')[-1].strip())
                self.frm_menu.user_entry.insert(0, temp[1].split(':')[-1].strip())
                self.frm_menu.pd_entry.insert(0, temp[2].split(':')[-1].strip())
                self.frm_menu.windows_path_entry.insert(0, temp[3].split(':')[-1].strip())
                self.frm_menu.linux_path_entry.insert(0, temp[4].split(':')[-1].strip())
            else:
                if os.path.exists(self.loginInfo):
                    os.remove(self.loginInfo)
        except OSError as e:
            # 第一次运行时不带登录信息,跳过
            pass

    def _setCurrLogin(self):
        """
        记录此次登录信息,只有在尝试链接成功以后才能调用
        格式为:          ip: xx
                         user: xx
                         pd: xx
                         win_path: xx
                         lin_path: xx
        :return:
        """
        temp1 = self.frm_menu.ip_entry.get()
        temp2 = self.frm_menu.user_entry.get()
        temp3 = self.frm_menu.pd_entry.get()
        temp4 = self.frm_menu.windows_path_entry.get()
        temp5 = self.frm_menu.linux_path_entry.get()
        with open(self.loginInfo, 'w') as f:
            f.write(''.join(['ip:', temp1, '\n']))
            f.write(''.join(['user:', temp2, '\n']))
            f.write(''.join(['pd:', temp3, '\n']))
            f.write(''.join(['win_path:', temp4, '\n']))
            f.write(''.join(['lin_path:', temp5, '\n']))

    @staticmethod
    def startThreadTimer(callback, timer=1):
        """
        启动一个循环线程
        :param callback: 回调函数
        :param timer:    时间差
        :return:         无
        """
        temp_thread = threading.Timer(timer, callback)
        temp_thread.setDaemon(True)
        temp_thread.start()

    @staticmethod
    def startThreadThread(callback, name='thread'):
        """
        启动一个线程
        :param callback: 回调函数
        :param name:     线程名
        :return:         无
        """
        temp_thread = threading.Thread(target=callback, name=name)
        temp_thread.setDaemon(True)
        temp_thread.start()

    def updateVer(self):
        self.frm_menu.version_label['text'] = self.version

    def startTime(self):
        self.updateTime()
        self.startThreadTimer(self.startTime, 1)

    # 后期会考虑将该函数移到主函数当中,并且更改写法,现在的写法缺乏移植性
    def updateTime(self):
        """
        更新时间
        :return: 无
        """
        self.frm_menu.time_label['text'] = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')

    def copyClick(self):
        """
        copy按键
        :return:
        """
        pass

    def checkClick(self):
        """
        check按键
        :return:
        """
        pass

    def sdkClick(self):
        """
        SDK按键
        :return:
        """
        pass


if __name__ == '__main__':
    root = tk.Tk()
    root.columnconfigure(0, weight=1)
    root.rowconfigure(0, weight=1)
    root.geometry()

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

    app = MainFrame(root)
    root.mainloop()
