#  使用fileDiff函数去除因编译或者删除原因导致的文件不存在

import os


class CompareFile:

    __sourceFileList = []
    __targetFileList = []

    def __init__(self, arg1, arg2):
        self.file1 = arg1
        self.file2 = arg2
        self.searchFileList(self.file1, self.file2)

    def searchFileList(self, arg1, arg2):
        for root, dirs, file in os.walk(arg1):
            for name in file:
                self.__sourceFileList.append(os.path.join(root, name))
        for root, dirs, file in os.walk(arg2):
            for name in file:
                self.__targetFileList.append(os.path.join(root, name))

    def fileDiff(self):
        with open('out\\sFileDiff.log', 'w') as f:
            for tar in self.__targetFileList:
                if tar not in self.__sourceFileList:
                    tmp = tar.split('\\')
                    tPath = '\\'.join(tmp[0:-1]) + '\\'
                    f.write(tmp[-1] + ' not in source file!target path is : ' + tPath
                            + '\n         target ---> source\n')
                    self.__targetFileList.remove(tar)
        with open('out\\tFileDiff.log', 'w') as f:
            for src in self.__sourceFileList:
                if src not in self.__targetFileList:
                    tmp = src.split('\\')
                    sPath = '\\'.join(tmp[0:-1]) + '\\'
                    f.write(tmp[-1] + ' not in source file!target path is : ' + sPath
                            + '\n        source ---> target\n')
                    self.__sourceFileList.remove(src)

    @staticmethod
    def fileExist(sFile, tFile):
        if os.path.exists(sFile) and os.path.exists(tFile):
            return True
        else:
            return False

    def compare(self):
        for num in range(len(self.__sourceFileList)):
            if self.fileExist(self.__sourceFileList[num], self.__targetFileList[num]):
                pass
            else:
                pass

        fp1 = open(self.file1, encoding='utf-8')
        fp2 = open(self.file2, encoding='utf-8')
        __private_list1 = [__i for __i in fp1]
        __private_list2 = [__x for __x in fp2]
        fp1.close()
        fp2.close()
        __private_lens1 = len(__private_list1)
        __private_lens2 = len(__private_list2)

        if __private_lens1 < __private_lens2:
            __private_list1[__private_lens1:__private_lens2 + 1] = ' ' * (__private_lens1 - __private_lens2)
        elif __private_lens2 < __private_lens1:
            __private_list2[__private_lens2:__private_lens1 + 1] = ' ' * (__private_lens1 - __private_lens2)
        else:
            pass

        counter = 1
        resultList = []
        for x in zip(__private_list1, __private_list1):
            if x[0] == x[1]:
                counter += 1
                continue
            if x[0] != x[1]:
                compares = '%s和%s第%s行不同, 内容为: %s --> %s' % \
                           (self.file1, self.file2, counter, x[0].strip(), x[1].strip())
                resultList.append(compares)
                counter += 1
        return resultList

    def listPrint(self):
        with open('sourceList.log', 'w') as f:
            for i in self.__sourceFileList:
                f.write(i+'\n')


a = CompareFile('test', 'venv')

a.listPrint()
a.fileDiff()
