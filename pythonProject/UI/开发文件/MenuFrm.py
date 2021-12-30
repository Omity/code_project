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
import sys
import datetime
import tkinter as tk
import tkinter.font as tkf
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
        # 重定向输出和error
        self.buf = ''
        sys.stdout = self
        sys.stderr = self
        self.createFrame()

    def createFrame(self):
        """
        创建窗体,分为左右两部分,左边为状态栏,右边为功能区
        :return: 无
        """
        self.frm = tk.Frame(self.root)

        self.frm_top = tk.Frame(self.frm)
        self.frm_bottom = tk.Frame(self.frm)

        self.frm_top.pack(fill='both', expand=1)
        self.frm_bottom.pack(fill='both', expand=0)

        self.createStatusFrame()
        self.createFunctionFrame()
        self.createVerFrame()

    def createStatusFrame(self):
        """
        状态栏显示功能区使用的公共变量, 为排版合适，将路径分离出来
        :return: 无
        """
        self.frm_status = tk.LabelFrame(self.frm_top)
        self.frm_status.pack(fill='both', expand=0, side=tk.LEFT)

        self.title_label = tk.Label(self.frm_status, text='状态栏', font=G_FONT)
        self.frm_status_top = tk.LabelFrame(self.frm_status)
        self.frm_status_bottom = tk.LabelFrame(self.frm_status)
        self.frm_status_setting = tk.Frame(self.frm_status)

        self.title_label.pack(fill='both', expand=0, padx=5, pady=5)
        self.frm_status_top.pack(fill='both', expand=0)
        self.frm_status_bottom.pack(fill='both', expand=0)
        self.frm_status_setting.pack(fill='both', expand=0)

        self.createStatusTopFrame()
        self.createStatusBottomFrame()
        self.createStatusSettingFrame()

    def createStatusTopFrame(self):
        """
        创建状态栏上部分窗体
        :return: 无
        """
        setting_label_list = ['IP:', 'user:', 'passwd:']
        self.frm_status_top_left = tk.Frame(self.frm_status_top)
        self.frm_status_top_right = tk.Frame(self.frm_status_top)
        self.frm_status_top_left.pack(fill='both', expand=0, side=tk.LEFT)
        self.frm_status_top_right.pack(fill='both', expand=1, side=tk.RIGHT)

        for item in setting_label_list:
            temp_label = tk.Label(self.frm_status_top, text=item, font=G_FONT, anchor='w')
            temp_label.pack(fill='both', expand=0, padx=5, pady=5)
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
        frm_linux_path_temp = tk.Frame(self.frm_status_bottom)
        self.windows_path_label = tk.Label(frm_windows_path_temp, text='Windows Path:', font=G_FONT)
        self.choose_path_btn = tk.Button(frm_windows_path_temp, text='...', command=self.openDir)
        self.windows_path_entry = tk.Entry(self.frm_status_bottom, width=20, textvariable=self.windows_path_var)
        self.linux_path_label = tk.Label(frm_linux_path_temp, text='Linux Path:', font=G_FONT)
        self.linux_path_entry = tk.Entry(self.frm_status_bottom, width=20)

        frm_windows_path_temp.pack(fill='both', expand=0, padx=5, pady=5)
        self.windows_path_label.pack(fill='both', expand=0, padx=5, pady=5, side=tk.LEFT)
        self.choose_path_btn.pack(fill='x', expand=0, padx=5, pady=5, side=tk.RIGHT)
        self.windows_path_entry.pack(fill='both', expand=0, padx=5, pady=5)
        frm_linux_path_temp.pack(fill='both', expand=0, padx=5, pady=5)
        self.linux_path_label.pack(fill='both', expand=0, padx=5, pady=5, side=tk.LEFT)
        self.linux_path_entry.pack(fill='both', expand=0, padx=5, pady=5)

    def createStatusSettingFrame(self):
        """
        创建状态栏的功能区,用于对UI的一些设置
        :return:
        """
        # 该功能区可能有很多附加功能,故使用grid布局
        self.set_btn = tk.Button(self.frm_status_setting, text='set',
                                 width=3, bg='lightblue', font=G_FONT,
                                 activebackground='green', command=self.setClick)

        self.set_btn.grid(row=0, column=0)

    def createFunctionFrame(self):
        """
        功能区,主要分为两部分,上半部为功能选项区,下半区为打印区
        :return:
        """
        self.frm_function = tk.LabelFrame(self.frm_top)
        self.frm_function.pack(fill='both', expand=1, side=tk.RIGHT)

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
        self.connect_status = tk.Radiobutton(self.frm_func_top_copy, text='未连接', fg='red', font=label_font)
        self.copy_btn = tk.Button(self.frm_func_top_copy, text='copy',
                                  width=6, bg='lightblue', font=G_FONT,
                                  activebackground='green', command=self.copyClick)

        self.connect_status.pack(fill='both', expand=0, padx=5, pady=5, side=tk.LEFT)
        self.copy_btn.pack(fill='both', expand=0, side=tk.RIGHT)

    def createCheckFuncFrame(self):
        """
        创建check功能区
        :return:
        """
        self.copyright_label = tk.Label(self.frm_func_top_check, text='copyright', font=G_FONT)
        self.licenses_label = tk.Label(self.frm_func_top_check, text='licenses', font=G_FONT)
        self.check_btn = tk.Button(self.frm_func_top_check, text='check',
                                   width=6, bg='lightblue', font=G_FONT,
                                   activebackground='green', command=self.checkClick)

        self.copyright_label.pack(fill='both', expand=0, side=tk.LEFT)
        self.licenses_label.pack(fill='both', expand=0, side=tk.LEFT)
        self.check_btn.pack(fill='both', expand=0, side=tk.RIGHT)

    def createSDKFuncFrame(self):
        """
        创建sdk功能区
        """
        self.file_path_var = tk.StringVar()
        self.file_path_label = tk.Label(self.frm_func_top_sdk, text='file:', font=G_FONT)
        self.file_path_entry = tk.Entry(self.frm_func_top_sdk, width=20, textvariable=self.file_path_var)
        self.file_path_btn = tk.Button(self.frm_func_top_sdk, text='...', command=self.openFile)
        self.sdk_btn = tk.Button(self.frm_func_top_sdk, text='sdk',
                                 width=6, bg='lightblue', font=G_FONT,
                                 activebackground='green', command=self.sdkClick)

        self.file_path_label.pack(fill='both', expand=0, padx=5, pady=5, side=tk.LEFT)
        self.file_path_entry.pack(fill='both', expand=0, padx=5, pady=5, side=tk.LEFT)
        self.file_path_btn.pack(fill='both', expand=0, padx=5, pady=5, side=tk.LEFT)
        self.sdk_btn.pack(fill='both', expand=0, side=tk.RIGHT)

    def createFuncBotFrame(self):
        """
        创建输出打印窗口
        """
        self.frm_out_left = tk.Frame(self.frm_function_bot)
        self.out_clear_btn = tk.Button(self.frm_out_left, command=self.outClear)
        self.out_label = tk.Label(self.frm_out_left, text='\n\n输\n\n\n\n出', font=G_FONT)
        self.s_bar = tk.Scrollbar(self.frm_function_bot)
        self.out_text = tk.Text(self.frm_function_bot, state='disabled', yscrollcommand=self.s_bar.set)

        self.s_bar.config(command=self.out_text.yview)
        self.frm_out_left.pack(fill='both', expand=0, padx=5, pady=5, side=tk.LEFT)
        self.out_clear_btn.pack(fill='both', expand=0, padx=5, pady=5)
        self.out_label.pack(fill='both', expand=0, padx=5, pady=5)
        self.s_bar.pack(fill='both', expand=0, padx=5, pady=5, side=tk.RIGHT)
        self.out_text.pack(fill='both', expand=1, padx=5, pady=5, side=tk.RIGHT)

    def createVerFrame(self):
        """
        创建版本信息窗口
        :return:
        """
        self.frm_version = tk.LabelFrame(self.frm_bottom)
        self.frm_version.pack(fill='both', expand=0, side=tk.BOTTOM)

        self.frm_version_left = tk.Frame(self.frm_version)
        self.version = tk.Label(self.frm_version_left, text='Version:', font=G_FONT)
        self.version_label = tk.Label(self.frm_version_left, text='Version:', font=G_FONT)
        self.time_label = tk.Label(self.frm_version, text=datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
                                   font=G_FONT)

        self.frm_version_left.pack(fill='both', expand=0, padx=5, pady=5, side=tk.LEFT)
        self.version.pack(fill='both', expand=0, padx=5, pady=5, side=tk.LEFT)
        self.version_label.pack(fill='both', expand=0, padx=5, pady=5, side=tk.RIGHT)
        self.time_label.pack(fill='both', expand=0, padx=5, pady=5, side=tk.RIGHT)

    def openDir(self):
        """
        打开Windows文件路径
        :return: 无
        """
        self.windows_path_var.set(fd.askdirectory())

    def openFile(self):
        """
        打开Windows文件路径
        :return:
        """
        self.file_path_var.set(fd.askopenfilename())

    def copyClick(self):
        """
        copy按键点击
        :return:
        """
        pass

    def checkClick(self):
        """
        check按键功能
        :return:
        """
        pass

    def sdkClick(self):
        """
        sdk按键功能区
        :return:
        """
        pass

    def setClick(self):
        """
        set按键功能区
        :return:
        """
        pass

    def outClear(self):
        """
        清除输出内容
        :return:
        """
        self.out_text['state'] = 'normal'
        self.out_text.delete("0.0", "end")
        self.out_text['state'] = 'disabled'

    def write(self, buf):
        """
        重写write函数
        :param buf:
        :return:
        """
        self.buf += buf
        self.out_text['state'] = 'normal'
        self.setSpecificWord()
        self.out_text.insert('end', self.buf)
        self.flush()
        self.out_text['state'] = 'disabled'
        self.out_text.see('end')

    def flush(self):
        self.buf = ''

    def setSpecificWord(self):
        """
        设置一些特殊字体
        :return: 无
        """
        fail_list = ['failed', 'fail', 'failure', 'invalid', 'Error', 'error', 'None', 'none', 'not']
        success_list = ['success', 'succeed', 'successfully', 'ok', 'OK', 'done', 'Done']
        self.out_text.tag_remove('found_failure', '1.0', tk.END)
        self.out_text.tag_remove('found_success', '1.0', tk.END)
        self.out_text.tag_remove('error', '1.0', tk.END)

        for word in fail_list:
            idx = '1.0'
            while idx:
                idx = self.out_text.search(word, idx, nocase=1, stopindex=tk.END)
                if idx:
                    last_idx = '%s+%dc' % (idx, len(word))
                    self.out_text.tag_add('found_failure', idx, last_idx)
                    idx = last_idx
        self.out_text.tag_config('found_failure', foreground='red')
        for word in success_list:
            idx = '1.0'
            while idx:
                idx = self.out_text.search(word, idx, nocase=1, stopindex=tk.END)
                if idx:
                    last_idx = '%s+%dc' % (idx, len(word))
                    self.out_text.tag_add('found_success', idx, last_idx)
                    idx = last_idx
        self.out_text.tag_config('found_success', foreground='green')


if __name__ == '__main__':

    root = tk.Tk()
    root.columnconfigure(0, weight=1)
    root.rowconfigure(0, weight=1)
    root.geometry()

    monacoFont = tkf.Font(family="Monaco", size=16)
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
    app.frm.pack(fill='both', expand=1)
    root.mainloop()
