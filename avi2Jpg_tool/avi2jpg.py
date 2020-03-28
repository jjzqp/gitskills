# -*- coding: utf-8 -*-

import os
import sys
import time
import binascii
import shutil


avi_name =  'REC00000.AVI'
def aviFile_rename():
    file_list = os.listdir(".")
    for file_name in file_list:
        if file_name.endswith("AVI"):
            os.rename(file_name,avi_name)
        
def str_rev(strings):
    s = strings[6:8]+strings[4:6]+strings[2:4]+strings[0:2]
    return s

def read_jpg_data(offs_begin,offs_end,num):
    num = '%d'%(num+1)
    jpg_file_name = "JPG/"+num+".jpg"
    f1 = open(jpg_file_name,'wb')
    f1.write(data[offs_begin:offs_end])
    f1.close()
  

if(os.path.exists("JPG")==False):
    os.mkdir("JPG") #创建jpg目录
else:
    for files in os.walk("JPG"):
        if(len(files)):
            shutil.rmtree("JPG")
            os.mkdir("JPG")
            break
        else:
            break
        

aviFile_rename() #将目录下的avi文件重命名
f = open(avi_name,'rb')
data = f.read()#读文件数据

'''
下面读idx块数据
'''
avih_pos = data.index('avih')#定位avih块位置
total_frames = binascii.b2a_hex(data[avih_pos+8+16 : avih_pos+8+16+4])
total_frames = int(str_rev(total_frames),16)#总帧数

movi_pos = data.index('movi')#定位movi块位置
#print movi_pos
pos = movi_pos+4
cur_frame = 0
start_time = time.time()
while True:
    if (data[pos:pos+4] == '00dc'):
        #print '00dc'
        jpeg_size =  binascii.b2a_hex(data[pos+4:pos+4+4])
        jpeg_size = int(str_rev(jpeg_size),16)
        jpeg_frame_begin = pos+4+4
        jpeg_frame_end = jpeg_frame_begin+jpeg_size
        #print jpeg_frame_begin
        #print jpeg_frame_end
        read_jpg_data(jpeg_frame_begin,jpeg_frame_end,cur_frame)
        cur_frame+=1
        #percent = 1.0 * cur_frame / total_frames * 100
        #sys.stdout.write('complete percent:%10.8s%s\r'%(str(percent),'%'))
        sys.stdout.write('=')
        sys.stdout.flush()
        if cur_frame>=total_frames:
            break
        data = data[jpeg_frame_end::]
        try:
            pos = data.index('00dc')
        except ValueError:
            break;  
    else:
        break;

end_time = time.time()
f.close()
print '\n'
print "ok %ds"%(end_time-start_time)




