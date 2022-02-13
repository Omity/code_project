#!/usr/bin/env python
# _*_ coding: utf-8 _*_
"""
源文件名：album_show.py
功能描述：
作者：ShuJie
版本：V0.1
创建时间：2022/2/10 20:57

修改历史：
修改时间：
版本号：
修改人：
修改内容：
"""

# 库引入
import sys

import requests
from PyQt5.QtCore import Qt, QTimer
from PyQt5.QtGui import QFont, QFontMetrics, QPainter, QPixmap
from PyQt5.QtWidgets import QApplication, QLabel, QWidget, QStyle


# 变量声明

# 函数定义

# 类定义


class ScrollTextWindow(QWidget):
    """ 滚动字幕 """

    def __init__(self, songName, singerName, parent=None):
        super().__init__(parent)
        self.songName = songName
        self.singerName = singerName
        # 实例化定时器
        self.timer = QTimer(self)
        # 设置刷新时间和移动距离
        self.timeStep = 20
        self.moveStep = 1
        self.songCurrentIndex = 0
        self.singerCurrentIndex = 0
        # 设置字符串溢出标志位
        self.isSongNameAllOut = False
        self.isSingerNameAllOut = False
        # 设置两段字符串之间留白的宽度
        self.spacing = 20  # 25
        # 初始化界面
        self.initWidget()

    def initWidget(self):
        """ 初始化界面 """
        self.setFixedHeight(60)
        self.setMaximumSize(100, 60)
        self.setStyleSheet('background-color: white')
        self.setAttribute(Qt.WA_StyledBackground)  # 设置父类样式不影响子类
        # 调整窗口宽度
        self.adjustWindowWidth()
        # 初始化定时器
        self.timer.setInterval(self.timeStep)
        self.timer.timeout.connect(self.updateIndex)
        # 只要有一个字符串宽度大于窗口宽度就开启滚动
        if self.isSingerNameTooLong or self.isSongNameTooLong:
            self.timer.start()

    def getTextWidth(self):
        """ 计算文本的总宽度 """
        songFontMetrics = QFontMetrics(QFont('Microsoft YaHei', 7))
        self.songNameWidth = songFontMetrics.width(self.songName)
        singerFontMetrics = QFontMetrics(QFont('Microsoft YaHei', 7))
        self.singerNameWidth = singerFontMetrics.width(self.singerName)

    def adjustWindowWidth(self):
        """ 根据字符串长度调整窗口宽度 """
        self.getTextWidth()
        # maxWidth = max(self.songNameWidth, self.singerNameWidth)
        # 判断是否有字符串宽度超过窗口的最大宽度
        self.isSongNameTooLong = self.songNameWidth > 50
        self.isSingerNameTooLong = self.singerNameWidth > 50
        # 设置窗口的宽度
        # self.setFixedWidth(min(maxWidth, 100))

    def updateIndex(self):
        """ 更新下标 """
        self.update()
        self.songCurrentIndex += 1
        self.singerCurrentIndex += 1
        # 设置下标重置条件
        resetSongIndexCond = self.songCurrentIndex * self.moveStep >= self.songNameWidth + self.spacing * \
                             self.isSongNameAllOut
        resetSingerIndexCond = self.singerCurrentIndex * \
                               self.moveStep >= self.singerNameWidth + self.spacing * self.isSingerNameAllOut
        # 只要条件满足就要重置下标并将字符串溢出置位，保证在字符串溢出后不会因为留出的空白而发生跳变
        if resetSongIndexCond:
            self.songCurrentIndex = 0
            self.isSongNameAllOut = True
        if resetSingerIndexCond:
            self.singerCurrentIndex = 0
            self.isSingerNameAllOut = True

    def paintEvent(self, e):
        """ 绘制文本 """
        # super().paintEvent(e)
        painter = QPainter(self)
        painter.setPen(Qt.black)
        # 绘制歌名
        painter.setFont(QFont('Microsoft YaHei', 7))
        if self.isSongNameTooLong:
            # 实际上绘制了两段完整的字符串
            painter.drawText(self.moveStep * self.songCurrentIndex - self.spacing * self.isSongNameAllOut, 20,
                             self.songName)
            # 绘制第二段字符串
            painter.drawText(self.moveStep * self.songCurrentIndex - self.songNameWidth - self.spacing *
                             (1 + self.isSongNameAllOut), 20, self.songName)
        else:
            painter.drawText(0, 20, self.songName)

        # 绘制歌手名
        if self.isSingerNameTooLong:
            painter.drawText(self.moveStep * self.singerCurrentIndex - self.spacing * self.isSingerNameAllOut, 40,
                             self.singerName)
            painter.drawText(self.moveStep * self.singerCurrentIndex - self.singerNameWidth - self.spacing *
                             (1 + self.isSingerNameAllOut), 40, self.singerName)
        else:
            painter.drawText(0, 40, self.singerName)

    def resetIndex(self):
        self.songCurrentIndex = 0
        self.singerCurrentIndex = 0


class SongInfoCard(QWidget):
    """ 播放栏左侧歌曲信息卡 """

    def __init__(self, songInfo: dict, parent=None):
        super().__init__(parent)
        # 保存信息
        self.songInfo = songInfo
        self.songName = self.songInfo['song']
        self.singerName = self.songInfo['singer']
        self.pix = QPixmap()
        # 实例化小部件
        self.albumPic = QLabel(self)
        self.albumPic.setFixedSize(60, 60)
        self.albumPic.setScaledContents(True)  # 图片自适应窗口大小
        self.scrollTextWindow = ScrollTextWindow(self.songName, self.singerName, self)
        # 初始化界面
        self.initWidget()

    def initWidget(self):
        """ 初始化小部件 """
        self.setFixedHeight(100)
        self.setFixedWidth(self.albumPic.width() + self.scrollTextWindow.width())
        self.setAttribute(Qt.WA_StyledBackground)
        # self.setWindowFlags(Qt.FramelessWindowHint)
        self.scrollTextWindow.move(self.albumPic.width(), 0)
        # self.albumPic.setPixmap(QPixmap(self.songInfo['album'][-1]).scaled(
        #     115, 115, Qt.KeepAspectRatio, Qt.SmoothTransformation))
        req = requests.get(self.songInfo['picUrl'])
        self.pix.loadFromData(req.content)
        self.albumPic.setPixmap(self.pix)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    thisSongInfo = {
        'song': 'ハッピーでバッドな眠りは浅い', 'singer': 'Jay-----------------',
        'songUrl': '',
        'picUrl': 'http://p3.music.126.net/XPPeIZu7wgcGXZ0666mfFg==/109951164640697307.jpg'}
    demo = SongInfoCard(thisSongInfo)
    demo.setStyleSheet('background:rgb(129,133,137)')
    demo.show()
    sys.exit(app.exec_())
