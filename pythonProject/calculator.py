# !/usr/bin/env python3
# _*_ coding: utf-8 _*_
"""
源文件名: calculator.py
功能描述: 
作者: ShuJie
版本: Ver1.0
创建日期: 2021/12/7 14:48

修改历史:
修改日期：
版本号：
修改人：
修改内容：
"""

# 导入的包
import wx
import wx.xrc
import math
import tkinter as TK
import os
import sys
# import threading
# 宏定义

BASE_DIR = os.path.dirname(os.path.dirname(__file__))
sys.path.append(BASE_DIR)
# 版本号

# 类实现

class WxPythonCalc(wx.Frame):
    def __init__(self, parent, *args, **kw):
        super().__init__(*args, **kw)

        wx.Frame.__init__(self, parent, id=wx.ID_ANY, title=wx.EmptyString, pos=wx.DefaultPosition,
                          size=wx.Size(486, 448), style=wx.DEFAULT_FRAME_STYLE | wx.TAB_TRAVERSAL)

        self.SetSizeHintsSz(wx.DefaultSize, wx.DefaultSize)

        bSizer1 = wx.BoxSizer(wx.VERTICAL)

        self.m_textCtrl1 = wx.TextCtrl(self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.Size(600, 60),
                                       style=wx.TE_RIGHT)
        self.m_textCtrl1.SetMinSize(wx.Size(470, 60))

        bSizer1.Add(self.m_textCtrl1, 0, wx.ALL, 5)
        bSizer2 = wx.BoxSizer(wx.HORIZONTAL)

        self.m_button1 = wx.Button(self, wx.ID_ANY, u"退格", wx.DefaultPosition, wx.Size(110, 60), 0)
        bSizer2.Add(self.m_button1, 0, wx.ALL, 5)

        self.m_button2 = wx.Button(self, wx.ID_ANY, u"清屏", wx.DefaultPosition, wx.Size(110, 60), 0)
        bSizer2.Add(self.m_button2, 0, wx.ALL, 5)

        self.m_button3 = wx.Button(self, wx.ID_ANY, u"sqrt", wx.DefaultPosition, wx.Size(110, 60), 0)
        bSizer2.Add(self.m_button3, 0, wx.ALL, 5)

        self.m_button4 = wx.Button(self, wx.ID_ANY, u"/", wx.DefaultPosition, wx.Size(110, 60), 0)
        bSizer2.Add(self.m_button4, 0, wx.ALL, 5)

        bSizer1.Add(bSizer2, 0, wx.EXPAND, 5)

        bSizer6 = wx.BoxSizer(wx.HORIZONTAL)

        self.m_button10 = wx.Button(self, wx.ID_ANY, u"7", wx.DefaultPosition, wx.Size(110, 60), 0)
        bSizer6.Add(self.m_button10, 0, wx.ALL, 5)

        self.m_button11 = wx.Button(self, wx.ID_ANY, u"8", wx.DefaultPosition, wx.Size(110, 60), 0)
        bSizer6.Add(self.m_button11, 0, wx.ALL, 5)

        self.m_button12 = wx.Button(self, wx.ID_ANY, u"9", wx.DefaultPosition, wx.Size(110, 60), 0)
        bSizer6.Add(self.m_button12, 0, wx.ALL, 5)

        self.m_button13 = wx.Button(self, wx.ID_ANY, u"x", wx.DefaultPosition, wx.Size(110, 60), 0)
        bSizer6.Add(self.m_button13, 0, wx.ALL, 5)

        bSizer1.Add(bSizer6, 0, wx.EXPAND, 5)

        bSizer7 = wx.BoxSizer(wx.HORIZONTAL)

        self.m_button15 = wx.Button(self, wx.ID_ANY, u"4", wx.DefaultPosition, wx.Size(110, 60), 0)
        bSizer7.Add(self.m_button15, 0, wx.ALL, 5)

        self.m_button16 = wx.Button(self, wx.ID_ANY, u"5", wx.DefaultPosition, wx.Size(110, 60), 0)
        bSizer7.Add(self.m_button16, 0, wx.ALL, 5)

        self.m_button17 = wx.Button(self, wx.ID_ANY, u"6", wx.DefaultPosition, wx.Size(110, 60), 0)
        bSizer7.Add(self.m_button17, 0, wx.ALL, 5)

        self.m_button18 = wx.Button(self, wx.ID_ANY, u"-", wx.DefaultPosition, wx.Size(110, 60), 0)
        bSizer7.Add(self.m_button18, 0, wx.ALL, 5)

        bSizer1.Add(bSizer7, 0, wx.EXPAND, 5)

        bSizer34 = wx.BoxSizer(wx.HORIZONTAL)

        self.m_button140 = wx.Button(self, wx.ID_ANY, u"1", wx.DefaultPosition, wx.Size(110, 60), 0)
        bSizer34.Add(self.m_button140, 0, wx.ALL, 5)

        self.m_button141 = wx.Button(self, wx.ID_ANY, u"2", wx.DefaultPosition, wx.Size(110, 60), 0)
        bSizer34.Add(self.m_button141, 0, wx.ALL, 5)

        self.m_button142 = wx.Button(self, wx.ID_ANY, u"3", wx.DefaultPosition, wx.Size(110, 60), 0)
        bSizer34.Add(self.m_button142, 0, wx.ALL, 5)

        self.m_button143 = wx.Button(self, wx.ID_ANY, u"+", wx.DefaultPosition, wx.Size(110, 60), 0)
        bSizer34.Add(self.m_button143, 0, wx.ALL, 5)

        bSizer1.Add(bSizer34, 0, wx.EXPAND, 5)

        bSizer35 = wx.BoxSizer(wx.HORIZONTAL)

        self.m_button145 = wx.Button(self, wx.ID_ANY, u"0", wx.DefaultPosition, wx.Size(110, 60), 0)
        bSizer35.Add(self.m_button145, 0, wx.ALL, 5)

        self.m_button148 = wx.Button(self, wx.ID_ANY, u".", wx.DefaultPosition, wx.Size(110, 60), 0)
        bSizer35.Add(self.m_button148, 0, wx.ALL, 5)

        self.m_button149 = wx.Button(self, wx.ID_ANY, u"＋／－", wx.DefaultPosition, wx.Size(110, 60), 0)
        bSizer35.Add(self.m_button149, 0, wx.ALL, 5)

        self.m_button150 = wx.Button(self, wx.ID_ANY, u"=", wx.DefaultPosition, wx.Size(110, 600), 0)
        self.m_button150.SetMinSize(wx.Size(110, 60))

        bSizer35.Add(self.m_button150, 0, wx.ALL, 5)

        bSizer1.Add(bSizer35, 0, wx.EXPAND, 5)

        self.SetSizer(bSizer1)
        self.Layout()

        self.Centre(wx.BOTH)

        # Connect Events
        self.m_button1.Bind(wx.EVT_BUTTON, self.m_button1OnButtonClick)
        self.m_button2.Bind(wx.EVT_BUTTON, self.m_button2OnButtonClick)
        self.m_button3.Bind(wx.EVT_BUTTON, self.m_button3OnButtonClick)
        self.m_button4.Bind(wx.EVT_BUTTON, self.m_button4OnButtonClick)
        self.m_button10.Bind(wx.EVT_BUTTON, self.m_button10OnButtonClick)
        self.m_button11.Bind(wx.EVT_BUTTON, self.m_button11OnButtonClick)
        self.m_button12.Bind(wx.EVT_BUTTON, self.m_button12OnButtonClick)
        self.m_button13.Bind(wx.EVT_BUTTON, self.m_button13OnButtonClick)
        self.m_button15.Bind(wx.EVT_BUTTON, self.m_button15OnButtonClick)
        self.m_button16.Bind(wx.EVT_BUTTON, self.m_button16OnButtonClick)
        self.m_button17.Bind(wx.EVT_BUTTON, self.m_button17OnButtonClick)
        self.m_button18.Bind(wx.EVT_BUTTON, self.m_button18OnButtonClick)
        self.m_button140.Bind(wx.EVT_BUTTON, self.m_button140OnButtonClick)
        self.m_button141.Bind(wx.EVT_BUTTON, self.m_button141OnButtonClick)
        self.m_button142.Bind(wx.EVT_BUTTON, self.m_button142OnButtonClick)
        self.m_button143.Bind(wx.EVT_BUTTON, self.m_button143OnButtonClick)
        self.m_button145.Bind(wx.EVT_BUTTON, self.m_button145OnButtonClick)
        self.m_button148.Bind(wx.EVT_BUTTON, self.m_button148OnButtonClick)
        self.m_button149.Bind(wx.EVT_BUTTON, self.m_button149OnButtonClick)
        self.m_button150.Bind(wx.EVT_BUTTON, self.m_button150OnButtonClick)

    def __del__(self):
        pass

    # Virtual event handlers, override them in your derived class
    def m_button1OnButtonClick(self, event):
        result = self.m_textCtrl1.GetValue()
        result = result[:-1]
        self.m_textCtrl1.SetValue(result)

    def m_button2OnButtonClick(self, event):
        # result = self.m_textCtrl1.GetValue()
        result = ''
        self.m_textCtrl1.SetValue(result)

    def m_button3OnButtonClick(self, event):
        result = self.m_textCtrl1.GetValue()
        result = int(result)
        result = math.sqrt(result)
        self.m_textCtrl1.SetValue(str(result))

    def m_button4OnButtonClick(self, event):
        result = self.m_textCtrl1.GetValue()
        result = result + '/'
        self.m_textCtrl1.SetValue(result)

    def m_button10OnButtonClick(self, event):
        result = self.m_textCtrl1.GetValue()
        result = result + '7'
        self.m_textCtrl1.SetValue(result)

    def m_button11OnButtonClick(self, event):
        result = self.m_textCtrl1.GetValue()
        result = result + '8'
        self.m_textCtrl1.SetValue(result)

    def m_button12OnButtonClick(self, event):
        result = self.m_textCtrl1.GetValue()
        result = result + '9'
        self.m_textCtrl1.SetValue(result)

    def m_button13OnButtonClick(self, event):
        result = self.m_textCtrl1.GetValue()
        result = result + '*'
        self.m_textCtrl1.SetValue(result)

    def m_button15OnButtonClick(self, event):
        result = self.m_textCtrl1.GetValue()
        result = result + '4'
        self.m_textCtrl1.SetValue(result)

    def m_button16OnButtonClick(self, event):
        result = self.m_textCtrl1.GetValue()
        result = result + '5'
        self.m_textCtrl1.SetValue(result)

    def m_button17OnButtonClick(self, event):
        result = self.m_textCtrl1.GetValue()
        result = result + '6'
        self.m_textCtrl1.SetValue(result)

    def m_button18OnButtonClick(self, event):
        result = self.m_textCtrl1.GetValue()
        result = result + '-'
        self.m_textCtrl1.SetValue(result)

    def m_button140OnButtonClick(self, event):
        result = self.m_textCtrl1.GetValue()
        result = result + '1'
        self.m_textCtrl1.SetValue(result)

    def m_button141OnButtonClick(self, event):
        result = self.m_textCtrl1.GetValue()
        result = result + '2'
        self.m_textCtrl1.SetValue(result)

    def m_button142OnButtonClick(self, event):
        result = self.m_textCtrl1.GetValue()
        result = result + '3'
        self.m_textCtrl1.SetValue(result)

    def m_button143OnButtonClick(self, event):
        result = self.m_textCtrl1.GetValue()
        result = result + '+'
        self.m_textCtrl1.SetValue(result)

    def m_button145OnButtonClick(self, event):
        result = self.m_textCtrl1.GetValue()
        result = result + '0'
        self.m_textCtrl1.SetValue(result)

    def m_button148OnButtonClick(self, event):
        result = self.m_textCtrl1.GetValue()
        result = result + '.'
        self.m_textCtrl1.SetValue(result)

    def m_button149OnButtonClick(self, event):
        result = self.m_textCtrl1.GetValue()
        result = -int(result)
        self.m_textCtrl1.SetValue(str(result))

    def m_button150OnButtonClick(self, event):
        self.m_textCtrl1.SetValue(str(eval(self.m_textCtrl1.GetValue())))


class TkinterCalc:

    def __init__(self):
        self.root = TK.Tk()                     # 创建TK实例
        self.root.title("TKINTER Calculator")   # 设置窗口的显示名字
        self.root.resizable(0, 0)               # 设置主窗口的宽度和高度是否可以通过鼠标进行拉伸改变，此处设置为不能
        self.root.geometry('320x420')           # 这里设置主窗口的初始尺寸，因为我们在上面设定了主窗口大小  不可变，因此这个尺寸也就是主窗口一直不变的尺寸了

        self.result = TK.StringVar()     # 用来显示结果的可变文本
        self.equation = TK.StringVar()   # 用来显示算式的可变文本
        self.result.set(' ')             # 赋初始值
        self.equation.set('0')           # 赋初始值

        self.show_equation = TK.Label(self.root, bg='white', fg='black', font=('Arail', '15'), bd='0',
                                      textvariable=self.equation, anchor='se')
        self.show_result = TK.Label(self.root, bg='white', fg='black', font=('Arail', '30'), bd='0',
                                    textvariable=self.result, anchor='se')
        self.show_equation.place(x='10', y='10', width='300', height='50')
        self.show_result.place(x='10', y='60', width='300', height='50')

    # 获得按下的数字或者符号
    def getNumber(self, num):
        temp = self.equation.get()
        temp2 = self.result.get()
        print(temp)
        print(temp2)
        if temp2 != ' ':
            temp = '0'
            temp2 = ' '
            self.result.set(temp2)
        if temp == '0':
            temp = ''
        temp = temp + num
        self.equation.set(temp)
        print(self.equation)

    # 按下退格键时，去除最后一个字符
    def back(self):
        temp = self.equation.get()
        self.equation.set(temp[:-1])

    # 按下MC时，清空算式行与结果行
    def clear(self):
        self.equation.set('0')
        self.result.set(' ')

    # 按下等于号时计算结果
    def run(self):
        temp = self.equation.get()
        temp = temp.replace('x', '*')
        temp = temp.replace('÷', '/')
        if temp == '5+2+0+1+3+1+4':  # 暗号
            self.result.set("surprise!")  # 彩蛋或者表白语
            return 0
        print(temp)
        answer = eval(temp)
        answer = '%.4f' % answer
        self.result.set(str(answer))

    # 结果显示框
    def buttonSet(self):
        # 第一行按钮
        button_back = TK.Button(self.root, text='←', bg='DarkGray', command=self.back)
        button_back.place(x='10', y='150', width='60', height='40')
        button_lbracket = TK.Button(self.root, text='(', bg='DarkGray', command=lambda: self.getNumber('('))
        button_lbracket.place(x='90', y='150', width='60', height='40')
        button_rbracket = TK.Button(self.root, text=')', bg='DarkGray', command=lambda: self.getNumber(')'))
        button_rbracket.place(x='170', y='150', width='60', height='40')
        button_division = TK.Button(self.root, text='÷', bg='DarkGray', command=lambda: self.getNumber('÷'))
        button_division.place(x='250', y='150', width='60', height='40')
        # 第二行按钮
        button_7 = TK.Button(self.root, text='7', bg='DarkGray', command=lambda: self.getNumber('7'))
        button_7.place(x='10', y='205', width='60', height='40')
        button_8 = TK.Button(self.root, text='8', bg='DarkGray', command=lambda: self.getNumber('8'))
        button_8.place(x='90', y='205', width='60', height='40')
        button_9 = TK.Button(self.root, text='9', bg='DarkGray', command=lambda: self.getNumber('9'))
        button_9.place(x='170', y='205', width='60', height='40')
        button_multiplication = TK.Button(self.root, text='X', bg='DarkGray', command=lambda: self.getNumber('x'))
        button_multiplication.place(x='250', y='205', width='60', height='40')
        # 第三行按钮
        button_4 = TK.Button(self.root, text='4', bg='DarkGray', command=lambda: self.getNumber('4'))
        button_4.place(x='10', y='260', width='60', height='40')
        button_5 = TK.Button(self.root, text='5', bg='DarkGray', command=lambda: self.getNumber('5'))
        button_5.place(x='90', y='260', width='60', height='40')
        button_6 = TK.Button(self.root, text='6', bg='DarkGray', command=lambda: self.getNumber('6'))
        button_6.place(x='170', y='260', width='60', height='40')
        button_minus = TK.Button(self.root, text='—', bg='DarkGray', command=lambda: self.getNumber('-'))
        button_minus.place(x='250', y='260', width='60', height='40')
        # 第四行按钮
        button_1 = TK.Button(self.root, text='1', bg='DarkGray', command=lambda: self.getNumber('1'))
        button_1.place(x='10', y='315', width='60', height='40')
        button_2 = TK.Button(self.root, text='2', bg='DarkGray', command=lambda: self.getNumber('2'))
        button_2.place(x='90', y='315', width='60', height='40')
        button_3 = TK.Button(self.root, text='3', bg='DarkGray', command=lambda: self.getNumber('3'))
        button_3.place(x='170', y='315', width='60', height='40')
        button_plus = TK.Button(self.root, text='+', bg='DarkGray', command=lambda: self.getNumber('+'))
        button_plus.place(x='250', y='315', width='60', height='40')
        # 第五行按钮
        button_MC = TK.Button(self.root, text='MC', bg='DarkGray', command=self.clear)
        button_MC.place(x='10', y='370', width='60', height='40')
        button_0 = TK.Button(self.root, text='0', bg='DarkGray', command=lambda: self.getNumber('0'))
        button_0.place(x='90', y='370', width='60', height='40')
        button_point = TK.Button(self.root, text='.', bg='DarkGray', command=lambda: self.getNumber('.'))
        button_point.place(x='170', y='370', width='60', height='40')
        button_equal = TK.Button(self.root, text='=', bg='DarkGray', command=self.run)
        button_equal.place(x='250', y='370', width='60', height='40')

# 函数实现

def wxPythonCal():
    app = wx.App()
    window = WxPythonCalc(None)
    window.Show(True)
    app.MainLoop()


def tkinterCal():
    window = TkinterCalc()
    window.buttonSet()
    window.root.mainloop()


if __name__ == '__main__':
    # t_wx = threading.Thread(target=wxPythonCal)
    # t_tk = threading.Thread(target=tkinterCal)
    # t_wx.start()
    # t_tk.start()
    # wxPythonCal()
    tkinterCal()
