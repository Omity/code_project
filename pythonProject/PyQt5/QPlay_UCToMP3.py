#!/usr/bin/env python
# _*_ coding: utf-8 _*_
"""
源文件名：QPlay_UCToMP3.py
功能描述：
作者：ShuJie
版本：V0.1
创建时间：2022/1/22 17:45

修改历史：
修改时间：
版本号：
修改人：
修改内容：
"""

# 库引入

# 变量声明

# 函数定义

# 类定义
# -*- coding:utf-8 -*-

import os
import re
import requests

UC_PATH = r'C:\Users\made in china\AppData\Local\Netease\CloudMusic\Cache\Cache'  # 缓存路径 例 D:/CloudMusic/Cache/
MP3_PATH = './test'  # 存放歌曲路径


class Transform:
    def do_transform(self):
        files = os.listdir(UC_PATH)
        for file in files:
            if file[-2:] == 'uc':  # 后缀uc结尾为歌曲缓存
                print(file)
                uc_file = open(UC_PATH + file, mode='rb')
                uc_content = uc_file.read()
                mp3_content = bytearray()
                for byte in uc_content:
                    byte ^= 0xa3
                    mp3_content.append(byte)
                song_id = self.get_songid_by_filename(file)
                song_name, singer_name = self.get_song_info(song_id)
                mp3_file_name = MP3_PATH + '%s - %s.mp3' % (singer_name, song_name)
                mp3_file = open(mp3_file_name, 'wb')
                mp3_file.write(mp3_content)
                uc_file.close()
                mp3_file.close()
                print('success %s' % mp3_file_name)

    def get_songid_by_filename(self, file_name):
        match_inst = re.match(r'\d*', file_name)  # -前面的数字是歌曲ID，例：1347203552-320-0aa1
        if match_inst:
            return match_inst.group()
        return ''

    def get_song_info(self, song_id):
        pattern = re.compile(r'<script "type=application/ld\+json(.*?)</a>')
        if not song_id:
            return str(song_id), ''

        try:
            url = 'https://music.163.com/song/'  # 请求url例子：https://api.imjad.cn/cloudmusic/?type=detail&id=1347203552
            headers = {
                'User-Agent': 'Mozilla / 5.0(WindowsNT10.0;Win64;x64) AppleWebKit / 537.36(KHTML, likeGecko) Chrome '
                              '/ 97.0.4692.99Safari / 537.36'
            }
            payload = {'id': song_id, 'userid': '426824820'}
            reqs = requests.get(url, params=payload, headers=headers)
            print(reqs.text)
            jsons = reqs.json()
            song_name = jsons['songs'][0]['name']
            singer = jsons['songs'][0]['ar'][0]['name']
            return song_name, singer
        except:
            return str(song_id), ''


def check_path():
    global UC_PATH, MP3_PATH

    if not os.path.exists(UC_PATH):
        print('缓存路径错误: %s' % UC_PATH)
        return False
    if not os.path.exists(MP3_PATH):
        print('目标路径错误: %s' % MP3_PATH)
        return False

    if UC_PATH[-1] != '/':  # 容错处理 防止绝对路径结尾不是/
        UC_PATH += '/'
    if MP3_PATH[-1] != '/':
        MP3_PATH += '/'
    return True


if __name__ == '__main__':
    # if not check_path():
    #     exit()

    transform = Transform()
    # transform.do_transform()
    transform.get_song_info(11111)
