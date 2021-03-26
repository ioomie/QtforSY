#include "database.h"
#include <sys/stat.h>
#include <windows.h>
#include <qmap.h>

Database::Database()
{
    DBconncet();
    c_db = new mqttclass();
    c_db->connet("Client_db"," ","classroom");
    c_db->connection();
    connect(c_db->c_all.client,SIGNAL(received(const QMQTT::Message&)),this,SLOT(client_Received(const QMQTT::Message&)));
    connect(this,SIGNAL(sendcode(QJsonObject)),c_db,SLOT(sendMessage(QJsonObject)));
    DBlisten = new QTimer(this);

    connect(DBlisten,SIGNAL(timeout()),this,SLOT(listen()));
    firsttime_clock = new QTimer(this);
    secondetime_clock = new QTimer(this);
    thirdtime_clock = new QTimer(this);
    endtime_clock = new QTimer(this);
    timereset();
    DBlisten->start(1000);
}

bool Database::DBconncet()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL","dbone");
    db.setHostName("这里是数据库地址");
    db.setPort(3306);
    db.setDatabaseName("数据库名称");
    db.setUserName("用户");
    db.setPassword("密码");
    bool oprs = db.open();
    if (oprs){
        qDebug()<<"数据库打开成功";
        return true;
    }
    else {
        qDebug()<<"数据库打开失败，失败原因："<<db.lastError().text();
        return false;
    }
}

void Database::listen()//监听操作
{
/* 关于旧表操作
 * 在数据库中以旧表的方式跟原表作比较
 * 用于仿监听
 * 当前表数据改动后，旧表是不受前端方向改变的
 * 当监听过程中发现两个表有不同数据时会自动触发信号
 * 当信号发出后再将旧表同步更新到新表中
*/
//    qDebug()<<"监听行为启动";
    QSqlDatabase db = QSqlDatabase::database("dbone");
    QSqlQuery query(db);
    QSqlQuery query_old(db);
    query.exec("select * from room");
    query_old.exec("select * from room_old");

    while(query.next()&&query_old.next())
    {

        if(query.value("id") == query_old.value("id"))
        {
            QMap<QString,int> map;
            QMap<QString,int> map_old;

            map.insert("light_status",query.value("light_status").toInt());
            map.insert("lock_status",query.value("lock_status").toInt());
            map.insert("fan_status",query.value("fan_status").toInt());
            map.insert("ac_status",query.value("ac_status").toInt());

            map_old.insert("light_status",query_old.value("light_status").toInt());
            map_old.insert("lock_status",query_old.value("lock_status").toInt());
            map_old.insert("fan_status",query_old.value("fan_status").toInt());
            map_old.insert("ac_status",query_old.value("ac_status").toInt());

            QMap<QString,int>::Iterator mapif = map.begin();
            QMap<QString,int>::Iterator mapif_old = map_old.begin();

            while(mapif != map.end() && mapif_old != map_old.end())
            {
                if(mapif.key() == mapif_old.key())
                {
                    if(mapif.value() != mapif_old.value())
                    {
                        //修改旧表
//                        Sleep(1000);
                        //name代表教室的名称
                        //mapif.key()代表用电器标识
                        //mapif.value()代表对应的状态
                        emit sendcode(datamix(query.value("name").toString(),mapif.key(),mapif.value()));
                        QString changestr = QString("update room_old set %1=%2 where id=%3").arg(mapif.key()).arg(mapif.value()).arg(query.value("id").toInt());
                        query_old.exec(changestr);
//                        qDebug()<<"修改成功";
                    }
                }
                else
                {
                    qDebug()<<"对比顺序出问题";
                    break;
                }
                mapif++;
                mapif_old++;
            }
        }
        else
        {
            qDebug()<<"数据库表出现对应问题，请重新刷新状态储存表";
        }
        //emit sendcode(datamix(query.value("name").toString(),mapif.key(),mapif.value()));
    }
}

QJsonObject Database::datamix(QString room_id_all, QString e_id_old, int status)
{
    QString b_id; //教学楼号
    QString r_id; //教室号
    QString e_id; //用电器号
    QString status_string = QString::number(status,10);

    QJsonObject message;
    QMap<QString,QString> e_id_change;

//    qDebug()<<room_id_all.mid(4,2).toInt();//检查数据转换是否成功

    switch (room_id_all.mid(1,1).toInt()) {
    case 10: b_id = "A";break;
    default: b_id = QString::number(room_id_all.mid(1,1).toInt(),10);
    }

    switch (room_id_all.mid(4,2).toInt()) {
    case 10: r_id = "A";break;
    case 11: r_id = "B";break;
    case 12: r_id = "C";break;
    default: r_id = QString::number(room_id_all.mid(4,2).toInt(),10);
    }

    e_id_change.insert("light_status","0");
    e_id_change.insert("lock_status","1");
    e_id_change.insert("fan_status","2");
    e_id_change.insert("ac_status","3");

    QMap<QString,QString>::Iterator e_id_change_choose = e_id_change.begin();

    while (e_id_change_choose != e_id_change.end()) {
        if(e_id_old == e_id_change_choose.key())
        {
            e_id = e_id_change_choose.value();
        }
        e_id_change_choose++;
    }
    // A1-101 xxx 0|1
    message.insert("head",QJsonValue("*")); //json头部
    message.insert("b_char",QJsonValue(room_id_all.mid(0,1))); //教学楼的前缀
    message.insert("b_id",QJsonValue(b_id)); //教学楼的后缀
    message.insert("floor",QJsonValue(room_id_all.mid(3,1))); //教室所在楼层
    message.insert("r_id",QJsonValue(r_id)); //教室编号
    message.insert("e_id",QJsonValue(e_id)); //用电器类型
    message.insert("status",QJsonValue(status_string)); //用电器状态
    message.insert("end",QJsonValue("+")); //json尾部
//    qDebug()<<message;
    return message;
}

void Database::timesend_clock() //定时关闭
{
    QSqlDatabase db = QSqlDatabase::database("dbone");
    QSqlQuery query(db);
    query.exec("update room set light_status = 0,lock_status = 0,fan_status = 0,ac_status = 0");
    //直接修改原表，原表修改完后信号触发修改旧表
}

void Database::getroom(){ //获取数据库中的教室列表
        QSqlDatabase db = QSqlDatabase::database("dbone");
        QSqlQuery query(db);
        query.exec("select id,name from room");
//        QMap<QString,QVariant> room;
        while (query.next()) {
            m_room.insert(query.value("name").toString(),query.value("id").toString());
        }
//        return room;
}

QVariantMap Database::getroom_detil(QString id){
//    qDebug()<<id;
    QSqlDatabase db = QSqlDatabase::database("dbone");
    QSqlQuery query(db);
    room_detil aroom;
    QString search = QString("select * from room where id=%1").arg(id);
    query.exec(search);
    while (query.next()) {
        aroom.id = query.value("id").toInt();
        aroom.floor_id = query.value("floor_id").toInt();
        aroom.desc = query.value("desc").toString();
        aroom.name = query.value("name").toString();
        aroom.capacity = query.value("capacity").toInt();
        aroom.light_status = query.value("light_status").toInt();
        aroom.fan_status = query.value("fan_status").toInt();
        aroom.ac_status = query.value("ac_status").toInt();
        aroom.lock_status = query.value("lock_status").toInt();
    }
    QVariantMap aroom_map;

    aroom_map.insert("id",aroom.id);
    aroom_map.insert("floor_id",aroom.floor_id);
    aroom_map.insert("desc",aroom.desc);
    aroom_map.insert("name",aroom.name);
    aroom_map.insert("capacity",aroom.capacity);
    aroom_map.insert("light_status",aroom.light_status);
    aroom_map.insert("fan_status",aroom.fan_status);
    aroom_map.insert("ac_status",aroom.ac_status);
    aroom_map.insert("lock_status",aroom.lock_status);
//    qDebug()<<aroom.desc;
    return aroom_map;
}

void Database::update(QString id,QString ele,QString status){
//    qDebug()<<id;
    QSqlDatabase db = QSqlDatabase::database("dbone");
    QSqlQuery query(db);
    QString update = QString("update room set %1=%2 where id=%3").arg(ele).arg(status).arg(id.toInt());
    query.exec(update);
}

void Database::s_close(QString id)
{
    QSqlDatabase db = QSqlDatabase::database("dbone");
    QSqlQuery query(db);
    query.exec("update room set light_status = 0,lock_status = 0,fan_status = 0,ac_status = 0 where id="+id);
}

void Database::getroom_re(QString text){ //获取数据库中的教室列表
        qDebug()<<text;
        QSqlDatabase db = QSqlDatabase::database("dbone");
        QSqlQuery query(db);
        query.exec("select id,name from room where name like '%"+text+"%'");
//        QMap<QString,QVariant> room;
        m_room.clear();
        while (query.next()) {
            m_room.insert(query.value("name").toString(),query.value("id").toString());
        }
        qDebug()<<m_room.size();
//        return room;
}

void Database::timereset(){

    QDateTime nowtime = QDateTime::currentDateTime();
    QString firsttime = nowtime.toString("yyyy.MM.dd")+" 12:05:00";
    QString secondtime = nowtime.toString("yyyy.MM.dd")+" 17:30:00";
    QString thirdtime = nowtime.toString("yyyy.MM.dd")+" 22:00:00";
    QString endtime = nowtime.addDays(1).toString("yyyy.MM.dd")+" 22:23:00";
//    QString endtime = nowtime.toString("yyyy.MM.dd")+" 11:26:00"; //测试用时间

    QDateTime firsttime_equals = QDateTime::fromString(firsttime,"yyyy.MM.dd hh:mm:ss");
    QDateTime secondtime_equals = QDateTime::fromString(secondtime,"yyyy.MM.dd hh:mm:ss");
    QDateTime thirdtime_equals = QDateTime::fromString(thirdtime,"yyyy.MM.dd hh:mm:ss");
    QDateTime endtime_equals = QDateTime::fromString(endtime,"yyyy.MM.dd hh:mm:ss");

    uint nowtime_frame = nowtime.toTime_t();
    uint firsttime_frame = firsttime_equals.toTime_t();
    uint secondtime_frame = secondtime_equals.toTime_t();
    uint thirdtime_frame = thirdtime_equals.toTime_t();
    uint endtime_frame = endtime_equals.toTime_t();

    qDebug()<<firsttime_frame<<secondtime_frame<<thirdtime_frame<<endtime_frame;

    if(nowtime_frame <= firsttime_frame)
    {
        firsttime_clock = new QTimer(this);
        firsttime_clock->start((firsttime_frame-nowtime_frame)*1000);
        qDebug()<<firsttime_clock->isActive()<<firsttime_frame-nowtime_frame;
    }
    else
    {
        qDebug()<<"时间已过";
    }

    if(nowtime_frame <= secondtime_frame)
    {
        secondetime_clock = new QTimer(this);
        secondetime_clock->start((secondtime_frame-nowtime_frame)*1000);
        qDebug()<<secondetime_clock->isActive()<<secondtime_frame-nowtime_frame;
    }
    else
    {
        qDebug()<<"时间已过";
    }

    if(nowtime_frame <= thirdtime_frame)
    {
        thirdtime_clock = new QTimer(this);
        thirdtime_clock->start((thirdtime_frame-nowtime_frame)*1000);
        qDebug()<<thirdtime_clock->isActive()<<thirdtime_frame-nowtime_frame;
    }
    else
    {
        qDebug()<<"时间已过";
    }

    if(nowtime_frame <= endtime_frame)
    {
//        delete endtime_clock;
        endtime_clock = new QTimer(this);
        endtime_clock->start((endtime_frame-nowtime_frame)*1000);
        qDebug()<<endtime_clock->isActive()<<endtime_frame-nowtime_frame;
    }
    else
    {
        qDebug()<<"时间已过";
    }
//    qDebug()<<"ABC";
    connect(firsttime_clock,&QTimer::timeout,
            [=]()
    {
        qDebug()<<"上午时间已结束，用电器关闭";
        firsttime_clock->stop();
        timesend_clock();
    });
//    qDebug()<<"abc";
    connect(secondetime_clock,&QTimer::timeout,
            [=]()
    {
        qDebug()<<"中午时间已结束，用电器关闭";
        secondetime_clock->stop();
        timesend_clock();
    });

    connect(thirdtime_clock,&QTimer::timeout,
            [=]()
    {
        qDebug()<<"下午时间已结束，用电器关闭";
        thirdtime_clock->stop();
        timesend_clock();
    });

    connect(endtime_clock,&QTimer::timeout,
            [=]()
    {
        Sleep(2000);
        qDebug()<<"系统重置时间";
        endtime_clock->stop();
        timereset();
        timesend_clock();
        //重新连接，清楚缓存
//        DBlisten->stop();
//        delete c_db;
//        Sleep(2000);
//        c_db = new mqttclass();
//        c_db->connet("Client_db"," ","classroom");
//        c_db->connection();
//        DBlisten->start(1000);
    });
//    qDebug()<<123;
}

void Database::client_Received(const QMQTT::Message &message)
{
//    QString strPaylaod = message.payload();
    QString text = message.payload();
    QDateTime dateTime(QDateTime::currentDateTime());
    QString time = dateTime.toString("yy-MM-dd hh:mm::ss");
    emit logSignal(time,text);
    //反馈
}
