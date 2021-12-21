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
import tkinter as tk
import tkinter.ttk as ttk
import tkinter.font as font
from MenuFrm import MenuFrame
# 宏定义

# 版本号

# 函数实现

# 类实现

class MainFrame(object):
    """
    主UI
    """
    def __init__(self, master=None):
        self.root = master
        self.createFrame()

    def createFrame(self):
        self.frm_main = tk.LabelFrame(self.root)
        self.frm_main.pack(fill='both', expand=1)

        self.createFrmMain()

    def createFrmMain(self):
        self.frm_menu = MenuFrame(self.frm_main)
        self.frm_menu.frm.pack(fill='both', expand=1)


if __name__ == '__main__':
    root = tk.Tk()
    root.columnconfigure(0, weight=1)
    root.rowconfigure(0, weight=1)
    root.geometry()

    monacofont = font.Font(family="Monaco", size=16)
    root.option_add("*TCombobox*Listbox*background", "#292929")
    root.option_add("*TCombobox*Listbox*foreground", "#FFFFFF")
    root.option_add("*TCombobox*Listbox*font", monacofont)

    root.configure(bg="#292929")
    combostyle = ttk.Style()
    combostyle.theme_use('default')
    combostyle.configure("TCombobox",
                         selectbackground="#292929",
                         fieldbackground="#292929",
                         background="#292929",
                         foreground="#FFFFFF")

    app = MainFrame(root)
    root.mainloop()
