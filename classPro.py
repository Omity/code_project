import os


class CompareFile:

    def __init__(self, arg1, arg2):
        self.file1 = arg1
        self.file2 = arg2

    @staticmethod
    def getFList(rootPath):
        for __root, __dirs, __files in os.walk(rootPath):
            print('root_dir:', __root)
            print('sub_dirs:', __dirs)
            print('files:', __files)

    def fileExist(self):
        if os.path.exists(self.file1) and os.path.exists(self.file2):
            return True
        else:
            return False

    def compare(self):
        if not CompareFile(self.file1, self.file2).fileExist():
            return []

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


# fileForCmp = CompareFile('C:\\Study_log\\test', 'C:\\Study_log\tet')
# diff = fileForCmp.compare()
# with open('output.log', 'w') as f:
#     for i in diff:
#         f.write(i)
#         print(i)
def list_all_files(rootdir):
    import os
    _files = []
    _list = os.listdir(rootdir)  # 列出文件夹下所有的目录与文件
    for _i in range(0, len(_list)):
        path = os.path.join(rootdir, _list[_i])
        if os.path.isdir(path):
            _files.extend(list_all_files(path))
        if os.path.isfile(path):
            _files.append(path)
    return _files





a = getFList('.idea')
print(a)
