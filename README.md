# MY TOOL
my test

##第一个python项目
avi文件解析工具：
将AVI视频文件解析分析成每帧的JPG图片
使用方法：
将AVI视频文件拷到当前目录，并命名为REC00000.AVI; 然后双击批处理,稍等片刻,解析出来的jpg图片文件在JPG文件夹里
## 264 裸流解析
h264裸流解析:

1. 分析 I帧 P帧 SPS PPS
2. 将裸流转成可播放的视频流

## PCM 转换程序

PCM 转换程序：

1. windows软件支持24bit pcm的播放
2. 32bit pcm转24bit，只需要取低24bit即可
3. 24bit pcm转32bit，24bit补一个byte（0x00）即可
4. 将需要转换的pcm放到相应的文件夹下，点击“点我.bat”批处理，生成output**.pcm即为生成结果