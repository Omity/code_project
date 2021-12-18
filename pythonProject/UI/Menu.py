# !/usr/bin/env python3
# _*_ coding: utf-8 _*_
"""
源文件名: Menu.py
功能描述: 
作者: ShuJie
版本: Ver1.0
创建日期: 2021/12/16 14:29

修改历史:
修改日期：
版本号：
修改人：
修改内容：
"""

# 导入的包
import tkinter as tk
import pythonProject.copyFile as cpFile
import tkinter.messagebox as msg
# 宏定义
G_FONT = ('Monaco', 16)
# 版本号

# 函数实现

# 类实现

class Menu(object):

    def __init__(self, master=None):
        self.root = master
        self.root.title('Open Source Tools')
        """
        初始化变量,防止报warning
        """
        self.frm = None
        self.frm_cp = None
        self.frm_ck = None
        self.frm_sdk = None
        self.cpBtn = None
        self.connect_label = None
        self.ip_entry = None
        self.user_entry = None
        self.passwd_entry = None
        self.windows_entry = None
        self.linux_entry = None
        self.ckBtn = None
        self.sdkBtn = None

        self.cpInfo = None
        self.createFrame()

    def createFrame(self):
        self.frm = tk.Frame(self.root)
        self.frm_cp = tk.LabelFrame(self.frm)
        self.frm_ck = tk.LabelFrame(self.frm)
        self.frm_sdk = tk.LabelFrame(self.frm)

        self.frm.pack(fill='both', expand=1)
        self.frm_cp.pack(fill='both', expand=1)
        self.frm_ck.pack(fill='both', expand=1)
        self.frm_sdk.pack(fill='both', expand=1)

        self.createBtn()
        self.createCopyFrm()

    def createBtn(self):
        self.ckBtn = tk.Button(self.frm_ck, text='check', font=G_FONT, activebackground='green')
        self.sdkBtn = tk.Button(self.frm_sdk, text='sdk', font=G_FONT, activebackground='green')

        # self.cpBtn.pack(expand=0)
        self.ckBtn.pack(expand=0)
        self.sdkBtn.pack(expand=0)

    def createCopyFrm(self):
        label_font = ('Monaco', 12)
        ip_label = tk.Label(self.frm_cp, text='ip:', font=label_font)
        user_label = tk.Label(self.frm_cp, text='user:', font=label_font)
        passwd_label = tk.Label(self.frm_cp, text='passwd:', font=label_font)
        windows_path = tk.Label(self.frm_cp, text='Windows Path:', font=label_font)
        linux_path = tk.Label(self.frm_cp, text='Linux Path:', font=label_font)
        self.connect_label = tk.Label(self.frm_cp, text='未连接', fg='red', font=label_font, relief=tk.RAISED)
        self.ip_entry = tk.Entry(self.frm_cp, width=20)
        self.user_entry = tk.Entry(self.frm_cp, width=20)
        self.passwd_entry = tk.Entry(self.frm_cp, width=20, show='*')
        self.windows_entry = tk.Entry(self.frm_cp, width=20)
        self.linux_entry = tk.Entry(self.frm_cp, width=20)
        self.cpBtn = tk.Button(self.frm_cp, text='copy', font=G_FONT,
                               bg='lightblue',
                               command=self.copyClick,
                               activebackground='green')

        ip_label.grid(row=0, column=0)
        user_label.grid(row=1, column=0)
        passwd_label.grid(row=2, column=0)
        windows_path.grid(row=3, column=0)
        linux_path.grid(row=4, column=0)
        self.connect_label.grid(row=0, column=3)
        self.ip_entry.grid(row=0, column=1)
        self.user_entry.grid(row=1, column=1)
        self.passwd_entry.grid(row=2, column=1)
        self.windows_entry.grid(row=3, column=1)
        self.linux_entry.grid(row=4, column=1)
        self.cpBtn.grid(row=5, column=5, sticky=tk.W)

    def copyClick(self):
        """
        copy按键点击
        """
        self.cpToggle()

    def getCpInfo(self):
        status = 1
        ip = self.ip_entry.get()
        user = self.user_entry.get()
        pd = self.passwd_entry.get()
        self.cpInfo = [user, pd, ip]

    def tryConnect(self):
        cp = cpFile.CopyFile(cpFile.PSCP_PATH, self.cpInfo[0], self.cpInfo[1], self.cpInfo[2])
        cp.connect()
        if cp.connect_status:
            self.connect_label.config(text='已连接', fg='green')
        else:
            self.cpBtn['text'] = 'copy'
            msg.showerror('尝试连接失败')

    def cpToggle(self):
        status = 0
        if self.cpBtn['text'] == 'copy':
            status = 1
            self.cpBtn['text'] = 'exit'
        else:
            self.cpBtn['text'] = 'copy'
        # if 1 == status:
        #     self.getCpInfo()
        #     self.tryConnect()


class OpenSourceHelper(object):

    def __init__(self, ip=None):
        self.ip = ip


if __name__ == '__main__':
    root = tk.Tk()
    root.geometry('480x480')

    app = Menu(root)
    root.mainloop()
