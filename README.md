# MY TOOL

##第一个python项目
avi文件解析工具：

1. 将AVI视频文件解析分析成每帧的JPG图片
2. 使用方法：将AVI视频文件拷到当前目录，并命名为REC00000.AVI; 然后双击批处理,稍等片刻,解析出来的jpg图片文件在JPG文件夹里

## 264 裸流解析
h264裸流解析:

1. 分析 I帧 P帧 SPS PPS
2. 将裸流转成可播放的视频流


## Circular_buf
一个基于C语言循环buf的操作，可以移植linux

## PCM 数据转换程序

PCM 转换程序支持功能：

1. windows软件支持24bit pcm的播放
2. 32bit pcm转24bit，只需要取低24bit即可
3. 24bit pcm转32bit，24bit补一个byte（0x00）即可
4. 24bit和32bit pcm分离左右声道
5. 32bit pcm小端格式转大端格式