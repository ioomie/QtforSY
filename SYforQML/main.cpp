#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "database.h"
#include "struct_all.h"
#include "mqttclass.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qRegisterMetaType<room_detil>("room_detil"); //注册数据类型

//    Database db; //创建数据库接口对象，打开数据库
//    db.DBconncet();

    qmlRegisterType<Database>("Database", 1,0, "Database");
    qmlRegisterType<mqttclass>("mqttclass", 1,0, "Mqttclass");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);


    return app.exec();
}
