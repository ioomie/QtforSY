#ifndef MQTTCLASS_H
#define MQTTCLASS_H

#include "qmqtt.h"
#include <QJsonObject>
#include <QJsonDocument>

class mqttclass: public QObject
{
    Q_OBJECT
public:
    mqttclass();

    struct mqttclient{
            QMQTT::Client * client;
            unsigned long qbytetonum;
        };
    mqttclient c_all;

//    Q_INVOKABLE void sendSigal();

private:
    QString topic;
//    mqttclass *c_all;

signals:
//    void logSignal(QString,QString);

public slots:
    bool connet(QString,QString,QString);
    QString contopic();
    void sendMessage(QJsonObject);
    void connection();
    void test();
    void dconnect();

//    void client_Received(const QMQTT::Message&);

};

#endif // MQTTCLASS_H
