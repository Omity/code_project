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
import configparser

import requests

from PyQt5.QtWidgets import QWidget, QPushButton, QApplication, QSlider, QLabel, QStyleFactory, QToolTip, QMessageBox, \
    QMainWindow, QDesktopWidget, QHeaderView, QAbstractItemView, QTableView, QTableWidgetItem
from PyQt5.QtWidgets import QVBoxLayout, QHBoxLayout
from PyQt5.QtGui import QIcon, QFont, QPixmap, QMouseEvent
from PyQt5.QtMultimedia import QMediaPlayer, QMediaPlaylist, QMediaContent
from PyQt5.QtCore import QUrl, Qt, QTimer, QEvent, QPoint, QCoreApplication
from PyQt5 import QtNetwork, uic

from album_show import SongInfoCard
from get_online_song import MusicApi
from test5 import ProxyStyle
from mainwindow import Ui_MainWindow


# 变量声明
song_info_list = {
    'song': 'ハッピーでバッドな眠りは浅い', 'singer': '鎖那',
    'songUrl': '',
    'picUrl': 'http://p3.music.126.net/tLKBLSGTBgHKpazu83hZfw==/109951166724351491.jpg'
}

song_list = ['http://music.163.com/song/media/outer/url?id=569213220.mp3',
             ]


# 函数定义

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
        self.playState = False
        self.songDict = []
        self.searchAPI = MusicApi()
        self.picPix = QPixmap()

        if os.path.exists('setting.ini'):
            self.LoadSetting()
        else:
            curPath = os.path.dirname(os.path.realpath(__file__))
            self.__IMAGE = os.path.join(curPath, 'images')
            self.__MUSIC = os.path.join(curPath, 'music')
            self.playMode = self.__PLAY_MODE_LOOP
            self.voiceState = 1  # 0表示静音
            self.voice = 70
            self.setGeometry(480, 480, 480, 320)
            self.setWindowTitle('Music Player')

        QToolTip.setFont(QFont('microsoft Yahei', 10))  # 字体格式
        self.setWindowIcon(QIcon(f'{self.__IMAGE}/icon.png'))
        # self.setWindowOpacity(0.9)  # 设置窗口透明度
        # self.setAttribute(Qt.WA_TranslucentBackground)  # 设置窗口背景透明

        self.Init()
        self.SetPlayModeStyleSheet()

    def Init(self):
        self.LayoutInit()
        self.PlayerInit()
        self.BottomInit()

    def BottomInit(self):
        self.SongInfoInit()
        self.PlayButtonInit()
        self.SliderInit()

    def LayoutInit(self):
        self.mainLayout = QVBoxLayout(self)  # 主框架
        self.bottomHLayout = QHBoxLayout()  # 底部布局
        self.bottomLeftHLayout = QHBoxLayout()  # 左边歌曲名称布局
        self.btnAndSliderVLayout = QVBoxLayout()  # 播放按键和进度条布局
        self.bottomRightHLayout = QHBoxLayout()  # 底部右边音量布局

        self.mainLayout.addLayout(self.bottomHLayout, Qt.AlignBottom)
        self.bottomHLayout.addLayout(self.bottomLeftHLayout, Qt.AlignLeft)
        self.bottomHLayout.addLayout(self.btnAndSliderVLayout, Qt.AlignCenter)
        self.bottomHLayout.addLayout(self.bottomRightHLayout, Qt.AlignRight)

    def SongInfoInit(self):
        """
        歌曲信息标签
        :return:
        """
        self.songInfo = SongInfoCard(song_info_list, parent=self)
        self.bottomLeftHLayout.addWidget(self.songInfo)

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
        self.voiceBtn.installEventFilter(self)
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

        self.buttonHLayout.addWidget(self.playModeBtn)
        self.buttonHLayout.addWidget(self.backwardBtn)
        self.buttonHLayout.addWidget(self.playBtn)
        self.buttonHLayout.addWidget(self.forwardBtn)
        self.buttonHLayout.addWidget(self.voiceBtn)

    def PlayerInit(self):
        self.PlayListInit()
        self.player = QMediaPlayer()
        self.player.setPlaylist(self.playList)
        self.player.setVolume(self.voice)
        self.playList.currentIndexChanged.connect(lambda: self.SongChanged())

    def PlayListInit(self):
        self.playList = QMediaPlaylist()  # 播放列表
        # self.localPlayList = ScanLocalMusic(self.__MUSIC)  # 获取的是默认存放的音乐文件
        # for m in self.localPlayList:
        #     tempDict = {'song': m.split('\\')[-1].split('.')[0].split('-')[-1],
        #                 'singer': m.split('\\')[-1].split('.')[0].split('-')[0],
        #                 'songUrl': m,
        #                 'picUrl': ''}
        #     url = QUrl.fromLocalFile(m)
        #     self.playList.addMedia(QMediaContent(url))
        #     self.songDict.append(tempDict)
        self.onlinePlayList = self.searchAPI.GetMusicList('微风细雨')
        for m in self.onlinePlayList:
            tempDict = {'song': m['song'],
                        'singer': m['singer'],
                        'songUrl': m['songUrl'],
                        'picUrl': m['picUrl']}
            self.playList.addMedia(QMediaContent(QtNetwork.QNetworkRequest(QUrl(m['songUrl']))))
            self.songDict.append(tempDict)

    def SliderInit(self):
        # 播放进度条
        self.sliderHLayout = QHBoxLayout()
        self.sliderHLayout.setSpacing(0)
        self.btnAndSliderVLayout.addLayout(self.sliderHLayout)
        self.sliderTime = QSlider(Qt.Horizontal, self)
        self.sliderTime.setMinimum(0)
        self.sliderTime.setStyle(QStyleFactory.create('Fusion'))
        self.sliderTime.sliderMoved.connect(lambda: self.SliderTimePressed())
        self.sliderTime.sliderReleased.connect(lambda: self.player.setPosition(self.sliderTime.value()))
        self.sliderTime.setObjectName('Slider')

        self.leftStartTime = QLabel(time.strftime('%M:%S', time.localtime(self.player.position() / 1000)))
        self.rightDurationTime = QLabel(time.strftime('%M:%S', time.localtime(self.player.duration() / 1000)))

        # self.sliderTime.setStyleSheet("QSlider#Slider:hover{height: 5}"
        #                               "QSlider#Slider::handle{border: none}"
        #                               "QSlider#Slider::sub-page:horizontal{background-color: red}"
        #                               )

        self.sliderTime.setStyleSheet('QSlider::groove:horizontal {border: 0px solid #bbb;}'
                                      'QSlider::sub-page:horizontal { background: rgb(239, 41, 41); '
                                      'border-radius: 2px;margin-top:8px; margin-bottom:8px;}'
                                      'QSlider::add-page:horizontal { background: rgb(255,255, 255);'
                                      'border: 0px solid #777; border-radius: 2px; margin-top:9px; margin-bottom:9px;}'
                                      'QSlider::handle:horizontal { background: rgb(193,204,208); width: 5px;'
                                      ' border: 1px solid rgb(193,204,208); border-radius: 2px; margin-top:6px;'
                                      'margin-bottom:6px;}'
                                      'QSlider::handle:horizontal:hover { background: rgb(193,204,208); width: 10px; '
                                      'border: 1px solid rgb(193,204,208); border-radius: 5px; margin-top:4px;'
                                      'margin-bottom:4px;}'
                                      )

        # 音量进度条
        self.sliderVoice = QSlider()
        self.sliderVoice.setObjectName('Slider')
        self.sliderVoice.setRange(0, 100)
        self.sliderVoice.setValue(self.voice)
        self.sliderVoice.setStyle(QStyleFactory.create('Fusion'))
        self.sliderVoice.setVisible(False)
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
        self.bottomRightHLayout.addWidget(self.sliderVoice)
        # self.btnAndSliderVLayout.addWidget(self.sliderVoice)

    def SliderTimePressed(self):
        """
        滑动进度条
        :return:
        """
        self.timer.stop()
        self.leftStartTime.setText(time.strftime('%M:%S', time.localtime(self.sliderTime.value() / 1000)))
        self.timer.start()

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

        self.SaveSetting()

    def PlayModeChange(self):
        """
        切换播放模式
        :return:
        """
        if self.playMode == self.__PLAY_MODE_LOOP:
            self.playMode = self.__PLAY_MODE_SINGLE
            self.playList.setPlaybackMode(QMediaPlaylist.CurrentItemInLoop)
            self.playModeBtn.setToolTip('单曲循环 Ctrl+m')
            self.playModeBtn.setStyleSheet("QPushButton {icon: url('images/single.png')}"
                                           "QPushButton:hover {icon: url('images/single_hover.png')}")
        elif self.playMode == self.__PLAY_MODE_SINGLE:
            self.playMode = self.__PLAY_MODE_SEQUENTIAL
            self.playList.setPlaybackMode(QMediaPlaylist.Sequential)
            self.playModeBtn.setToolTip('顺序播放 Ctrl+m')
            self.playModeBtn.setStyleSheet("QPushButton {icon: url('images/sequential.png')}"
                                           "QPushButton:hover {icon: url('images/sequential_hover.png')}")
        elif self.playMode == self.__PLAY_MODE_SEQUENTIAL:
            self.playMode = self.__PLAY_MODE_RANDOM
            self.playList.setPlaybackMode(QMediaPlaylist.Random)
            self.playModeBtn.setToolTip('随机播放 Ctrl+m')
            self.playModeBtn.setStyleSheet("QPushButton {icon: url('images/random.png')}"
                                           "QPushButton:hover {icon: url('images/random_hover.png')}")
        elif self.playMode == self.__PLAY_MODE_RANDOM:
            self.playMode = self.__PLAY_MODE_LOOP
            self.playList.setPlaybackMode(QMediaPlaylist.Loop)
            self.playModeBtn.setToolTip('列表播放 Ctrl+m')
            self.playModeBtn.setStyleSheet("QPushButton {icon: url('images/loop.png')}"
                                           "QPushButton:hover {icon: url('images/loop_hover.png')}")

    def NextSong(self):
        """
        切换下一首歌
        :return:
        """
        # self.playList.setCurrentIndex(1)
        self.playList.next()
        print(self.songInfo.scrollTextWindow.songName)
        print(self.songInfo.scrollTextWindow.singerName)
        # print(self.player.currentMedia().canonicalUrl().path()[1:])

    def PreviousSong(self):
        """
        切换上一首歌
        :return:
        """
        self.playList.previous()

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
            self.voice = self.player.volume()  # 保存上一次的音量
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

    def eventFilter(self, obj, event):
        """
        重写按钮事件过滤
        :param obj:
        :param event:
        :return:
        """
        if obj == self.voiceBtn and event.type() == QEvent.HoverEnter:
            self.sliderVoice.setVisible(True)
        elif obj == self.voiceBtn and event.type() == QEvent.HoverLeave:
            self.sliderVoice.setVisible(False)
        return super().eventFilter(obj, event)

    def LoadSetting(self):
        config = configparser.ConfigParser()
        config.read('setting.ini')

        # 载入路径
        self.__IMAGE = config.get('PATH', 'images')
        self.__MUSIC = config.get('PATH', 'music')

        self.setWindowTitle(config.get('Default', 'title'))

        # 载入默认大小
        self.setGeometry(int(config.get('Default', 'start_x')),
                         int(config.get('Default', 'start_y')),
                         int(config.get('Default', 'width')),
                         int(config.get('Default', 'height')))

        # 载入播放模式
        self.voiceState = int(config.get('MainConfig', 'voice_state'))
        self.playMode = int(config.get('MainConfig', 'play_mode'))
        self.voice = int(config.get('MainConfig', 'volume'))
        # self.SetPlayModeStyleSheet()

    def SetPlayModeStyleSheet(self):

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

        if self.playMode == self.__PLAY_MODE_LOOP:
            self.playList.setPlaybackMode(QMediaPlaylist.Loop)
        elif self.playMode == self.__PLAY_MODE_SINGLE:
            self.playList.setPlaybackMode(QMediaPlaylist.CurrentItemInLoop)
        elif self.playMode == self.__PLAY_MODE_SEQUENTIAL:
            self.playList.setPlaybackMode(QMediaPlaylist.Sequential)
        elif self.playMode == self.__PLAY_MODE_RANDOM:
            self.playList.setPlaybackMode(QMediaPlaylist.Random)

    def SaveSetting(self):
        config = configparser.ConfigParser()

        config.add_section('PATH')
        config.set('PATH', 'images', self.__IMAGE)
        config.set('PATH', 'music', self.__MUSIC)

        config.add_section('Default')
        config.set('Default', 'start_x', str(self.geometry().x()))
        config.set('Default', 'start_y', str(self.geometry().y()))
        config.set('Default', 'width', str(self.geometry().width()))
        config.set('Default', 'height', str(self.geometry().height()))
        config.set('Default', 'title', self.windowTitle())

        config.add_section('MainConfig')
        config.set('MainConfig', 'voice_state', str(self.voiceState))
        config.set('MainConfig', 'play_mode', str(self.playMode))
        config.set('MainConfig', 'volume', str(self.voice))
        config.write(open('setting.ini', 'w'))

    def closeEvent(self, QCloseEvent):
        """
        重写关闭事件
        :param QCloseEvent:
        :return:
        """
        res = QMessageBox.question(self, 'tips', 'Are you sure to close?',
                                   QMessageBox.Yes | QMessageBox.No, QMessageBox.No)
        if res == QMessageBox.Yes:
            self.SaveSetting()
            QCloseEvent.accept()
        else:
            QCloseEvent.ignore()

    def SongChanged(self):
        """
        音乐切换时修改标签
        :return:
        """
        tempIndex = self.playList.currentIndex()
        self.songInfo.albumPic.setPixmap(QPixmap(self.ShowOnlinePic(
            self.songDict[tempIndex]['picUrl'])))
        self.songInfo.scrollTextWindow.songName = self.songDict[tempIndex]['song']
        self.songInfo.scrollTextWindow.singerName = self.songDict[tempIndex]['singer']
        self.songInfo.scrollTextWindow.resetIndex()
        self.songInfo.scrollTextWindow.adjustWindowWidth()

    def ShowOnlinePic(self, pic):
        """
        获取网络图片
        :param pic:
        :return:
        """
        req = requests.get(pic)
        self.picPix.loadFromData(req.content)
        return self.picPix

    def SearchOnlineSong(self):
        """
        获取网络音乐
        :return:
        """
		if self.ui.searchEdit.text() != '':
			searchText = self.ui.searchEdit.text()
			result = self.searchAPI.GetMusicList(searchText)

			for i in range(len(result)):
				row = self.ui.tableWidget.rowCount()
				self.ui.tableWidget.insertRow(row)
				item1 = QTableWidgetItem(result[i]['song'])
				item2 = QTableWidgetItem(result[i]['singer'])
				self.ui.tableWidget.setItem(row, 1, item1)
				self.ui.tableWidget.setItem(row, 2, item2)
				self.ui.tableWidget.item(row, 1).setToolTip(result[i]['song'])


class MusicPlayerMainWindow(QMainWindow):

    _startPos = None
    _endPos = None
    _isTracking = False

    def __init__(self, parent=None):
        super().__init__(parent)

        self.__PLAY_MODE_LOOP = 1
        self.__PLAY_MODE_SINGLE = 2
        self.__PLAY_MODE_SEQUENTIAL = 3
        self.__PLAY_MODE_RANDOM = 4

        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        # 一些固定参数及对象
        self.playState = False   # 默认启动时播放状态为暂停
        self.songDict = []
        self.searchAPI = MusicApi()    # 网易云音乐爬虫

        # 加载ini文件
        if os.path.exists('setting.ini'):
            self.LoadSetting()
        else:  # 第一次启动软件时的默认设置
            curPath = os.path.dirname(os.path.realpath(__file__))
            self.__IMAGE = os.path.join(curPath, 'images')
            self.__MUSIC = os.path.join(curPath, 'music')
            self.currentPlayMode = self.__PLAY_MODE_SEQUENTIAL
            self.voiceState = 1   # 0表示静音
            self.currentVoice = 70          # 音量大小
            self.lastVoice = 70             # 上一个音量
            self.currentSong = ''
            self.currentSinger = ''
            self.setGeometry(int((QDesktopWidget().width() - self.ui.widget_2.width()) / 2),
                             int((QDesktopWidget().height() - self.ui.widget_2.height()) / 2),
                             self.ui.widget_2.width(), self.ui.widget_2.height())

        self.PlayerInit()
        self.AlbumInit()
        self.SetAllStyleSheet()
        self.setWindowIcon(QIcon('./images/icon.ico'))
        self.setWindowFlags(Qt.FramelessWindowHint)  # 无边框

        if not os.path.exists('./cache'):   # 创建缓存区
            os.mkdir('./cache')

        # 按键功能连接
        self.ui.closeBtn.clicked.connect(lambda: self.close())
        self.ui.maximizeBtn.setDisabled(True)   # 暂时不用最大化功能
        self.ui.minimizeBtn.clicked.connect(lambda: self.showMinimized())
        self.ui.playBtn.clicked.connect(lambda: self.PlayStateChanged())
        self.ui.nextBtn.clicked.connect(lambda: self.playList.next())
        self.ui.previousBtn.clicked.connect(lambda: self.playList.previous())
        self.ui.playModeBtn.clicked.connect(lambda: self.PlayModeChanged())
        self.ui.volumeBtn.clicked.connect(lambda: self.VolumeMute())
        self.ui.volumeBtn.installEventFilter(self)
        # 播放进度条
        self.songTimer.timeout.connect(lambda: self.SongTimer())
        self.ui.playSlider.sliderMoved.connect(lambda: self.PlaySliderMoved())
        self.ui.playSlider.sliderReleased.connect(lambda: self.player.setPosition(self.ui.playSlider.value()))
        # 音量进度条
        self.ui.volumeSlider.sliderMoved.connect(lambda: self.SliderVoiceEvent(self.ui.volumeSlider.value()))
        self.ui.volumeSlider.installEventFilter(self)
        self.ui.widget_4.hide()   # 隐藏音量slider

        # 播放列表
        self.playList.currentIndexChanged.connect(lambda: self.SongChanged(self.playList.currentIndex()))

        # 搜索栏
        self.ui.searchEdit.setPlaceholderText('new lane')
        self.ui.searchEdit.editingFinished.connect(lambda: self.SearchOnlineSong())

        # 搜索列表
        self.ui.tableWidget.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)  # 所有列自动拉伸，充满界面
        self.ui.tableWidget.setSelectionMode(QAbstractItemView.SingleSelection)  # 设置只能选中一行
        self.ui.tableWidget.setEditTriggers(QTableView.NoEditTriggers)  # 不可编辑
        self.ui.tableWidget.setSelectionBehavior(QAbstractItemView.SelectRows)  # 设置只有行选中, 整行选中
        self.ui.tableWidget.resizeColumnsToContents()  # 设置列宽高按照内容自适应
        self.ui.tableWidget.resizeRowsToContents()  # 设置行宽和高按照内容自适应
        self.ui.tableWidget.verticalHeader().setVisible(False)  # 设置列标题隐藏（针对列标题纵向排列
        self.ui.tableWidget.setShowGrid(False)       # 无外框
        self.ui.tableWidget.horizontalHeader().setSectionsClickable(False)  # 表头无法点击
        # 单元格宽高设置
        self.ui.tableWidget.horizontalHeader().resizeSection(0, 100)
        # self.ui.tableWidget.setColumnWidth(0, 80)
        # self.ui.tableWidget.setRowHeight(0, 50)
        # self.ui.tableWidget.setHorizontalHeaderLabels(['操作', '标题', '歌手', '专辑', '时间'])

    def PlayStateChanged(self):
        """
        playBtn槽函数
        :return:
        """
        if self.playState:
            self.songTimer.stop()
            self.ui.playBtn.setStyleSheet('QPushButton{icon: url("images/pause.png")}')
            self.playState = False
            self.player.pause()  # 暂停音乐
            self.ui.playBtn.setToolTip("播放")
        else:
            self.songTimer.start(1000)
            self.ui.playBtn.setStyleSheet('QPushButton{icon: url("images/play.png")}')
            self.playState = True
            self.player.play()  # 播放
            self.ui.playBtn.setToolTip("暂停")

    def PlayModeChanged(self):
        """
        切换播放模式
        :return:
        """
        if self.currentPlayMode == self.__PLAY_MODE_LOOP:
            self.currentPlayMode = self.__PLAY_MODE_SINGLE
            self.playList.setPlaybackMode(QMediaPlaylist.CurrentItemInLoop)
            self.ui.playModeBtn.setToolTip('单曲循环播放')
            self.ui.playModeBtn.setStyleSheet("QPushButton {icon: url('images/single.png')}"
                                           "QPushButton:hover {icon: url('images/single_hover.png')}")
        elif self.currentPlayMode == self.__PLAY_MODE_SINGLE:
            self.currentPlayMode = self.__PLAY_MODE_SEQUENTIAL
            self.playList.setPlaybackMode(QMediaPlaylist.Sequential)
            self.ui.playModeBtn.setToolTip('顺序播放')
            self.ui.playModeBtn.setStyleSheet("QPushButton {icon: url('images/sequential.png')}"
                                           "QPushButton:hover {icon: url('images/sequential_hover.png')}")
        elif self.currentPlayMode == self.__PLAY_MODE_SEQUENTIAL:
            self.currentPlayMode = self.__PLAY_MODE_RANDOM
            self.playList.setPlaybackMode(QMediaPlaylist.Random)
            self.ui.playModeBtn.setToolTip('随机播放')
            self.ui.playModeBtn.setStyleSheet("QPushButton {icon: url('images/random.png')}"
                                           "QPushButton:hover {icon: url('images/random_hover.png')}")
        elif self.currentPlayMode == self.__PLAY_MODE_RANDOM:
            self.currentPlayMode = self.__PLAY_MODE_LOOP
            self.playList.setPlaybackMode(QMediaPlaylist.Loop)
            self.ui.playModeBtn.setToolTip('列表播放')
            self.ui.playModeBtn.setStyleSheet("QPushButton {icon: url('images/loop.png')}"
                                           "QPushButton:hover {icon: url('images/loop_hover.png')}")

        self.albumWidget.scrollTextWindow.songName = 'ハッピーでバッドな眠りは浅い'

    def PlaySliderMoved(self):
        """
        拖动play进度条时, 更新标签
        :return:
        """
        self.songTimer.stop()
        self.ui.startTimeLabel.setText(time.strftime('%M:%S', time.localtime(self.ui.playSlider.value() / 1000)))
        self.songTimer.start()

    def SongTimer(self):
        """
        管理进度条播放时间
        :return:
        """
        # self.sliderTime.setMinimum(0)
        self.ui.playSlider.setMaximum(self.player.duration())
        self.ui.playSlider.setValue(self.ui.playSlider.value() + 1000)

        self.ui.startTimeLabel.setText(time.strftime('%M:%S', time.localtime(self.player.position() / 1000)))
        self.ui.endTimeLabel.setText(time.strftime('%M:%S', time.localtime(self.player.duration() / 1000)))

        # 进度条满了之后回零
        if self.player.duration() == self.ui.playSlider.value():
            self.ui.playSlider.setValue(0)

    def SongChanged(self, idx):
        """
        切换音乐时修改标签
        :return:
        """
        dic = dict(self.songDict[idx])
        self.currentSong = dic['song']
        self.currentSinger = dic['singer']
        self.albumWidget.albumPic.setPixmap(self.ShowOnlinePic(dic))
        self.albumWidget.scrollTextWindow.songName = dic['song']
        self.albumWidget.scrollTextWindow.singerName = dic['singer']
        # self.albumWidget.scrollTextWindow.resetIndex()
        self.albumWidget.scrollTextWindow.adjustWindowWidth()
        self.albumWidget.scrollTextWindow.repaint()

    def SliderVoiceEvent(self, vol):
        """
        音量进度条滑动时间
        :param vol:
        :return:
        """
        self.player.setVolume(vol)
        self.currentVoice = vol
        if vol == 1:  # 音量为0
            self.ui.volumeBtn.setIcon(QIcon(f'{self.__IMAGE}/mute.png'))
            self.ui.volumeBtn.setToolTip('恢复音量')
            self.voiceState = 0
        elif vol > 1:
            self.ui.volumeBtn.setIcon(QIcon(f'{self.__IMAGE}/voice.png'))
            self.ui.volumeBtn.setToolTip('静音')
            self.voiceState = 1

    def VolumeMute(self):
        """
        音量按键
        :return:
        """
        if self.voiceState == 1:
            self.lastVoice = self.player.volume()  # 保存上一次的音量
            self.currentVoice = 0
            self.player.setVolume(0)
            self.ui.volumeSlider.setValue(0)
            self.voiceState = 0
            self.ui.volumeBtn.setIcon(QIcon(f'{self.__IMAGE}/mute.png'))
            self.ui.volumeBtn.setToolTip('恢复音量')
        elif self.voiceState == 0:
            self.voiceState = 1
            self.ui.volumeSlider.setValue(self.lastVoice)
            self.player.setVolume(self.lastVoice)
            self.currentVoice = self.lastVoice
            self.ui.volumeBtn.setIcon(QIcon(f'{self.__IMAGE}/voice.png'))
            self.ui.volumeBtn.setToolTip('静音')

    def PlayerInit(self):
        """
        初始化播放器
        :return:
        """
        self.PlayListInit()
        self.player = QMediaPlayer()
        self.player.setPlaylist(self.playList)
        self.player.setVolume(self.currentVoice)
        # self.playList.currentIndexChanged.connect(lambda: self.SongChanged())

        self.songTimer = QTimer()   # 音乐进度定时器

    def PlayListInit(self):
        """
        初始化播放列表
        :return:
        """
        self.playList = QMediaPlaylist()  # 播放列表
        # self.localPlayList = ScanLocalMusic(self.__MUSIC)  # 获取的是默认存放的音乐文件
        # for m in self.localPlayList:
        #     tempDict = {'song': m.split('\\')[-1].split('.')[0].split('-')[-1],
        #                 'singer': m.split('\\')[-1].split('.')[0].split('-')[0],
        #                 'songUrl': m,
        #                 'picUrl': ''}
        #     url = QUrl.fromLocalFile(m)
        #     self.playList.addMedia(QMediaContent(url))
        #     self.songDict.append(tempDict)
        self.onlinePlayList = self.searchAPI.GetMusicList('毛不易')
        for m in self.onlinePlayList:
            tempDict = {'song': m['song'],
                        'singer': m['singer'],
                        'songUrl': m['songUrl'],
                        'picUrl': m['picUrl']}
            self.playList.addMedia(QMediaContent(QtNetwork.QNetworkRequest(QUrl(m['songUrl']))))
            self.songDict.append(tempDict)

    def AlbumInit(self):
        """
        初始化当前音乐标签widget
        :return:
        """
        # self.albumWiget = SongInfoCard(self.songDict[self.playList.currentIndex()], parent=self)
        if self.currentSong == '' or self.currentSinger == '':      # 当前任何一个为空都会加载默认的
            self.albumWidget = SongInfoCard(self.songDict[0], parent=self)
            self.ui.endTimeLabel.setText(time.strftime('%M:%S', time.localtime(self.player.duration() / 1000)))
        else:
            for i in self.songDict:
                if i['song'] == self.currentSong and i['singer'] == self.currentSinger:
                    self.albumWidget = SongInfoCard(self.songDict[self.songDict.index(i)], parent=self)
                    self.playList.setCurrentIndex(self.songDict.index(i))
                    break
        self.ui.horizontalLayout_2.addWidget(self.albumWidget)

    def SetAllStyleSheet(self):
        """
        设置按键图标及播放选项
        :return:
        """
        # 默认加载模式, 由统一的变量管控,防止在初始化时出现不匹配的状态
        if self.playState:
            self.ui.playBtn.setIcon(QIcon(f'{self.__IMAGE}/play.png'))
            self.ui.playBtn.setToolTip('播放')
        else:
            self.ui.playBtn.setIcon(QIcon(f'{self.__IMAGE}/pause.png'))
            self.ui.playBtn.setToolTip('暂停')

        if self.currentPlayMode == self.__PLAY_MODE_LOOP:
            self.playList.setPlaybackMode(QMediaPlaylist.Loop)
            self.ui.playModeBtn.setIcon(QIcon(f'{self.__IMAGE}/loop.png'))
            self.ui.playModeBtn.setToolTip('列表播放')
            self.ui.playModeBtn.setStyleSheet("QPushButton:hover {icon: url('images/loop_hover.png')}")
        elif self.currentPlayMode == self.__PLAY_MODE_SINGLE:
            self.playList.setPlaybackMode(QMediaPlaylist.CurrentItemInLoop)
            self.ui.playModeBtn.setIcon(QIcon(f'{self.__IMAGE}/single.png'))
            self.ui.playModeBtn.setToolTip('单曲循环播放')
            self.ui.playModeBtn.setStyleSheet("QPushButton:hover {icon: url('images/single_hover.png')}")
        elif self.currentPlayMode == self.__PLAY_MODE_SEQUENTIAL:
            self.playList.setPlaybackMode(QMediaPlaylist.Sequential)
            self.ui.playModeBtn.setIcon(QIcon(f'{self.__IMAGE}/sequential.png'))
            self.ui.playModeBtn.setToolTip('顺序播放')
            self.ui.playModeBtn.setStyleSheet("QPushButton:hover {icon: url('images/sequential_hover.png')}")
        elif self.currentPlayMode == self.__PLAY_MODE_RANDOM:
            self.playList.setPlaybackMode(QMediaPlaylist.Random)
            self.ui.playModeBtn.setIcon(QIcon(f'{self.__IMAGE}/random.png'))
            self.ui.playModeBtn.setToolTip('随机播放')
            self.ui.playModeBtn.setStyleSheet("QPushButton:hover {icon: url('images/random_hover.png')}")
        else:
            QMessageBox.about(self, 'parameter invalid', '无效的播放模式,恢复默认状态')
            self.currentPlayMode = self.__PLAY_MODE_SEQUENTIAL

        # 音量按键
        if self.voiceState == 1:
            self.ui.volumeBtn.setIcon(QIcon(f'{self.__IMAGE}/voice.png'))
            self.ui.volumeBtn.setToolTip('静音')
            self.ui.volumeSlider.setValue(self.currentVoice)
        elif self.voiceState == 0:
            self.ui.volumeBtn.setIcon(QIcon(f'{self.__IMAGE}/mute.png'))
            self.ui.volumeBtn.setToolTip('恢复音量')
            self.ui.volumeSlider.setValue(0)
        else:
            QMessageBox.about(self, 'parameter invalid', '无效的音量状态, 恢复默认状态',)
            self.voiceState = 1
            self.currentVoice = 70

    @staticmethod
    def ShowOnlinePic(pic: dict):
        """
        获取网络图片
        :param pic:
        :return:
        """
        req = requests.get(pic['picUrl'])
        pix = QPixmap()
        pix.loadFromData(req.content)
        return pix

    def SearchOnlineSong(self):
        """
        搜索栏搜索歌曲
        :return:
        """
        if self.ui.searchEdit.text() != '':
            searchText = self.ui.searchEdit.text()
            result = self.searchAPI.GetMusicList(searchText)

            for i in range(len(result)):
                self.ui.tableWidget.setItem(i, 0, QTableWidgetItem(result[i]['song']))
                self.ui.tableWidget.setItem(i, 1, QTableWidgetItem(result[i]['singer']))

    def LoadSetting(self):
        """
        加载ini文件
        :return:
        """
        config = configparser.ConfigParser()
        config.read('setting.ini')

        # 载入路径
        self.__IMAGE = config.get('PATH', 'images')
        self.__MUSIC = config.get('PATH', 'music')

        # 载入默认大小
        self.setGeometry(int(config.get('Default', 'start_x')),
                         int(config.get('Default', 'start_y')),
                         int(config.get('Default', 'width')),
                         int(config.get('Default', 'height')))

        # 载入播放模式
        self.voiceState = int(config.get('MainConfig', 'voice_state'))
        self.currentPlayMode = int(config.get('MainConfig', 'play_mode'))
        self.currentVoice = int(config.get('MainConfig', 'volume'))
        self.lastVoice = int(config.get('MainConfig', 'volume'))

        # 载入最近播放
        self.currentSong = config.get('LastPlay', 'last_song')
        self.currentSinger = config.get('LastPlay', 'last_singer')

    def SaveSetting(self):
        """
        保存设置
        :return:
        """
        config = configparser.ConfigParser()

        config.add_section('PATH')
        config.set('PATH', 'images', self.__IMAGE)
        config.set('PATH', 'music', self.__MUSIC)

        config.add_section('Default')
        config.set('Default', 'start_x', str(self.geometry().x()))
        config.set('Default', 'start_y', str(self.geometry().y()))
        config.set('Default', 'width', str(self.geometry().width()))
        config.set('Default', 'height', str(self.geometry().height()))

        config.add_section('MainConfig')
        config.set('MainConfig', 'voice_state', str(self.voiceState))
        config.set('MainConfig', 'play_mode', str(self.currentPlayMode))
        config.set('MainConfig', 'volume', str(self.currentVoice))

        config.add_section('LastPlay')
        config.set('LastPlay', 'last_song', str(self.currentSong))
        config.set('LastPlay', 'last_singer', str(self.currentSinger))

        config.write(open('setting.ini', 'w'))

    def closeEvent(self, QCloseEvent):
        """
        关闭事件重写
        :param QCloseEvent:
        :return:
        """
        self.SaveSetting()  # 保存设置

    def mouseMoveEvent(self, e: QMouseEvent):  # 重写移动事件
        self._endPos = e.pos() - self._startPos
        self.move(self.pos() + self._endPos)

    def mousePressEvent(self, e: QMouseEvent):
        if e.button() == Qt.LeftButton:
            self._isTracking = True
            self._startPos = QPoint(e.x(), e.y())

    def mouseReleaseEvent(self, e: QMouseEvent):
        if e.button() == Qt.LeftButton:
            self._isTracking = False
            self._startPos = None
            self._endPos = None

    def eventFilter(self, obj, event):
        """
        重写按钮事件过滤
        :param obj:
        :param event:
        :return:
        """
        if obj == self.ui.volumeBtn and event.type() == QEvent.HoverEnter:
            self.ui.widget_4.show()
        if obj == self.ui.volumeSlider and event.type() == QEvent.HoverEnter:
            self.ui.widget_4.show()
        elif obj == self.ui.volumeSlider and event.type() == QEvent.HoverLeave:
            self.ui.widget_4.hide()
        # elif obj == self.ui.volumeBtn and event.type() == QEvent.HoverLeave:
        #     self.ui.widget_4.hide()
        return super().eventFilter(obj, event)


if __name__ == '__main__':
    app = QApplication(sys.argv)
    proxyStyle = ProxyStyle(app.style())
    app.setStyle(proxyStyle)

    # ui = MusicPlayer()
    ui = MusicPlayerMainWindow()
    ui.show()

    sys.exit(app.exec_())
