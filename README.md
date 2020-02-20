# 飘流幻境手游人宠转生摇点器
可能会被杀毒软件报HackTools
## MyRecv
### 使用
32位编译成dll，放到Roll.exe同一目录下
### 原理
Roll.exe给这个dll注入到游戏内存中，hook游戏程序使用的ws2_32.dll里面的recv函数。
当游戏接收到服务器传来的摇点数据后，保存到$HOME/documents/wlRoll/res中。
顺便使用WSADuplicateSocket复制socket给摇点器使用，socket保存到$HOME/documents/wlRoll/socket中。

## Roll
Qt程序，主要是GUI，顺带含有注入dll功能和send功能
获取到socket后，可给服务器发送摇点封包，然后对res进行判断是否继续摇。