# -*- coding: utf-8 -*-

import os
import binascii

oneline_bytes = 16

def str_rev(strings):
    s = strings[6:8]+strings[4:6]+strings[2:4]+strings[0:2]
    return s
def read_jpg_data(offs_begin,offs_end,num):
    if(len(offs_end)==0):
        return
    #先转十进制
    offs_begin = int(offs_begin,16)
    offs_end = int(offs_end,16)
    #print "offs_begin",offs_begin
    #print "offs_end",offs_end
    #再写进去
    num = '%d'%(num+1)
    jpg_file_name = "JPG/"+num+".jpg"
    f1 = open(jpg_file_name,'wb')
    f1.write(data[offs_begin+8:offs_end])
    f1.close()

def get_oneframe_fromIdx(frame_cnt): 
    while True:
        fr = frame_cnt*oneline_bytes
        if(idx_data[fr:fr+4]=='00dc'):
            frame_begin=str_rev(binascii.b2a_hex(idx_data[fr+8:fr+12]))
            frame_end=str_rev(binascii.b2a_hex(idx_data[fr+8+oneline_bytes:fr+12+oneline_bytes]))
            if(frame_begin==frame_end):
                frame_cnt = frame_cnt + 1
                continue    
            return (frame_begin,frame_end)
        elif(idx_data[fr:fr+4]=='01wb'):
            frame_cnt = frame_cnt + 1
        
    
    

if(os.path.exists("JPG")==False):
    os.mkdir("JPG") #创建jpg目录
f = open('REC00000.AVI','rb')
data = f.read()#读文件数据

'''
下面读idx块数据
'''
movi_size = binascii.b2a_hex(data[504:508]) #读movi块大小
movi_size = str_rev(movi_size) #高位在前低位在后
#print movi_size
movi_size = int(movi_size,16)
idx_postion_start = movi_size + 0x000001FC
#print idx_postion_start
if(data[idx_postion_start:idx_postion_start+4] != 'idx1'):
    print 'is not indx1'
else:
    idx_size = binascii.b2a_hex(data[idx_postion_start+4:idx_postion_start+4+4]) #读idx块大小
    idx_size =  str_rev(idx_size)  #高位在前低位在后
    idx_size = int(idx_size,16)
    idx_postion_end = (idx_postion_start + 8 ) + idx_size
    #print idx_postion_end
    idx_data = data[idx_postion_start + 8:idx_postion_end] #保存idx块数据
totalframeNum = idx_size / oneline_bytes #根据idx块大小计算总帧数，包括了音频帧

#开始循环读帧保存为jpg#
for i in range(0,totalframeNum):
    (frameBegin,frameEnd)=get_oneframe_fromIdx(i)
    read_jpg_data(frameBegin,frameEnd,i)

f.close()

print "ok"




