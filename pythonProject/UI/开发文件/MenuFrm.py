# !/usr/bin/env python3
# _*_ coding: utf-8 _*_
"""
源文件名: MenuFrm.py
功能描述: 主要实现关于UI界面的设置,功能不会在这里实现
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
import tkinter.font as font
import tkinter.ttk as ttk
import tkinter.filedialog as fd
# 宏定义
G_FONT = ('Monaco', 16)
# 版本号

# 函数实现

# 类实现

class MenuFrame(object):

    def __init__(self, master=None):
        self.root = master
        self.createFrame()

        """
        参数初始化
        """
        self.frm = tk.Frame()
        self.frm_status_top_left = tk.Frame()
        self.frm_status_top_right = tk.Frame()

        self.frm_status = tk.LabelFrame()
        self.frm_status_top = tk.LabelFrame()
        self.frm_status_bottom = tk.LabelFrame()
        self.frm_function = tk.LabelFrame()
        self.frm_function_top = tk.LabelFrame()
        self.frm_func_top_copy = tk.LabelFrame()
        self.frm_func_top_check = tk.LabelFrame()
        self.frm_func_top_sdk = tk.LabelFrame()
        self.frm_function_bot = tk.LabelFrame()

        self.title_label = tk.Label()
        self.windows_path_label = tk.Label()
        self.linux_path_label = tk.Label()
        self.connect_status_label = tk.Label()
        self.connect_status = tk.Label()

        self.ip_entry = tk.Entry()
        self.user_entry = tk.Entry()
        self.pd_entry = tk.Entry()
        self.windows_path_entry = tk.Entry()
        self.linux_path_entry = tk.Entry()

        self.windows_path_var = tk.StringVar()

        self.choose_path_btn = tk.Button()
        self.copy_btn = tk.Button()

    def createFrame(self):
        """
        创建窗体,分为左右两部分,左边为状态栏,右边为功能区
        :return: 无
        """
        self.frm = tk.Frame(self.root)

        self.frm_status = tk.LabelFrame(self.frm)
        self.frm_function = tk.LabelFrame(self.frm)

        self.frm.pack(fill='both', expand=1)
        self.frm_status.pack(fill='both', expand=1, side=tk.LEFT)
        self.frm_function.pack(fill='both', expand=1, side=tk.RIGHT)

        self.createStatusFrame()
        self.createFunctionFrame()

    def createStatusFrame(self):
        """
        状态栏显示功能区使用的公共变量, 为排版合适，将路径分离出来
        :return: 无
        """

        self.title_label = tk.Label(self.frm_status, text='状态栏', font=G_FONT)
        self.frm_status_top = tk.LabelFrame(self.frm_status)
        self.frm_status_bottom = tk.LabelFrame(self.frm_status)

        self.title_label.pack(fill='both', expand=0, padx=5, pady=5)
        self.frm_status_top.pack(fill='both', expand=1)
        self.frm_status_bottom.pack(fill='both', expand=1)

        self.createStatusTopFrame()
        self.createStatusBottomFrame()

    def createStatusTopFrame(self):
        """
        创建状态栏上部分窗体
        :return: 无
        """
        setting_label_list = ['IP:', 'user:', 'passwd:']
        self.frm_status_top_left = tk.Frame(self.frm_status_top)
        self.frm_status_top_right = tk.Frame(self.frm_status_top)
        self.frm_status_top_left.pack(fill='both', expand=1, side=tk.LEFT)
        self.frm_status_top_right.pack(fill='both', expand=1, side=tk.RIGHT)

        for item in setting_label_list:
            temp_label = tk.Label(self.frm_status_top, text=item, font=G_FONT)
            temp_label.pack(fill='both', expand=1, padx=5, pady=5)
        self.ip_entry = tk.Entry(self.frm_status_top_right, width=20)
        self.user_entry = tk.Entry(self.frm_status_top_right, width=20)
        self.pd_entry = tk.Entry(self.frm_status_top_right, width=20, show='*')

        self.ip_entry.pack(fill='x', expand=1, padx=5, pady=5)
        self.user_entry.pack(fill='x', expand=1, padx=5, pady=5)
        self.pd_entry.pack(fill='x', expand=1, padx=5, pady=5)

    def createStatusBottomFrame(self):
        """
        创建状态栏下部分窗体
        :return: 无
        """
        self.windows_path_var = tk.StringVar()
        frm_windows_path_temp = tk.Frame(self.frm_status_bottom)
        self.windows_path_label = tk.Label(frm_windows_path_temp, text='Windows Path:', font=G_FONT)
        self.choose_path_btn = tk.Button(frm_windows_path_temp, text='...', command=self.openDir)
        self.windows_path_entry = tk.Entry(self.frm_status_bottom, width=20, textvariable=self.windows_path_var)
        self.linux_path_label = tk.Label(self.frm_status_bottom, text='Linux Path:', font=G_FONT)
        self.linux_path_entry = tk.Entry(self.frm_status_bottom, width=20)

        frm_windows_path_temp.pack(fill='both', expand=0, padx=5, pady=5)
        self.windows_path_label.pack(fill='both', expand=0, padx=5, pady=5, side=tk.LEFT)
        self.choose_path_btn.pack(fill='x', expand=0, padx=5, pady=5, side=tk.RIGHT)
        self.windows_path_entry.pack(fill='both', expand=1, padx=5, pady=5)
        self.linux_path_label.pack(fill='both', expand=0, padx=5, pady=5)
        self.linux_path_entry.pack(fill='x', expand=1, padx=5, pady=5)

    def createFunctionFrame(self):
        """
        功能区,主要分为两部分,上半部为功能选项区,下半区为打印区
        :return:
        """

        self.frm_function_top = tk.LabelFrame(self.frm_function)
        self.frm_function_bot = tk.LabelFrame(self.frm_function)

        self.frm_function_top.pack(fill='both', expand=1, padx=5, pady=5)
        self.frm_function_bot.pack(fill='both', expand=1, padx=5, pady=5)

        self.createFuncTopFrame()
        self.createFuncBotFrame()

    def createFuncTopFrame(self):
        """
        创建功能选项区, 分为三部分
        :return:
        """
        self.frm_func_top_copy = tk.LabelFrame(self.frm_function_top)
        self.frm_func_top_check = tk.LabelFrame(self.frm_function_top)
        self.frm_func_top_sdk = tk.LabelFrame(self.frm_function_top)

        self.frm_func_top_copy.pack(fill='both', expand=0, padx=1)
        self.frm_func_top_check.pack(fill='both', expand=0, padx=1)
        self.frm_func_top_sdk.pack(fill='both', expand=0, padx=1)

        self.createCopyFuncFrame()
        self.createCheckFuncFrame()
        self.createSDKFuncFrame()

    def createCopyFuncFrame(self):
        """
        创建copy功能区
        :return:
        """
        label_font = ('Monaco', 10)
        self.connect_status_label = tk.Label(self.frm_func_top_copy, text='status:', font=G_FONT)
        self.connect_status = tk.Label(self.frm_func_top_copy, text='disconnected', fg='red', font=label_font)
        self.copy_btn = tk.Button(self.frm_func_top_copy, text='copy',
                                  width=8, bg='lightblue', font=G_FONT,
                                  command=self.copyClick)

        self.connect_status_label.pack(fill='both', expand=0, padx=5, pady=5, side=tk.LEFT)
        self.connect_status.pack(fill='both', expand=0, padx=5, pady=5, side=tk.LEFT)
        self.copy_btn.pack(fill='both', expand=0, padx=5, pady=5, side=tk.RIGHT)

    def createCheckFuncFrame(self):
        pass

    def createSDKFuncFrame(self):
        pass

    def createFuncBotFrame(self):
        pass

    def openDir(self):
        """
        打开Windows文件路径
        :return: 无
        """
        self.windows_path_var.set(fd.askdirectory())

    def copyClick(self):
        pass


if __name__ == '__main__':

    root = tk.Tk()
    root.columnconfigure(0, weight=1)
    root.rowconfigure(0, weight=1)
    root.geometry()
    root.title("Open Source Tools")

    monacoFont = font.Font(family="Monaco", size=16)
    root.option_add("*TCombobox*Listbox*background", "#292929")
    root.option_add("*TCombobox*Listbox*foreground", "#FFFFFF")
    root.option_add("*TCombobox*Listbox*font", monacoFont)

    root.configure(bg="#292929")
    comboStyle = ttk.Style()
    comboStyle.theme_use('default')
    comboStyle.configure("TCombobox",
                         selectbackground="#292929",
                         fieldbackground="#292929",
                         background="#292929",
                         foreground="#FFFFFF")

    app = MenuFrame(root)
    # app.frm.pack(fill='both', expand=1)
    root.mainloop()
