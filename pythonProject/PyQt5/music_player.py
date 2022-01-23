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
import os
import time

from PyQt5.QtWidgets import QWidget, QPushButton, QApplication, QSlider, QLabel, QStyleFactory, QToolTip
from PyQt5.QtWidgets import QVBoxLayout, QHBoxLayout
from PyQt5.QtGui import QIcon, QFont
from PyQt5.QtMultimedia import QMediaPlayer, QMediaPlaylist, QMediaContent
from PyQt5.QtCore import QUrl, Qt, QTimer


# from pydub import AudioSegment
# 变量声明
# 函数定义

# # 将该路径文件从mp3转为wav
# def ChangeFormat(path):
#     new_path = path.replace('mp3', 'wav')
#     AudioSegment.converter = r'\\tools\\ffmpg.exe'
#     # 将mp3文件转换成wav
#     sound = AudioSegment.from_mp3(path)
#     sound.export(new_path, format="wav")

def ScanLocalMusic(path):
    """
    扫描本地文件夹里的音乐文件
    :param path: 本地文件夹
    :return:
    """
    tmp_list = []
    for root, dirs, files in os.walk(path):
        for file in files:
            if os.path.join(root, file) not in tmp_list:
                tmp_list.append(os.path.join(root, file))
    return tmp_list


# 类定义

class MusicPlayer(QWidget):

    def __init__(self):
        super().__init__()
        self.__PLAY_MODE_LOOP = 1
        self.__PLAY_MODE_SINGLE = 2
        self.__PLAY_MODE_SEQUENTIAL = 3
        self.__PLAY_MODE_RANDOM = 4
        self.__IMAGE = './images'
        self.__MUSIC = './music'
        self.playState = False
        self.playMode = self.__PLAY_MODE_LOOP
        self.voice = 70
        self.voiceState = 1      # 0表示静音

        self.setGeometry(480, 480, 480, 320)
        self.setWindowIcon(QIcon(f'{self.__IMAGE}/icon.png'))
        QToolTip.setFont(QFont('microsoft Yahei', 10))  # 字体格式

        # self.setWindowOpacity(0.9)  # 设置窗口透明度
        # self.setAttribute(Qt.WA_TranslucentBackground)  # 设置窗口背景透明

        self.Init()

    def Init(self):
        self.LayoutInit()
        self.PlayerInit()
        self.BottomInit()

    def BottomInit(self):
        self.PlayButtonInit()
        self.SliderInit()

    def LayoutInit(self):
        self.mainLayout = QVBoxLayout(self)   # 主框架
        self.btnAndSliderVLayout = QVBoxLayout()   # 播放按键和进度条布局

        self.mainLayout.addLayout(self.btnAndSliderVLayout)

    def PlayButtonInit(self):
        self.buttonHLayout = QHBoxLayout()
        self.buttonHLayout.setSpacing(0)
        self.buttonHLayout.setAlignment(Qt.AlignCenter)
        self.btnAndSliderVLayout.addLayout(self.buttonHLayout)

        self.playBtn = QPushButton()
        self.playBtn.setShortcut('space')
        self.forwardBtn = QPushButton()
        self.forwardBtn.setIcon(QIcon(f'{self.__IMAGE}/forward.png'))
        self.forwardBtn.setToolTip('下一首 Ctrl+n')
        self.forwardBtn.setShortcut('Ctrl+n')
        self.backwardBtn = QPushButton()
        self.backwardBtn.setIcon(QIcon(f'{self.__IMAGE}/backward.png'))
        self.backwardBtn.setToolTip('上一首 Ctrl+f')
        self.playModeBtn = QPushButton()
        self.playModeBtn.setShortcut('Ctrl+m')

        self.voiceBtn = QPushButton()
        self.voiceBtn.setIcon(QIcon(f'{self.__IMAGE}/voice.png'))
        self.voiceBtn.setToolTip('静音')

        self.playBtn.setObjectName('playButton')
        self.forwardBtn.setObjectName('playButton')
        self.backwardBtn.setObjectName('playButton')
        self.playModeBtn.setObjectName('playButton')
        self.voiceBtn.setObjectName('playButton')

        self.playBtn.clicked.connect(lambda: self.PlayBtnState())
        self.playModeBtn.clicked.connect(lambda: self.PlayModeChange())
        self.forwardBtn.clicked.connect(lambda: self.NextSong())
        self.backwardBtn.clicked.connect(lambda: self.PreviousSong())
        self.voiceBtn.clicked.connect(lambda: self.VoiceChange())
        self.playBtn.setChecked(True)
        self.forwardBtn.setChecked(True)
        self.backwardBtn.setChecked(True)
        self.playModeBtn.setChecked(True)
        self.voiceBtn.setChecked(True)

        # 统一的属性
        self.setStyleSheet("QPushButton#playButton{width:auto}"
                           "QPushButton#playButton{height:auto}"
                           "QPushButton#playButton{background-color: white}"
                           "QPushButton#playButton{border-style: none}")

        # 独立的属性单独提出来设置
        self.forwardBtn.setStyleSheet("QPushButton:hover {icon: url('images/forward_hover.png')}")
        self.backwardBtn.setStyleSheet("QPushButton:hover {icon: url('images/backward_hover.png')}")

        # 默认加载模式, 由统一的变量管控,防止在初始化时出现不匹配的状态
        if self.playState:
            self.playBtn.setIcon(QIcon(f'{self.__IMAGE}/play.png'))
            self.playBtn.setToolTip('播放')
        else:
            self.playBtn.setIcon(QIcon(f'{self.__IMAGE}/pause.png'))
            self.playBtn.setToolTip('暂停')

        if self.playMode == self.__PLAY_MODE_LOOP:
            self.playModeBtn.setIcon(QIcon(f'{self.__IMAGE}/loop.png'))
            self.playModeBtn.setToolTip('循环播放 Ctrl+m')
            self.playModeBtn.setStyleSheet("QPushButton:hover {icon: url('images/loop_hover.png')}")
        elif self.playMode == self.__PLAY_MODE_SINGLE:
            self.playModeBtn.setIcon(QIcon(f'{self.__IMAGE}/single.png'))
            self.playModeBtn.setToolTip('单曲播放 Ctrl+m')
            self.playModeBtn.setStyleSheet("QPushButton:hover {icon: url('images/single_hover.png')}")
        elif self.playMode == self.__PLAY_MODE_SEQUENTIAL:
            self.playModeBtn.setIcon(QIcon(f'{self.__IMAGE}/sequential.png'))
            self.playModeBtn.setToolTip('顺序播放 Ctrl+m')
            self.playModeBtn.setStyleSheet("QPushButton:hover {icon: url('images/sequential_hover.png')}")
        elif self.playMode == self.__PLAY_MODE_RANDOM:
            self.playModeBtn.setIcon(QIcon(f'{self.__IMAGE}/random.png'))
            self.playModeBtn.setToolTip('随机播放 Ctrl+m')
            self.playModeBtn.setStyleSheet("QPushButton:hover {icon: url('images/random_hover.png')}")

        self.buttonHLayout.addWidget(self.playModeBtn)
        self.buttonHLayout.addWidget(self.backwardBtn)
        self.buttonHLayout.addWidget(self.playBtn)
        self.buttonHLayout.addWidget(self.forwardBtn)
        self.buttonHLayout.addWidget(self.voiceBtn)

    def PlayerInit(self):
        self.play_list = QMediaPlaylist()  # 播放列表
        self.local_play_list = ScanLocalMusic(self.__MUSIC)  # 获取的是默认存放的音乐文件
        print(self.local_play_list)
        for m in self.local_play_list:
            url = QUrl.fromLocalFile(m)
            self.play_list.addMedia(QMediaContent(url))
        self.player = QMediaPlayer()
        self.player.setPlaylist(self.play_list)
        self.player.setVolume(self.voice)

        if self.playMode == self.__PLAY_MODE_LOOP:
            self.play_list.setPlaybackMode(QMediaPlaylist.Loop)
        elif self.playMode == self.__PLAY_MODE_SINGLE:
            self.play_list.setPlaybackMode(QMediaPlaylist.CurrentItemInLoop)
        elif self.playMode == self.__PLAY_MODE_SEQUENTIAL:
            self.play_list.setPlaybackMode(QMediaPlaylist.Sequential)
        elif self.playMode == self.__PLAY_MODE_RANDOM:
            self.play_list.setPlaybackMode(QMediaPlaylist.Random)

    def SliderInit(self):
        # 播放进度条
        self.sliderHLayout = QHBoxLayout()
        self.btnAndSliderVLayout.addLayout(self.sliderHLayout)
        self.sliderTime = QSlider(Qt.Horizontal, self)
        self.sliderTime.setMinimum(0)
        self.sliderTime.setStyle(QStyleFactory.create('Fusion'))
        self.sliderTime.sliderReleased.connect(lambda: self.player.setPosition(self.sliderTime.value()))
        self.sliderTime.setObjectName('Slider')

        self.leftStartTime = QLabel(time.strftime('%M:%S', time.localtime(self.player.position() / 1000)))
        self.rightDurationTime = QLabel(time.strftime('%M:%S', time.localtime(self.player.duration() / 1000)))

        self.sliderTime.setStyleSheet("QSlider#Slider:hover{height: 5}"
                                      "QSlider#Slider::handle{border: none}"
                                      "QSlider#Slider::sub-page:horizontal{background-color: red}"
                                      )

        # 音量进度条
        self.sliderVoice = QSlider()
        self.sliderVoice.setObjectName('Slider')
        self.sliderVoice.setRange(0, 100)
        self.sliderVoice.setValue(self.voice)
        self.sliderVoice.setStyle(QStyleFactory.create('Fusion'))
        self.sliderVoice.sliderMoved.connect(lambda: self.SliderVoiceEvent(self.sliderVoice.value()))
        # self.sliderVoice.sliderReleased.connect(lambda: self.SliderVoiceEvent(self.sliderVoice.value()))
        # self.sliderVoice.setStyleSheet("QSlider#Slider:hover{height: 5}"
        #                                "QSlider#Slider::handle{border: none}"
        #                                "QSlider#Slider::sub-page:horizontal{background-color: red}"
        #                                )

        # 进度条控制
        self.timer = QTimer()
        self.timer.timeout.connect(self.PlayerTimer)

        self.sliderHLayout.addWidget(self.leftStartTime)
        self.sliderHLayout.addWidget(self.sliderTime)
        self.sliderHLayout.addWidget(self.rightDurationTime)
        self.btnAndSliderVLayout.addWidget(self.sliderVoice)

    def PlayBtnState(self):
        if self.playState:
            self.timer.stop()
            self.playBtn.setStyleSheet('QPushButton{icon: url("images/pause.png")}')
            self.playState = False
            self.player.pause()  # 暂停音乐
            self.playBtn.setToolTip("播放")
        else:
            self.timer.start(1000)
            self.playBtn.setStyleSheet('QPushButton{icon: url("images/play.png")}')
            self.playState = True
            self.player.play()  # 播放
            self.playBtn.setToolTip("暂停")

    def PlayModeChange(self):
        """
        切换播放模式
        :return:
        """
        if self.playMode == self.__PLAY_MODE_LOOP:
            self.playMode = self.__PLAY_MODE_SINGLE
            self.play_list.setPlaybackMode(QMediaPlaylist.CurrentItemInLoop)
            self.playModeBtn.setToolTip('单曲循环 Ctrl+m')
            self.playModeBtn.setStyleSheet("QPushButton {icon: url('images/single.png')}"
                                           "QPushButton:hover {icon: url('images/single_hover.png')}")
        elif self.playMode == self.__PLAY_MODE_SINGLE:
            self.playMode = self.__PLAY_MODE_SEQUENTIAL
            self.play_list.setPlaybackMode(QMediaPlaylist.Sequential)
            self.playModeBtn.setToolTip('顺序播放 Ctrl+m')
            self.playModeBtn.setStyleSheet("QPushButton {icon: url('images/sequential.png')}"
                                           "QPushButton:hover {icon: url('images/sequential_hover.png')}")
        elif self.playMode == self.__PLAY_MODE_SEQUENTIAL:
            self.playMode = self.__PLAY_MODE_RANDOM
            self.play_list.setPlaybackMode(QMediaPlaylist.Random)
            self.playModeBtn.setToolTip('随机播放 Ctrl+m')
            self.playModeBtn.setStyleSheet("QPushButton {icon: url('images/random.png')}"
                                           "QPushButton:hover {icon: url('images/random_hover.png')}")
        elif self.playMode == self.__PLAY_MODE_RANDOM:
            self.playMode = self.__PLAY_MODE_LOOP
            self.play_list.setPlaybackMode(QMediaPlaylist.Loop)
            self.playModeBtn.setToolTip('列表播放 Ctrl+m')
            self.playModeBtn.setStyleSheet("QPushButton {icon: url('images/loop.png')}"
                                           "QPushButton:hover {icon: url('images/loop_hover.png')}")

    def NextSong(self):
        """
        切换下一首歌
        :return:
        """
        # self.play_list.setCurrentIndex(1)
        self.play_list.next()
        # print(self.play_list.nextIndex())
        # print(self.player.currentMedia().canonicalUrl().path()[1:])

    def PreviousSong(self):
        """
        切换上一首歌
        :return:
        """
        self.play_list.previous()

    def PlayerTimer(self):
        """
        管理进度条播放时间
        :return:
        """
        # self.sliderTime.setMinimum(0)
        self.sliderTime.setMaximum(self.player.duration())
        self.sliderTime.setValue(self.sliderTime.value() + 1000)

        self.leftStartTime.setText(time.strftime('%M:%S', time.localtime(self.player.position() / 1000)))
        self.rightDurationTime.setText(time.strftime('%M:%S', time.localtime(self.player.duration() / 1000)))

        # 进度条满了之后回零
        if self.player.duration() == self.sliderTime.value():
            self.sliderTime.setValue(0)

    def VoiceChange(self):
        """
        调整音量
        :return:
        """
        if self.voiceState == 1:
            self.voice = self.player.volume()   # 保存上一次的音量
            self.player.setVolume(0)
            self.sliderVoice.setValue(0)
            self.voiceState = 0
            self.voiceBtn.setIcon(QIcon(f'{self.__IMAGE}/mute.png'))
            self.voiceBtn.setToolTip('恢复音量')
        elif self.voiceState == 0:
            self.voiceState = 1
            self.sliderVoice.setValue(self.voice)
            self.player.setVolume(self.voice)
            self.voiceBtn.setIcon(QIcon(f'{self.__IMAGE}/voice.png'))
            self.voiceBtn.setToolTip('静音')

    def SliderVoiceEvent(self, vol):
        """
        音量进度条滑动时间
        :param vol:
        :return:
        """
        self.player.setVolume(vol)
        self.voice = vol
        if vol == 1:  # 音量为0
            self.voiceBtn.setIcon(QIcon(f'{self.__IMAGE}/mute.png'))
            self.voiceBtn.setToolTip('恢复音量')
            self.voiceState = 0
        elif vol > 1:
            self.voiceBtn.setIcon(QIcon(f'{self.__IMAGE}/voice.png'))
            self.voiceBtn.setToolTip('静音')
            self.voiceState = 1

        print(self.player.volume())


if __name__ == '__main__':
    app = QApplication(sys.argv)

    ui = MusicPlayer()
    ui.show()

    sys.exit(app.exec_())
