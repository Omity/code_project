# coding=utf-8
import pyvisa  # 接口库文件
import time
import os
import math

'''处理文件创建地址，名称以及设置功率计方式'''
CreatFileAddress = "D:\Cal\datacheck"
CreatFileName    = "datacheck"
SetCenterFreq    = ":FREQ" + " %f"
DetectionValue   =":MEAS?"

'''处理类封装中关于阈值阀门与单调性的const，以及其他初始设置值的问题'''
Zero       = 0
ZeroPosTwo = 0.2
ZeroNegTwo = -0.2
ZeroNegOne = -0.1
ZeroPosOne = 0.1
wavetest = ["SINIQ"]
GetBackAmp = []#创建初始获取回读值list
list1      = []
newlist    = []
value_mhz  = 1000000
'''被控仪器地址与测量仪器地址设置'''
ControllInstrumentAddress = "TCPIP::172.18.9.2::INSTR"
MeasureInstrumentAddress  = "TCPIP::172.18.9.229::INSTR"
# ControllInstrumentAddress = "TCPIP::172.18.8.160::INSTR"
# MeasureInstrumentAddress  = "TCPIP::172.18.8.33::INSTR"



'''接口设置'''
rm = pyvisa.ResourceManager()
print(rm.list_resources())
awg = rm.open_resource(ControllInstrumentAddress)
awg.write_termination = "\n"

#解决关于输入指令存在\n的问题，后续所有指令均不需要添加\n
awg.read_termination = pyvisa.resources.GPIBInstrument.LF
awg.timeout = 5000
pm = rm.open_resource(MeasureInstrumentAddress)  # 功率计或者频率计或者万用表地址
pm.write_termination = "\n"
#解决关于输入指令存在\n的问题，后续所有指令均不需要添加\n
pm.read_termination = pyvisa.resources.GPIBInstrument.LF
pm.timeout = 10000  #功率计



#创建类，使用该类实现两值大小比较，Threshold为阈值判定。pos,大于有效True；neg，小于有效True
class CompareImputValue:
    def __init__(self, a, b):
        self.a = a
        self.b = b
    def value_pos(self, Threshold):#所有大于的值均采用该类中的此函数，大于时，返回值为True
        a = self.a
        b = self.b
        if a - b > Threshold:
            return True
        else:
            return False
    def value_neg(self, Threshold):#所有小于的值均采用该类中的此函数，小于时，返回值为True
        a = self.a
        b = self.b
        if (a - b) < Threshold:
            return True
        else:
            return False


'''解决opc问题，加速程序与输出幅度回读时间'''
def ProcessOpc(variable):#1为控制仪器，非1为测量仪器
    count  = 0#用于计数，当连续判定OPC的值不为1时，启动计数
    if variable == 1:
        Mach = awg
    else:
        Mach = pm
    opc = Mach.query("*opc?")
    time.sleep(0.5)
    while opc != str(1):
        time.sleep(0.01)
        opc = Mach.query("*opc?")
        if opc == str(1) or opc == '+1':#处理OPC发送指令后，返回值
            break
        elif count > 50:#连续50次查询OPC均不为1时，跳出查询，并打印错误
            print("进程错误！")
            break
        else:
            count += 1


'''创建基础波形的SCPI命令运行'''
def AwgWaveWorkStatus(CH):  # 复数波形创建测试版本
    awg.write("AWGControl:CHANnel%d:RUN:IMMediate"% (CH))
    ProcessOpc(1)
    awg.write("OUTPUT%d:STATE ON" % (CH))  # del,OUTPUT2



'''使用AD9162电流修改问题与NCO变化'''
def SetNcofreq(ncofreq):  # 通道幅值与偏移设置,区别复数与实数波形的问题
    awg.write(":CALibration%d:CFRequency %f" % (CH,ncofreq))




'''初始解决机器初始化的基本信息'''
def SetInitBaseInfo(wave,CH):#主要解决得实数波形得模式选择
    awg.write("BWAVeform:COMPile:NAME wavetest")  # sets name of the compiledwaveform from the Basic Waveform editor plug-in to “Basic_Waveform”
    awg.write("BWAVeform:FUNCtion %s" % wave)
    awg.write("BWAVeFORM:FREQuency 1000000")  # 设立基准的点频频率，便于保持设定的值
    awg.write("BWAVeFORM:CYCLe 1")  # 设立循环数，便于保持设定的值
    awg.write("BWAVeform:SRATe 5000000000")
    awg.write("BWAVeform:COMPile")
    time.sleep(1)
    awg.write("BWAVeform:COMPile:CASSign ON")
    awg.write("BWAVeform:COMPile:CHANnel CH%d,wavetest" % (CH))
    # awg.write("CLOCk:SRATe 5000000000")  # sets the sample rate of Clock
    awg.write("CLOCk:SRATe 3000000000")  # sets the sample rate of Clock
    awg.write("CLOCk:INCLock:FREQuency 6000000000")  # sets the Frequency of Clock时钟采样率输出到12GSa/s
    awg.write(":AWGControl:CHANnel%d:WMODe COMPlex" % CH)
    awg.write("SOURce%d:IQIMode I8X" % (CH))
    # awg.write("SOURce%d:IQIMode I4X" % (CH))
    awg.write(":OUTPut%d:PATH %s" % (CH, cpath))
    if cpath == "AC":
        awg.write(":SOUR%d:VOLT:AMPL 0.3" % (CH))
    elif cpath == "DCA":
        awg.write(":SOUR%d:VOLT:AMPL 0.6" % (CH))
    print("插值选择I4X，关闭sinc滤波！")


def DeleteWave():
    awg.write(":WLISt:WAVeform:DELete:ALL")
    del GetBackAmp[:]  # 清除本轮中已经储存在GetBackAmp中元素，释放空间进行下一轮储存

def RST(CH):
    if "DCH" == cpath:
        awg.write(":CALibration:SCALe:CURrent:CLEar CHAN%d" % CH)#清除通道上所有曲线上的AD9162线电流的校准项
        awg.write(":CALibration:DAC:ROLLoff:CLEar CHAN%d" %CH)
        awg.write(":CALibration:DAC:ROLLoff:CLEar:ALL CHAN%d" %CH)
        awg.write(":CALibration:DAC:ROLLoff:CLEar:DDR CHAN%d" % CH)
    elif "DCA" == cpath or "AC" == cpath:
        awg.write(":CALibration:AC:ATTE:CLEar CHAN%d" % CH)
        awg.write(":CALibration:AC:AFCC:CLEar CHAN%d,0" % CH)
        awg.write(":CALibration:AC:AFCC:CLEar CHAN%d,1" % CH)
        awg.write(":CALibration:AC:AFCC:CLEar CHAN%d,2" % CH)
        awg.write(":CALibration:AMP:ATTE:CLEar CHAN%d" % CH)
        awg.write(":CALibration:AMP:AFCC:CLEar CHAN%d,0" % CH)
        awg.write(":CALibration:AMP:AMP:CLEar CHAN%d" % CH)




def get_all(minfreq,maxfreq,step,CH):
    for wave in wavetest:
        SetInitBaseInfo(wave, CH)
        cpathadress = os.path.join(CreatFileAddress, CreatFileName)
        file_find = os.path.exists(cpathadress+cpath + str(CH)+"AWGPointTest.csv")
        if True == file_find:
            os.remove(cpathadress+cpath + str(CH)+"AWGPointTest.csv")
        file_name = cpath + str(CH)+"AWGPointTest.csv"
        fadr = os.path.abspath(os.path.join(os.path.dirname(__file__), CreatFileAddress))  # 创建文件的绝对路径，并且返回路径
        file_add1 = os.path.join(fadr, CreatFileName)
        file_add = os.path.join(file_add1, file_name)
        print(file_add)
        with open(file_add, "a") as file_obj:
            ncofreq = minfreq#保证起始频率在1000KHZ以上
            '''
            起始点为A，生成步进后为B
            Begin
                for freq ← 1M to 4000M,step = 10MHZ
                    do A\B\C\D ← Readback(creat(wave))
                    if  A>B
                        then C ← B + step
                        if C>B and C-B>0.2
                            then  write C
                            del A
                        elseif C<B and C-B<-0.1
                                then  write C
                                del A
                        elseif C<B and D>C and (|C-B|>0.1 or |D-C|>0.1)
                                then write C
                                del A
                        else  
                                del C
                                continue
                    elseif A<B
                            then C ← B + step
                            then D ← C + step
                            if C<B and C-B<-0.2
                                then  write C
                                del A
                            elseif C>B and C-B>0.1
                                    then  write C
                                    del A
                            elseif C>B and D<C and (|C-B|>0.1 or |D-C|>0.1)
                                    then write C
                                    del A
                            else  
                                del C
                                continue 
                end
            End
            '''
            for nco in range(ncofreq, (maxfreq) * value_mhz, step):
                #处理第一点有问题
                if nco == value_mhz/10:
                    print(str(nco/value_mhz) + "MHZ")
                    SetNcofreq(nco)
                    AwgWaveWorkStatus(CH)
                    awg.write(":CALibration:SCALe:CURrent:CODe CHAN%d,150" % (CH))
                    time.sleep(0.1)
                    SetNcofreq(nco*10)
                    AwgWaveWorkStatus(CH)
                    if cpath == "DCA":
                        awg.write(":CALibration:AMP:AMP:CODe CHAN%d,4" % (CH))
                        awg.write(":CALibration:SCALe:CURrent:CODe CHAN%d,150" % (CH))
                    elif cpath == "AC":
                        awg.write(":CALibration:AC:AMP:CODe CHAN%d,10" % (CH))
                        awg.write(":CALibration:SCALe:CURrent:CODe CHAN%d,150" % (CH))
                    else:
                        awg.write(":CALibration:SCALe:CURrent:CODe CHAN%d,150" % (CH))
                    ProcessOpc(1)
                    pm.write(SetCenterFreq % (nco*10))  # 设置功率计的中心频率，保持一致n

                else:
                    print(str(nco / value_mhz) + "MHZ")
                    SetNcofreq(nco)
                    AwgWaveWorkStatus(CH)
                    if cpath == "DCA":
                        awg.write(":CALibration:AMP:AMP:CODe CHAN%d,4" % (CH))
                        awg.write(":CALibration:SCALe:CURrent:CODe CHAN%d,150" % (CH))
                    elif cpath == "AC":
                        awg.write(":CALibration:AC:AMP:CODe CHAN%d,10" % (CH))
                        awg.write(":CALibration:SCALe:CURrent:CODe CHAN%d,150" % (CH))
                    else:
                        awg.write(":CALibration:SCALe:CURrent:CODe CHAN%d,150" % (CH))
                    ProcessOpc(1)
                    pm.write(SetCenterFreq % (nco))  # 设置功率计的中心频率，保持一致n
                read_value_last1 = pm.query(DetectionValue).strip()  # 获得光标幅度
                list1.append(read_value_last1)
                time.sleep(0.1)
                read_value_last2 = pm.query(DetectionValue).strip()  # 获得光标幅度
                list1.append(read_value_last2)
                time.sleep(0.1)
                read_value_last3 = pm.query(DetectionValue).strip()  # 获得光标幅度
                list1.append(read_value_last3)
                time.sleep(0.1)
                read_value_last4 = pm.query(DetectionValue).strip()  # 获得光标幅度
                list1.append(read_value_last4)
                time.sleep(0.1)
                read_value_last5 = pm.query(DetectionValue).strip()  # 获得光标幅度
                list1.append(read_value_last5)
                newlist = list(sorted((list1)))
                newlist.remove(newlist[-1])  # 去掉最高分
                newlist.remove(newlist[0])  # 去掉一个最低分
                read_value_last = (float(newlist[0])+float(newlist[1])+float(newlist[2])) /3.0
                del list1[:]
                del newlist[:]
                if nco/value_mhz >= 4000 and cpath == "DCH":
                    file_obj.write(str(nco / value_mhz) + ",")
                    file_obj.write(str(read_value_last) + ",")
                    file_obj.write("\n")
                    file_obj.flush()
                    continue
                elif nco/value_mhz >= 2800 and cpath == "DCA":
                    file_obj.write(str(nco / value_mhz) + ",")
                    file_obj.write(str(read_value_last) + ",")
                    file_obj.write("\n")
                    file_obj.flush()
                    continue
                elif nco/value_mhz >= 5000 and cpath == "AC":
                    file_obj.write(str(nco / value_mhz) + ",")
                    file_obj.write(str(read_value_last) + ",")
                    file_obj.write("\n")
                    file_obj.flush()
                    continue
                GetBackAmp.append(float(read_value_last))
                if len(GetBackAmp) == 1 :#判定amp列表中否具有两个以上的元素，一个元素不满足比较方案
                    if cpath =="AC":
                        file_obj.write(str(nco/(value_mhz)) + ",")
                    else:
                        file_obj.write(str(nco/(value_mhz/10)) + ",")
                    file_obj.write(str(read_value_last) + ",")
                    file_obj.write("\n")
                    file_obj.flush()
                    nco = nco + step
                    SetNcofreq(nco)
                    AwgWaveWorkStatus(CH)
                    if cpath == "DCA":
                        awg.write(":CALibration:AMP:AMP:CODe CHAN%d,4" % (CH))
                        awg.write(":CALibration:SCALe:CURrent:CODe CHAN%d,150" % (CH))
                    elif cpath == "AC":
                        awg.write(":CALibration:AC:AMP:CODe CHAN%d,10" % (CH))
                        awg.write(":CALibration:SCALe:CURrent:CODe CHAN%d,150" % (CH))
                    ProcessOpc(1)
                    pm.write(SetCenterFreq % (nco))  # 设置功率计的中心频率，保持一致n
                    read_value_last1 = pm.query(DetectionValue).strip()  # 获得光标幅度
                    list1.append(read_value_last1)
                    time.sleep(0.1)
                    read_value_last2 = pm.query(DetectionValue).strip()  # 获得光标幅度
                    list1.append(read_value_last2)
                    time.sleep(0.1)
                    read_value_last3 = pm.query(DetectionValue).strip()  # 获得光标幅度
                    list1.append(read_value_last3)
                    time.sleep(0.1)
                    read_value_last4 = pm.query(DetectionValue).strip()  # 获得光标幅度
                    list1.append(read_value_last4)
                    time.sleep(0.1)
                    read_value_last5 = pm.query(DetectionValue).strip()  # 获得光标幅度
                    list1.append(read_value_last5)
                    newlist = list(sorted((list1)))
                    newlist.remove(newlist[-1])  # 去掉最高分
                    newlist.remove(newlist[0])  # 去掉一个最低分
                    read_value_last = (float(newlist[0])+float(newlist[1])+float(newlist[2])) /3.0

                    del list1[:]
                    del newlist[:]
                    file_obj.write(str((nco) / value_mhz) + ",")
                    file_obj.write(str(read_value_last) + ",")
                    file_obj.write("\n")
                    file_obj.flush()
                    continue
                else:
                    if 2 == len(GetBackAmp) or 3 == len(GetBackAmp)  :
                       continue# 为了继续向amp列表中添加元素，判定一旦为两个元素，结束本轮继续判定进入到下一轮，使其元素变为三个
                    Get_monotonicity = CompareImputValue(float(GetBackAmp[1]), float(GetBackAmp[0]))#比较两个值的大小，获取单调性
                    value_present = CompareImputValue(float(GetBackAmp[2]), float(GetBackAmp[1])) #计算本轮amp[1]与amp[2]的差大小
                    value         = CompareImputValue(float(GetBackAmp[3]), float(GetBackAmp[2]))#计算当前的amp[3]与amp[2]差的大小值得问题
                    if(True == Get_monotonicity.value_pos(Zero) and 4 == len(GetBackAmp)):#当amp[0]<amp[1]时，此时初始单调性为增。并且满足list(amp)中元素为三
                        if(True == value_present.value_pos(Zero) and True == value_present.value_pos(ZeroPosTwo)):#当amp[1]<amp[2]，此时amp[2]与amp[1]的差与0.2大小，大于0.2，记录当前的输出频率
                            file_obj.write(str(nco/value_mhz) + ",")
                            file_obj.write(str(float(GetBackAmp[2])) + ",")
                            file_obj.write("\n")
                            file_obj.flush()
                            del GetBackAmp[0]#删除第一个元素
                        elif(True == value_present.value_neg(Zero) and True == value_present.value_neg(ZeroNegOne)):
                            file_obj.write(str(nco/value_mhz) + ",")
                            file_obj.write(str(float(GetBackAmp[2])) + ",")
                            file_obj.write("\n")
                            file_obj.flush()
                            del GetBackAmp[0]
                        elif(True == value_present.value_neg(Zero)) and True == value.value_pos(Zero) and \
                                ( True == value.value_pos(ZeroPosOne) or True == value_present.value_neg(ZeroNegOne)):
                            file_obj.write(str(nco / value_mhz) + ",")
                            file_obj.write(str(float(GetBackAmp[2])) + ",")
                            file_obj.write("\n")
                            file_obj.flush()
                            del GetBackAmp[0]
                        elif(True == value_present.value_pos(Zero)) and True == value.value_neg(Zero) and \
                                (True == value.value_neg(ZeroNegOne) or True == value_present.value_pos(ZeroPosOne)):
                            file_obj.write(str(nco / value_mhz) + ",")
                            file_obj.write(str(float(GetBackAmp[2])) + ",")
                            file_obj.write("\n")
                            file_obj.flush()
                            del GetBackAmp[0]

                        else:
                            del GetBackAmp[2]#如果增大区间之后不符合需求，则忽略本次的步进设置的回读幅度，继续增大幅度
                            continue
                    elif(True == Get_monotonicity.value_neg(Zero) and 4 == len(GetBackAmp)):
                        if (True == value_present.value_neg(Zero) and True == value_present.value_neg(ZeroNegTwo)):
                            file_obj.write(str(nco/value_mhz) + ",")
                            file_obj.write(str(float(GetBackAmp[2])) + ",")
                            file_obj.write("\n")
                            file_obj.flush()
                            del GetBackAmp[0]
                        elif (True == value_present.value_pos(Zero) and True == value_present.value_pos(ZeroPosOne)):
                            file_obj.write(str(nco/value_mhz) + ",")
                            file_obj.write(str(float(GetBackAmp[2])) + ",")
                            file_obj.write("\n")
                            file_obj.flush()
                            del GetBackAmp[0]
                        elif(True == value_present.value_pos(Zero) and True == value.value_neg(Zero)) and \
                                 (True == value_present.value_pos(ZeroPosOne) or True == value.value_neg(ZeroNegOne) ):
                            file_obj.write(str(nco / value_mhz) + ",")
                            file_obj.write(str(float(GetBackAmp[2])) + ",")
                            file_obj.write("\n")
                            file_obj.flush()
                            del GetBackAmp[0]
                        elif(True == value_present.value_neg(Zero) )and (True == value.value_pos(Zero)) and \
                                (True == value.value_pos(ZeroPosOne) or True == value_present.value_neg(ZeroNegOne)):
                            file_obj.write(str(nco / value_mhz) + ",")
                            file_obj.write(str(float(GetBackAmp[2])) + ",")
                            file_obj.write("\n")
                            file_obj.flush()
                            del GetBackAmp[0]
                        else:
                            del GetBackAmp[2]
                            continue
                    else:
                        continue



if __name__ == "__main__":
    CH = int(input("请输入选择通道："))
    cpath = input("请输入路径(例如DCH、DCA、AC):")
    while cpath != "DCA" and cpath != "AC"  and cpath != "DCH":
        cpath = input("请正确输入路径：")
        if cpath == "DCA" or cpath == "AC" or cpath != "DCH":
            break
    stepall = [5000000]
    DeleteWave()
    for step in stepall:
        RST(CH)
        if cpath == "DCA":
            get_all(100000, 2805, step, CH)  # cpath为路径选择，1代表起始频率，3500代表终止频率，1000000代表步进，CH代表通道选择
        elif cpath == "DCH":
            get_all(100000, 4005, step, CH)  # cpath为路径选择，1代表起始频率，3500代表终止频率，1000000代表步进，CH代表通道选择
        else:
            get_all(10000000, 5005, step, CH)  # cpath为路径选择，1代表起始频率，3500代表终止频率，1000000代表步进，CH代表通道选择
        awg.close()
