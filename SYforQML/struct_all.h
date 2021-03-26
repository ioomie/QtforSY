#ifndef STRUCT_ALL_H
#define STRUCT_ALL_H
#include <QObject>

struct room_detil{
    int id;
    QString name;
    QString desc;
    int capacity;
    int light_status;
    int lock_status;
    int fan_status;
    int ac_status;
    int floor_id;
};
Q_DECLARE_METATYPE(room_detil);

#endif // STRUCT_ALL_H
