#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtSql>
#include "mqttclass.h"
#include "struct_all.h"
//#include <vector>
//using namespace std;

class Database: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QMap room MEMBER m_room)//获取教室列表
//    Q_PROPERTY(QString text MEMBER m_text)

public:
    Database();
    bool DBconncet(); //连接数据库
    QJsonObject datamix(QString,QString,int); //教室全名，用电器编号，用电器状态

private:
    QTimer*DBlisten;

    QTimer*firsttime_clock;
    QTimer*secondetime_clock;
    QTimer*thirdtime_clock;
    QTimer*endtime_clock;

    QMap<QString,QVariant> m_room;
//    QString test;
    mqttclass* c_db;

//    QString m_text;

signals:
    void sendcode(QJsonObject);
    void sendMessage();

    void logSignal(QString,QString);

public slots:
    void listen(); //时刻监听在发生变动后发送mqtt信号
//    QMap<QString,QVariant> getroom(); //获取所有的教室列表，用于点击选取
    void getroom();
    void timesend_clock(); //整点上锁|也可用于手动操作
    Q_INVOKABLE QVariantMap getroom_detil(QString id); //在点击后获取选中教室的信息
    void update(QString,QString,QString);
    void s_close(QString id);
//    QMap<QString,QVariant> getroom_re(QString);
    void getroom_re(QString);

    void timereset();

    void client_Received(const QMQTT::Message&);

};

#endif // DATABASE_H
