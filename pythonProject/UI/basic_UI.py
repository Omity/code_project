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
import tkinter.messagebox as msg
from PIL import Image, ImageTk    # tk自带的仅支持GIF、PGM和PPM
# 变量声明
G_FONT = ('Monaco', 16)
APP_WIDTH = 720
APP_HEIGHT = 480
# 函数定义

def get_image(pic, width, height):
    im = Image.open(pic).resize((width, height), Image.ANTIALIAS)
    return ImageTk.PhotoImage(im)


class BasicUI(object):

    def __init__(self, width, height, master=None):
        self.root = master
        self.width = width
        self.height = height
        icon = ImageTk.PhotoImage(file='image/icon.png')
        self.root.iconphoto(False, icon)

        self.state = True
        self.root.title('OpenSource Tools')

        # self.win.geometry(f'{self.width}x{self.height}+{(self.win.winfo_screenwidth() - self.width) // 2}'
        #                   f'+{(self.win.winfo_screenheight() - self.height) // 2 - 18}')

        """
        变量初始化,防止解析器报warning
        """
        self.frm = None
        self.frm_top = None
        self.canvas_basic = None
        self.info_label = None
        self.help_label = None
        self.start_button = None
        self.back_photo = get_image('image\\basic_back.jpg', self.width, self.height)

        self.createFrame()

    def createFrame(self):
        self.frm = tk.Frame(self.root, bg='blue')

        self.frm_top = tk.LabelFrame(self.frm)
        self.frm.pack(fill='both', expand=1)
        self.frm_top.pack(expand=1, anchor=tk.CENTER)

        self.createBackPic()
        self.createInfoLabel()
        self.createStartButton()

    def createStartButton(self):
        self.start_button = tk.Button(self.frm_top, text='start', font=G_FONT, activebackground='green')
        self.start_button.pack(expand=0)

    def createBackPic(self):
        self.canvas_basic = tk.Canvas(self.root, width=self.width, height=self.height)
        self.canvas_basic.create_image(self.width / 2, self.height / 2, image=self.back_photo)
        self.canvas_basic.pack(expand=1)

    def createInfoLabel(self):
        label_font = ('Monaco', 20)
        self.info_label = tk.Label(self.frm_top, text='Welcome to this opensource script', font=label_font, fg='black')
        self.info_label.pack(fill='both', expand=1)


if __name__ == '__main__':
    root = tk.Tk()
    screen_width = root.winfo_screenwidth()
    screen_height = root.winfo_screenheight()
    root.geometry(f'{APP_WIDTH}x{APP_HEIGHT}+{(screen_width - APP_WIDTH) // 2}'
                  f'+{(screen_height - APP_HEIGHT) // 2 - 18}')
    # frame1 = tk.Frame(root, height=200, width=200, bg='red')
    # frame2 = tk.Frame(frame1, height=100, width=100, bg='blue')
    # frame3 = tk.Frame(frame1, height=100, width=100, bg='green')
    # frame1.propagate(False)
    # frame1.pack()
    # frame2.pack()
    # frame3.pack()
    app = BasicUI(APP_WIDTH, APP_HEIGHT, root)
    root.mainloop()
