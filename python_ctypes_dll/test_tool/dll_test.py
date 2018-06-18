# -*- coding: UTF-8 -*-

from Tkinter import *
from ctypes import *

dll = windll.LoadLibrary("DChiDV.dll")
import Tkinter as tk
root = tk.Tk()
root.title("CrappyCalc")
root.geometry("600x520")

buttons = ['InitDevice', 'GetIDCode', 'WriteInfo','ReadInfo', 'SetMSDC', 'ReadBattery','ReadRes','SyncTime']



# set up GUI
showFrame = Frame(root)
showFrame.grid(row=2, column=2)#, sticky="nsew")
field_1_t = Text(showFrame, width=85,height=15, wrap='none')  # 设置滚动条-不换行
field_1_t.pack(fill="x", expand=1, side=RIGHT)
row = 1
col = 0
buttonFrame= Frame(root)
buttonFrame.grid(row=4, column=2)

def click_event(btn_index):
    cur_func = buttons[btn_index]
    field_1_t.insert(END,cur_func+'\n')
    ret = c_bool()
    iRet = c_void_p()
    if btn_index == 0:
        IDCode = c_char_p()
        ret = dll.Init_Device(IDCode,iRet)
        dll.printf("%d",iRet)

    elif btn_index == 1:
        IDCode = c_char_p("Hello")
        dll.printf("%s",IDCode)
        ret = dll.GetIDCode(IDCode,iRet)
        print iRet
    elif btn_index == 2:
        ret = dll.WriteZFYInfo(None,None,None)
    elif btn_index == 3:
        ret = dll.GetZFYInfo(None,None,None)
    elif btn_index == 4:
        ret = dll.SetMSDC(None,None)
    elif btn_index == 5:
        ret = dll.ReadDeviceBatteryDumpEnergy(None,None,None)
    elif btn_index == 6:
        ret = dll.ReadDeviceResolution(None,None,None,None)
    elif btn_index == 7:
        ret = dll.SyncDevTime(None,None)

for index,i in enumerate(buttons):
    action = lambda x=index: click_event(x)
    tk.Button(buttonFrame, text=i,width=20, height=4, relief='raised', command=action) \
        .grid(row=row, column=col)#, sticky='nesw', )
    col += 1
    if col > 1:
        col = 0
        row += 1

# RUNTIME
root.mainloop()


