# 普源精电科技技术有限公司版权所有 (2021 - )

# !/usr/bin/env python3
# _*_ coding: utf-8 _*_
"""
源文件名: copyFile.py
功能描述: 
作者: ShuJie
版本: Ver1.0
创建日期: 2021/12/9 14:42

修改历史:
修改日期：
版本号：
修改人：
修改内容：
"""

# 导入的包
import os
import os.path
import paramiko
# 宏定义

# 版本号

# 类实现

# 函数实现
def get_file_path(path1, file_list, dir_list):   # 递归
    list_1 = os.listdir(path1)  # 当前目录下有哪些文件和子目录
    for i in range(len(list_1)):
        list_1[i] = os.path.join(path1, list_1[i])  # 将目录名和文件或文件名组成一个路径
        if os.path.isfile(list_1[i]):   # 如果该条路径下为一个文件
            file_list.append(list_1[i])
        else:
            dir_list.append(list_1[i])
            get_file_path(list_1[i], file_list, dir_list)

def mk_dri(list4, path2):
    list4.sort(key=lambda x: x.count('\\'))
    files_name = list4[0]
    files_name = files_name[files_name.rfind('\\')+1:]
    path2 = path2+'\\'+files_name

    for i in range(len(list4)):
        str_1 = list4[i]
        str_2 = path2+str_1[str_1.find(files_name)+len(files_name):]
        list4[i] = str_2
        os.mkdir(list4[i])

def handle_list2(list4, list2, path2, key_word):
    files_name = list4[0]
    files_name = files_name[files_name.rfind('\\') + 1:]
    path2 = path2+'\\'+files_name
    list2.sort(key=lambda x: x.count('\\'))

    for i in range(len(list2)):
        if key_word == os.path.splitext(list2[i])[1]:
            str_1 = list2[i]

            str_2 = path2+str_1[str_1.find(files_name)+len(files_name):]

            print('正在复制文件%s到%s当中' % (list2[i], str_2))
            with open(list2[i], 'rb') as f:
                content = f.read()
            with open(str_2, 'wb') as f:
                f.write(content)


# 获取SSHClient实例
client = paramiko.SSHClient()
client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
# 连接SSH服务端
client.connect("192.168.136.137", username="GJ", password="0")

stdin, stdout, stderr = client.exec_command('ls')
result = stdout.read()
if not result:
    result = stderr.read()

# 获取Transport实例
tran = client.get_transport()
# 获取SFTP实例
sftp = paramiko.SFTPClient.from_transport(tran)

remotePath = '/home/GJ/Desktop/test/test.c'
localPath = 'E:\\test\\test.c'

sftp.get(remotePath, localPath)

client.close()

print(result.decode())
