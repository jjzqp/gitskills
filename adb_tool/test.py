import os
import time

#os.system("adb shell input tap %s %s" % (x,y))
cnt = 0
while True:
    os.system("adb.exe shell input touchscreen swipe 1000 1000 1000 500 1000")
    cnt+=1
    if cnt==25:
        cnt = 0
        time.sleep(1)
        os.system("adb.exe shell input keyevent 4") #back
        time.sleep(1)
        os.system("adb.exe shell input touchscreen swipe 1000 1000 1000 500 1000")
        os.system("adb.exe shell input tap 500 500") #touch
