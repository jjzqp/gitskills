# -*- coding: UTF-8 -*-

'''
api doc url:
http://ai.baidu.com/docs#/TTS-Online-Python-SDK/top
'''
from aip import AipSpeech

""" 你的 APPID AK SK """
APP_ID = '11410669'
API_KEY = 'htLn2nSqPp8HUTzbsyrAcDKh'
SECRET_KEY = '2dpAFHFfZQvYknzHUhxItt9TUy8VdU9R'

client = AipSpeech(APP_ID,API_KEY,SECRET_KEY)


'''
文本必须utf-8编码,长度小于1024字节
'''
strings = ""
with open("voice.txt",'r') as f0:
    strings = f0.read()
    print strings


result = client.synthesis(strings,'zh',1,{'vol':5,'per':4,})

#正确则返回语音二进制
if not isinstance(result,dict):
    with open("test.mp3",'wb') as f:
        f.write(result)
else:
    print "transfrom err!!!\n"
    print result