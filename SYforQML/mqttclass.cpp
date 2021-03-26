#include "mqttclass.h"
#include <qtimer.h>
using namespace QMQTT;


mqttclass::mqttclass()
{
    c_all.client = new Client();
    c_all.qbytetonum = 1;
}

bool mqttclass::connet(QString cid,QString ip,QString topic_re)
{
    if(ip.isNull()){
        ip="mqtt服务器地址";
    }
    QHostAddress host("mqtt服务器地址");
    quint16 port = 1883;
    quint16 Qos = 1;

    c_all.client->setHost(host);//ip地址
    c_all.client->setPort(port);//端口号
    c_all.client->setWillQos(Qos);//Qos
    c_all.client->setClientId(cid);//客户端标识
    c_all.client->connectToHost();
    if(!c_all.client->isConnectedToHost()){
        if(topic_re == NULL){
            topic = "thirtyseven";
        }
        else {
            topic = topic_re;
        }
        qDebug()<<"client connect true";
        return true;
    }
    else{
        qDebug()<<"client connect false";
        return false;
    }
}

void mqttclass::connection(){
    connect(c_all.client,SIGNAL(subscribed(const QString&,const quint8)),this,SLOT(test()));
//    connect(c_all.client,SIGNAL(received(const QMQTT::Message&)),this,SLOT(client_Received(const QMQTT::Message&)));
    QTimer::singleShot(500,this,SLOT(contopic()));
}

void mqttclass::test(){
    qDebug("订阅成功");
}

QString mqttclass::contopic() //订阅主题
{

    c_all.client->subscribe(topic, 0);
    c_all.client->setWillTopic(topic);
    qDebug()<<c_all.client->willTopic();
    return c_all.client->willTopic();
}

void mqttclass::sendMessage(QJsonObject message)
{
    QJsonDocument document = QJsonDocument(message);
    QByteArray qbyte_message = document.toJson();
    QMQTT::Message messagesend(c_all.qbytetonum,"classroom",qbyte_message,0,false,false);
    qDebug()<<"发送数据:"<<c_all.client->publish(messagesend);
//    emit logSignal("abc","hahahahah");
    c_all.qbytetonum++;
}

void mqttclass::dconnect() //停止连接
{
//    c_all.client->disconnectFromHost();
}

//void mqttclass::sendSigal(){
//    emit logSignal("abc","hahahahah");
//}

//void mqttclass::client_Received(const QMQTT::Message &message)
//{
//    QString strPaylaod = message.payload();
//    qDebug()<<strPaylaod;
//    //反馈
//}
