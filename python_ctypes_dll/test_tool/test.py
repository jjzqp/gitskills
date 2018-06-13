from ctypes import *
#dll = CDLL("DChiDV.dll")
dll = windll.LoadLibrary("DChiDV.dll")
ret = c_char_p('')
dll.Init_Device(c_char_p("iiii"),ret)
