#!/usr/bin/env python
# _*_ coding: utf-8 _*_
"""
源文件名：basic_UI.py
功能描述：
作者：ShuJie
版本：V0.1
创建时间：2021/12/14 20:43

修改历史：
修改时间：
版本号：
修改人：
修改内容：
"""

# 库引入
import tkinter as tk
from PIL import Image, ImageTk    # tk自带的仅支持GIF、PGM和PPM
# 变量声明

# 函数定义

def get_image(pic, width, height):
    im = Image.open(pic).resize((width, height), Image.ANTIALIAS)
    return ImageTk.PhotoImage(im)


class BasicUI(object):

    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.win = tk.Tk()
        self.win.resizable(False, False)
        self.win.title('OpenSource Tools')
        self.win.geometry(f'{self.width}x{self.height}+{(self.win.winfo_screenwidth() - self.width) // 2}'
                          f'+{(self.win.winfo_screenheight() - self.height) // 2 - 18}')

        but_font = ('黑体', 14)
        label_font = ('Arial', 20)
        photo = get_image('basic_back.jpg', self.width, self.height)
        self.canvas_basic = tk.Canvas(self.win, width=self.width, height=self.height)
        self.canvas_basic.create_image(self.width / 2, self.height / 2, image=photo)
        self.canvas_basic.pack()

        self.label = tk.Label(self.win, text='Welcome to this opensource script ', font=label_font, fg='black')
        self.start_button = tk.Button(self.win, text='start', font=but_font)
        self.label.place(x=self.width / 4, y=self.height / 4)
        self.start_button.place(x=self.width / 2, y=self.height / 2)

        self.win.mainloop()


BasicUI(720, 480)
