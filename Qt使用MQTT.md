### 关于MQTT协议的应用面理解

从实际入手，目前有三台主机：

- ubuntu虚拟机

  无局域网内的ip，但因为是虚拟机，可访问其它局域网内的主机

- centOS主机

  有局域网内的一个ip

- CC3200

  同Ubuntu虚拟机

经过真实的数据检测实践，其所搭建的通信工作过程如下：

​	在centOS上同时发布两个主题（分别用两个命令窗口）

```
mosquitto_sub -t topic
mosquitto_sub -t hello
```

​	在CC3200上分别关注centOS上发布的两个主题，对应的是topic主题指向led1的亮灭，hello主题指向的是led2的亮灭。

​	在Ubuntu虚拟机上开启mqtt的功能作测试

```
mosquitto -v
```

​	使用推送指令，目标主机ip为10.3.40.112，推送主题为topic，信息暂不做要求

```
mosquitto_pub -h 10.3.40.112 -t topic -m hi
```

​	CC3200能够接收到对应的主题号以及信息，因对信息不做要求，led1在对应主题下收到消息后并亮起。

​	同时对于主题hello操作一样：

```
mosquitto_pub -h 10.3.40.112 -t hello -m hi
```

​	CC3200在hello对应主题下收到消息后便会亮起led2

*在当前情况下是通过订阅不同主题上决定对应led灯的开关*

对于Qos：

![image-20200827202239698](https://raw.githubusercontent.com/ioomie/QtforSY/main/Photo_folder/image-20200827202239698.png)

使用Qos的第三种方式，确保消息发布能够只收到1次，也就是准确性。

### 关于Qt MQTT的使用

在应用上用于将板子和数据库搭建桥梁，直接读写数据库作监控。

此篇笔记以按照TCP的方式对其进行连接，只作对TCP连接方式的一个连接作解析笔记

首先在此要先明白协议的一个模型情况：

​	模型由三种身份构成：服务器、发布者（pub）、订阅者（sub）。发布者和订阅者都是客户端，意思为单个客户端是可以作数据的一个发送以及接收。

​	所谓订阅，在应用上意思为，订阅一个主题，这个主题并不是提前就有的。如上边所说的，订阅者在订阅这个主题后就会一直对该主题作一个监听。此时将视角转为发布者。在同一客户端上是可以订阅相关主之后对该主题作发布（当然在没订阅之前也是可以对该主题作发布的一个动作），因此当客户端在这个主题下发布了相关的指令比如：hello，则在这个主题下的所有订阅者都是可以接收到该指令。

​	一个客户端可以订阅多个主题，同时也可以在不同主题下发布，只要确保客户端和服务端是一个保持连接的状态，便可执行订阅和发布的状态。

#### 代码分析（应用层面）

首先在确保服务器开启了mqtt后，第一件事就是联机

git上的联机代码：

```c++
#include "qmqtt.h" //需要在project的pro文件上添加 QT += qmqtt方可使用该头文件
//#include <mqtt/qmqtt.h> //该头文件适用于linux系统

QMQTT::Client *client = new QMQTT::Client(QHostAddress::LocalHost, 1883);//创建客户端对象，这里我不采用这样的方式创建对象。
/*
	QMQTT::Client * client;//这句放在头文件
利用调用函数的形式确定ip和端口
    client->setHost(host);
    client->setPort(port);
当然也可以直接固定端口和ip，只将上边的代码稍作修改即可：
	client=new QMQTT::Client(QHostAddress(host),1883);//host是类型为QHostAddress的ip变量，可以直接手打：QHostAddress("192.168.88.128")
*/
//设定客户端的一个cid标识以及账号密码(账号密码部分还要进行深一步的研究，这一部分将会放在放在代码分析2中进行详说)
client->setClientId("clientId");
client->setUsername("user");
client->setPassword("password");
client->connectToHost();//连接客户端。
```

实际联机代码（轮子）：

```c++
QHostAddress host("10.3.40.113");
quint16 port = 1883;
quint16 Qos = 0;
QString cid1 = "client1-409-win";
client1->setHost(host);//ip地址
client1->setPort(port);//端口号
client1->setWillQos(Qos);//Qos
client1->setClientId(cid1);//客户端标识
client1->connectToHost();
//在这先前注意给客户端对象初始化
```

在实现连接后可以执行两个基本动作：订阅相关主题；在相关主题上发布消息。

​	订阅主题：

```c++
QString topic = "topic"; //该变量用于存储
client1->setWillTopic(topic);//遗嘱主题，意思为当客户端与服务器异常断开的时候会自动对该主题发送提前设定好的信息。其中Qos以及message都有默认配置，但是主题订阅不具备默认配置，所以在订阅主题之前或者之后要设定好一个遗嘱主题，用于报告断开的情况。否则将无法进行通信
client1->subscribe(topic, 0);//订阅主题方法
ui->label_7->setText(client1->willTopic());
```

​	发布消息：

```c++
quint16 id = 02;//设置消息id，不同消息的id要不用，可用于识别
QByteArray qbyte = QString::number(qbytetonum).toUtf8();//将消息或指令主体转为字节类型，编码格式utf8
QMQTT::Message message(id,"topic",qbyte);//制定message格式，有消息id，主题，消息主体。当然也可以设定qos
client1->publish(message);//客户端对象调用发送方法
```

