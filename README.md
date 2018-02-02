# SmartCar

> 已经实现 单片机使用ESP8266模块，使单片机联网，nodejs写的一个socket server，iOS手机APP通过socket通信远控单片机，并且手机的操作数据日志存储到MySQL数据库！

## Server

> Nodejs写的一个socket server，运行在Nodejs环境下，在这个项目里面，起到的作用相当于桥梁的作用，代码对应的文件夹是“SmartCarServer”


## 单片机

> 使用ESP8266模块，使单片机联网，单片机使用的芯片STC15W4K61S4，单片机和模块之间数据通信是串口通信，串口1 引脚 3.6 P3.7，ESP8266和服务器使用socket通信！代码对应的文件夹是“SmartCarClient”

## iOS 手机APP

> iOS 手机APP通过socket连接Nodejs server，发送的数据是根据DOC文件夹里面的协议组装，目前已经实现手机控制开/关灯、开/关蜂鸣器！代码对应的文件夹是“SmartCarAPP”


做此项目纯粹自己爱好，若有更好的解决方案或改进，欢迎一起讨论学习！ guntertien@gmail.com
