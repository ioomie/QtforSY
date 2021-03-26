### 说明

该笔记用于整个mqtt从安装到测试成功的一整个流程，该流程包括：

- Linux安装mqtt
- QT添加mqtt（Linux、Windows）
- 测试流程

### Linux安装mqtt

在linux上便于学习开发，直接使用代理服务器：Mosquitto

1. 引入mosquitto仓库并更新

   ```
   sudo apt-add-repository ppa:mosquitto-dev/mosquitto-ppa
   sudo apt-get update
   ```

2. 安装mosquitto包

   ```
   sudo apt-get install mosquitto
   ```

3. 安装mosquitto开发包

   ```
   sudo apt-get install mosquitto-dev
   ```

4. 安装mosquitto客户端

   ```
   sudo apt-get install mosquitto-clients
   ```

5. 测试运行

   ```
   sudo service mosquitto status
   ```

   ![image-20200903201052846](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200903201052846.png)

6. 一些基本的操作命令

   - 订阅主题

     ```
     mosquitto_sub -h ipaddress -t 主题 
     ```

   - 消息发布

     ```
     mosquitto_pub -h ipaddress -t 主题 -m 消息
     ```

### QT添加mqtt

首先两个版本都是用同一个git包：https://github.com/emqx/qmqtt/tree/master

1. Linux版本

   首先下载Qt，对于Linux的一个稳定版本，最好选择5.12.9 http://download.qt.io/archive/qt/5.12/

   ![image-20200905194046443](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200905194046443.png)

   当然这只是我个人的测试，Linux能支持的最新版本是为该版本。

   ![image-20200905194209371](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200905194209371.png)

   下载下来之后要给予权限：

   ```
   sudo chmod 777 qt-opensource-linux-x64-5.12.9.run
   ```

   之后直接点击进行安装

   在安装好测试完没有出现编译错误的情况下，开始安装mqtt库。从上方的地址中下载包 https://github.com/emqx/qmqtt/tree/master 解压：

   ```
   tar -zxvf mqtt的包名
   ```

   关于压缩和解压：

   ![image-20200905194956137](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200905194956137.png)

   解压后在文件内用Qt打开.pro文件进行编译，编译的要求如下：

   ![image-20200905200610380](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200905200610380.png)

   1. 构建配置选择Release

   2. 取消勾选Shadow build

   3. Build步骤中添加make install步骤

      ![image-20200905200747713](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200905200747713.png)

   编译完成后会生成一些文件，其中会用到的有：

   ![image-20200905201841463](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200905201841463.png)

   1. include

      将include中的文件复制到src的mqtt文件中

      ![image-20200905202040125](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200905202040125.png)

   2. 将整个mqtt文件夹复制到qt的gcc编译器中

      ![image-20200905202330641](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200905202330641.png)

   3. 复制lib中的链接文件到gcc的lib中

      ![image-20200905202504907](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200905202504907.png)

   4. 复制lib中的链接文件到gcc的bin中

      ![image-20200905202720621](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200905202720621.png)

   5. modules-inst文件夹

      ![image-20200905202826832](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200905202826832.png)

   6. modules文件夹![image-20200905203000100](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200905203000100.png)

   7. 因为头文件路径的问题会出现一些错误，这里使用最暴力的方式：将include中的头文件再复制一遍到上一级目录：![image-20200905203148755](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/\image-20200905203148755.png)

   至此linux上的mqtt模块安装完成

2. Windows

   对于Win上的操作，这里利用MinGW，同样也是上方的mqtt包 https://github.com/emqx/qmqtt/tree/master 

   下载后解压，并打开MinGW cd进到文件中并执行：

   1. qmake
   2. mingw32-make
   3. mingw32-make install

   然后进入文件夹将相关路径中的头文件复制到对应的编译器中的QtQmqtt文件夹中：

   ![image-20200905210531268](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200905210531268.png)

### 测试

两者的测试程序可以用以下方式

- 在.pro文件下

```
QT += qmqtt
```

- 在要调用的文件上：

  - 在Linux上：

    ```
    #include <mqtt/qmqtt.h>
    ```

  - 在Win上：

    ```
    #include "qmqtt.h"
    ```

在添加上述两行代码后如果能编译成功并运行，说明mqtt客户端安装成功。