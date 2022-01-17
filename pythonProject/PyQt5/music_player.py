#!/usr/bin/env python
# _*_ coding: utf-8 _*_
"""
源文件名：music_player.py
功能描述：
作者：ShuJie
版本：V0.1
创建时间：2022/1/13 21:36

修改历史：
修改时间：
版本号：
修改人：
修改内容：
"""

# 库引入
import sys

from PyQt5.QtWidgets import QWidget, QPushButton, QFrame, QApplication
from PyQt5.QtWidgets import QVBoxLayout, QHBoxLayout
from PyQt5.QtGui import QIcon


# 变量声明

# 函数定义

# 类定义

class MusicPlayer(QWidget):

    def __init__(self):
        super().__init__()
        self.__PLAY_MODE_SINGLE = 1
        self.__PLAY_MODE_SEQUENCE = 2
        self.__PLAY_MODE_RANDOM = 3

        self.InitUI()
        self.playState = True
        self.playMode = self.__PLAY_MODE_SEQUENCE

    def InitUI(self):
        self.CreatePlayButton()

    def CreatePlayButton(self):
        self.buttonHLayout = QHBoxLayout()
        self.setLayout(self.buttonHLayout)
        self.buttonHLayout.setSpacing(0)

        self.playBtn = QPushButton()
        self.playBtn.setIcon(QIcon('images/play.png'))
        self.playBtn.setToolTip('暂停')
        self.playBtn.setShortcut('space')
        self.forwardBtn = QPushButton()
        self.forwardBtn.setIcon(QIcon('images/forward.png'))
        self.forwardBtn.setToolTip('下一首 Ctrl+n')
        self.forwardBtn.setShortcut('Ctrl+n')
        self.backwardBtn = QPushButton()
        self.backwardBtn.setIcon(QIcon('images/backward.png'))
        self.backwardBtn.setToolTip('上一首 Ctrl+f')
        self.playModeBtn = QPushButton()
        self.playModeBtn.setIcon(QIcon('images/sequence.png'))
        self.playModeBtn.setToolTip('顺序播放 Ctrl+m')
        self.playModeBtn.setShortcut('Ctrl+m')

        self.playBtn.setObjectName('playButton')
        self.forwardBtn.setObjectName('playButton')
        self.backwardBtn.setObjectName('playButton')
        self.playModeBtn.setObjectName('playButton')

        self.playBtn.clicked.connect(self.PlayBtnState)
        self.playModeBtn.clicked.connect(self.PlayModeChange)

        self.setStyleSheet("QPushButton#playButton{width:auto}"
                           "QPushButton#playButton{height:auto}"
                           "QPushButton#playButton{background-color:white}"
                           "QPushButton#playButton{border-style: none}")

        self.playBtn.setChecked(True)
        self.forwardBtn.setChecked(True)
        self.backwardBtn.setChecked(True)
        self.playModeBtn.setChecked(True)

        self.forwardBtn.setStyleSheet("QPushButton:hover {icon: url('images/forward_hover.png')}")
        self.backwardBtn.setStyleSheet("QPushButton:hover {icon: url('images/backward_hover.png')}")
        self.playModeBtn.setStyleSheet("QPushButton:hover {icon: url('images/sequence_hover.png')}")

        self.buttonHLayout.addWidget(self.playModeBtn)
        self.buttonHLayout.addWidget(self.backwardBtn)
        self.buttonHLayout.addWidget(self.playBtn)
        self.buttonHLayout.addWidget(self.forwardBtn)

    def PlayBtnState(self):
        if self.playState:
            self.playBtn.setStyleSheet('QPushButton{icon: url("images/pause.png")}')
            self.playState = False
            self.playBtn.setToolTip("播放")
        else:
            self.playBtn.setStyleSheet('QPushButton{icon: url("images/play.png")}')
            self.playState = True
            self.playBtn.setToolTip("暂停")

    def PlayModeChange(self):
        if self.playMode == self.__PLAY_MODE_SEQUENCE:
            self.playMode = self.__PLAY_MODE_SINGLE
            self.playModeBtn.setToolTip('单曲循环 Ctrl+m')
            self.playModeBtn.setStyleSheet("QPushButton {icon: url('images/single.png')}"
                                           "QPushButton:hover {icon: url('images/single_hover.png')}")
        elif self.playMode == self.__PLAY_MODE_SINGLE:
            self.playMode = self.__PLAY_MODE_RANDOM
            self.playModeBtn.setToolTip('随机播放 Ctrl+m')
            self.playModeBtn.setStyleSheet("QPushButton {icon: url('images/random.png')}"
                                           "QPushButton:hover {icon: url('images/random_hover.png')}")
        elif self.playMode == self.__PLAY_MODE_RANDOM:
            self.playMode = self.__PLAY_MODE_SEQUENCE
            self.playModeBtn.setToolTip('顺序播放 Ctrl+m')
            self.playModeBtn.setStyleSheet("QPushButton {icon: url('images/sequence.png')}"
                                           "QPushButton:hover {icon: url('images/sequence_hover.png')}")


if __name__ == '__main__':
    app = QApplication(sys.argv)

    ui = MusicPlayer()
    ui.show()

    sys.exit(app.exec_())
