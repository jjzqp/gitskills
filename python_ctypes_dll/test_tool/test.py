# -*- coding: UTF-8 -*-

from Tkinter import *
from ctypes import *

def button0():
    print "hello button0"

#dll = CDLL("DChiDV.dll")
dll = windll.LoadLibrary("DChiDV.dll")
#dll.Init_Device(None,None)


root = Tk()
root.title("DLL TEST")
root.geometry('300x1000')

# 通过command属性来指定Button的回调函数
#btn0 = Button(root, text='Hello Button',command=button0).pack()

#下面的relief = FLAT设置，就是一个Label了！！！指定样式
Button(root,text = 'hello button',relief=GROOVE,command=button0).pack()


''''' 
下例中使用了bind方法，它建立事件与回调函数（响应函数）之间的关系，每当产生<Enter>事件 
后，程序便自动的调用cb2，与cb1,cb3不同的是，它本身还带有一个参数----event,这个参数传递 
响应事件的信息。 
'''
def cb1():
    print('button1 clicked')


def printEventInfo(event):
    print('event.time = ', event.time)
    print('event.type = ', event.type)
    print('event.WidgetId = ', event.widget)
    print('event.KeySymbol = ', event.keysym)


def cb3():
    print('button3 clicked')


b1 = Button(root, text='Button1', command=cb1).pack()
b2 = Button(root, text='Button2')
b2.bind("<Enter>", printEventInfo)
b3 = Button(root, text='Button3', command=cb3).pack()
b2.pack()
b2.focus_set()

'''''5.指定Button的宽度与高度 
width:    宽度 
heigth:    高度 
使用三种方式： 
1.创建Button对象时，指定宽度与高度 
2.使用属性width和height来指定宽度与高度 
3.使用configure方法来指定宽度与高度 
'''

Button(root, text='30X1', width=30, height=2).pack()

b4 = Button(root, text='30X2')
b4['width'] = 30
b4['height'] = 3
b4.pack()

b5 = Button(root, text='30X2')
b5.configure(width=30, height=2)
b5.pack()

# 上述的三种方法同样也适合其他的控件

'''''设置Button文本在控件上的显示位置 
anchor： 
使用的值为:n(north),s(south),w(west),e(east)和ne,nw,se,sw，就是地图上的标识位置了，使用 
width和height属性是为了显示各个属性的不同。 

for a in ['n', 's', 'e', 'w', 'ne', 'nw', 'se', 'sw']:
    Button(root,text="anchor",anchor=a,width=20,height=2).pack()

'''

'''''改变Button的前景色与背景色 
fg: 前景色 
bg：背景色 
'''

bfg = Button(root, text='change foreground', fg='red')
bfg.pack()

bbg = Button(root, text='change backgroud', bg='blue')
bbg.pack()

root.mainloop()
