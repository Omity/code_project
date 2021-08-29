import os
import time
import shutil
import chardet

'''
/******************************************************************************
函数名称: class CompareFile
创建时间: 2021年8月29日21:22:51
函数作者: ShuJie
函数内容: 用于对源文件和目标文件里面内容的不同比较，找出修改过的地方 
功能描述: 函数输出在out文件夹下，该文件是对源文件和目标文件的copy,每个文件夹下会对应源文件和目标
        文件的源码,分别带有s和t前缀,还有一个result.log文件用于描述文件的差异
******************************************************************************/
'''


class CompareFile:
    __sourceFileList = []
    __targetFileList = []
    __compareFileList = []
    __compareDirList = []
    __process = 0

    msgFileDiff = f'''/*******************************************************************************
文件名称: fileDiff.log
创建时间: {time.asctime(time.localtime(time.time()))}
文件作者: ShuJie
文件内容: 针对源文件和目标文件进行对比，输出两个文件夹下的不同文件，
            具体是否为编译产生或者其他原因,可以就本文件的内容进行查找
*******************************************************************************/'''

    def __init__(self, arg1, arg2):  # 初始化并将source和target文件夹的所有文件遍历
        self.file1 = arg1
        self.file2 = arg2
        try:
            os.mkdir('out')
        except FileExistsError:
            shutil.rmtree('out')
            os.mkdir('out')
        self.searchFileList(self.file1, self.file2)
        self.fileDiff()
        self.readMe()

    @staticmethod
    def createFolder(cPath):
        try:
            os.makedirs(cPath)
        except FileExistsError:
            shutil.rmtree(cPath)
            os.makedirs(cPath)

    def searchFileList(self, arg1, arg2):  # 遍历函数
        for root, dirs, file in os.walk(arg1):
            for name in file:
                self.__sourceFileList.append(os.path.join(root, name))
            for name in dirs:
                root = '\\'.join(root.split('\\')[root.split('\\')
                                 .index(self.file1.split('\\')[-1]):])
                tmp = '\\'.join(os.path.join(root, name).split('\\')[1:])
                if tmp not in self.__compareDirList:
                    self.__compareDirList.append(tmp)
        for root, dirs, file in os.walk(arg2):
            for name in file:
                self.__targetFileList.append(os.path.join(root, name))

    def fileDiff(self):  # 比较文件的差异 将编译导致的增删去除
        with open('out\\fileDiff.log', 'w') as f:
            f.write(self.msgFileDiff + '\n\n\n\n')
            for tar in self.__targetFileList:
                tarIndex = tar.split('\\').index(self.file2.split('\\')[-1])
                tarTmp = '\\'.join(tar.split('\\')[tarIndex + 1:])
                if (os.path.join(self.file1, tarTmp)) not in \
                        self.__sourceFileList:
                    tmp = tar.split('\\')
                    tPath = '\\'.join(tmp[0:-1]) + '\\'
                    f.write(tmp[-1] + ' not in source file!target path is : '
                            + tPath + '\n         target ---> source\n')
                else:
                    if tarTmp not in self.__compareFileList:
                        self.__compareFileList.append(tarTmp)
            for src in self.__sourceFileList:
                srcIndex = tar.split('\\').index(self.file2.split('\\')[-1])
                srcTmp = '\\'.join(src.split('\\')[srcIndex + 1:])
                if (os.path.join(self.file2, srcTmp)) not in \
                        self.__targetFileList:
                    tmp = src.split('\\')
                    sPath = '\\'.join(tmp[0:-1]) + '\\'
                    f.write(tmp[-1] + ' not in target file!source path is : '
                            + sPath + '\n        source ---> target\n')
                else:
                    if srcTmp not in self.__compareFileList:
                        self.__compareFileList.append(srcTmp)

    @staticmethod
    def fileExist(thisFile):
        if os.path.exists(thisFile):
            return True
        else:
            print(f'ERROR:打开文件{thisFile}\033[31m失败\033[0m!\n')
            return False

    @staticmethod
    def codeDetect(path):
        with open(path, 'rb') as f:
            d = f.read(4)
            code = chardet.detect(d)['encoding']
        return code

    @staticmethod
    def readFile(file):
        try:
            with open(file, 'r', encoding='utf-8') as f:     # 这里对编码后期需要调整，目前没有办法
                return f.readlines()
        except IOError:
            print('\033[31m读取失败!\033[0m\n')

    def compare(self):
        print('\033[32m----开始进行比较----\033[0m')
        for dirs in self.__compareDirList:
            self.createFolder('out\\' + dirs)
        for file in self.__compareFileList:
            if self.fileExist(self.file1 + '\\' + file) \
                    and self.fileExist(self.file2 + '\\' + file):
                nameTmp = file.split('\\')
                srcTmp = 's_' + nameTmp[-1]
                tarTmp = 't_' + nameTmp[-1]
                outTmp = 'out\\' + '\\'.join(nameTmp[0:-1])
                shutil.copy(self.file1 + '\\' + file, outTmp + '\\' + srcTmp)
                shutil.copy(self.file2 + '\\' + file, outTmp + '\\' + tarTmp)
                sTmp = self.readFile(self.file1 + '\\' + file)
                tTmp = self.readFile(self.file2 + '\\' + file)
                sList = [i for i in sTmp]
                tList = [x for x in tTmp]
                lines1 = len(sList)
                lines2 = len(tList)

                if lines1 < lines2:
                    sList[lines1:lines2 + 1] = ' ' * (lines2 - lines1)
                if lines2 < lines1:
                    tList[lines2:lines1 + 1] = ' ' * (lines1 - lines2)
                counter = 1
                for x in zip(sList, tList):
                    if x[0] == x[1]:
                        counter += 1
                        continue
                    if x[0] != x[1]:
                        result = '%s和%s第%s行不同, 内容为: %s --> %s' % \
                                 (self.file1 + '\\' + file, self.file2 + '\\'
                                  + file, counter, x[0].strip(), x[1].strip())
                        with open(outTmp + '\\' + 'result.log', 'w') as f:
                            f.write(result)
                        counter += 1
            self.__process = self.__compareFileList.index(file)
            self.processOn()
        print('\033[32m----已完成所有比较----\033[0m')

    @staticmethod
    def readMe():
        with open('out\\readme', 'w', encoding='utf-8') as f:
            details = '''该类有两个传参,第一个是源文件目录,第二个是目标文件目录,可以相对路径，\
            但是源文件和目标文件得在同一级下，
            输出物都保存在out文件下,以该类建立对象后,或者引用该类的compare函数,就可以完成功能.
            '''
            f.write(details)

    def processOn(self):
        tmp = round((self.__process + 1) / len(self.__compareFileList) * 100, 2)
        print(f'已完成比较, 当前进度 ====>  \033[31m{tmp}\033[0m% / 100%')

    def printf(self):
        print(self.__compareDirList)


a = CompareFile('C:\\Study_log\\project_code\\pythonProject\\source',
                'C:\\Study_log\\project_code\\pythonProject\\target')
a.compare()