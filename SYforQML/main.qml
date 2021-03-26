import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Database 1.0
import mqttclass 1.0

ApplicationWindow {
    id:root
    visible: true
    width: 1280
    height: 720
    title: qsTr("智能教室管理系统")

    property int room_id: -1
    property var roominfo: " "

    Database{ //数据库对象
        id:database
    }

    Mqttclass{
        id:mqttclass //单独一个对象 和qt是分开的
    }

    Component.onCompleted: { //组件加载时完成
        x = Screen.width / 2 - width / 2//启动时根据屏幕大小对窗口位置进行一个调整
        y = Screen.height / 2 - height / 2
//        database.timereset();
        mqttclass.connet("Client_thirtyseven"," ","classroom")
        mqttclass.connection();

        database.onLogSignal.connect(log_append)

//        mqttclass.onLogSignal.connect(()=>console.log('test'))
    }

    Timer{ //监听计时器
        id:listetimer
        interval: 1000 //设置1秒执行一次
        repeat: true
        running: false
        onTriggered:{
            sg_room_update(room_id)
        }
    }
    RowLayout {
        spacing: 45
        anchors.fill: parent
        anchors.margins: 50

        ColumnLayout { //教室选择布局
            spacing: 16
            TextField {
                Layout.maximumWidth: 230
                Layout.fillWidth: true
                onTextChanged: {
                    list_getroom_re(text)
                }
            }

            Frame {
                id: classroom_frame
                leftPadding: 1
                rightPadding: 1
                Layout.maximumWidth: 230
                Layout.fillWidth: true
                Layout.fillHeight: true

                ListView {
                    id:classroom
                    clip: true
                    anchors.fill: parent
                    model: classroom_item

                    delegate: ItemDelegate {
                        font.family: "Ubuntu"
                        font.pixelSize: 28
                        text: model.room
                        width: parent.width
                        onClicked: {
                            sg_room_update(model.id)
                            listetimer.running = true
                        }
                    }

                    ScrollBar.vertical: ScrollBar {
                        parent: classroom_frame
                        policy: ScrollBar.AlwaysOn
                        anchors.top: parent.top
                        anchors.topMargin: classroom_frame.topPadding
                        anchors.right: parent.right
                        anchors.rightMargin: 1
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: classroom_frame.bottomPadding
                    }
                }
                ListModel{
                    id:classroom_item
                    Component.onCompleted: {
                        list_getroom()
                    }
                }
            }
            Text {
                id: id_bottom
                text: qsTr("智能教室 thirtyseven")
                Layout.alignment: Qt.AlignHCenter //居中对齐
                //                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
        ColumnLayout {
            spacing: 24
            Text {
                id: classroom_title
                text: qsTr("教室详情")
                font.family: "Times"
                font.pixelSize: 38
                Layout.alignment: Qt.AlignHCenter
            }

            Frame{
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.maximumHeight: 300
                Text {
                    id: desc_show
                    text: qsTr("教室公告")
                    anchors.centerIn: parent
                }

            }
            Rectangle{
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.maximumHeight: 180
                color: "#DFE4EA"
                GridLayout{
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.maximumHeight: 180
                    anchors.leftMargin: 100
                    anchors.fill: parent
                    rows:2
                    columns:3
                    Flatswitch {
                        id:light
                        checked: false
                        text: "灯"
                        onClicked: {
                            checked ? database.update(room_id,"light_status",1) :database.update(room_id,"light_status",0)
                        }

                    }
                    Flatswitch {
                        id:lock
                        checked: false
                        text: "锁"
                        onClicked: {
                            checked ? database.update(room_id,"lock_status",1) :database.update(room_id,"lock_status",0)
                        }

                    }
                    Button {
                        width: 30
                        text: "关闭当前教室所有用电器"
                        onClicked: {
                            database.s_close(room_id)
                            sg_room_update(room_id)
                        }
                    }
                    Flatswitch {
                        id:fan
                        checked: false
                        text: "风扇"
                        onClicked: {
                            checked ? database.update(room_id,"fan_status",1) :database.update(room_id,"fan_status",0)
                        }
                    }
                    Flatswitch {
                        id:ac
                        checked: false
                        text: "空调"
                        onClicked: {
                            checked ? database.update(room_id,"ac_status",1) :database.update(room_id,"ac_status",0)
                        }
                    }
                    Button {
                        width: 30
                        text: "     刷新当前教室     "
                        onClicked: {
                            sg_room_update(room_id)
                        }
                    }
                }
            }
        }
        ColumnLayout {
            spacing: 16
            RowLayout{
                Layout.alignment: Qt.AlignHCenter
                spacing: 16
                Button{
                    text: "修改MQTT服务器"
                    onClicked: {
                        message_item.log_append({"time":"123\n","message":"abc"})
                        log_textarea.currentIndex = log_textarea.count-1
//                        mqttclass.sendMessage()
                    }
                }
                Button{
                    text: "退出系统"
                    onClicked: Qt.quit()
                }
            }

            Frame{
                leftPadding: 1
                rightPadding: 1
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.maximumWidth: 250

                ListView{
                    clip: true
                    anchors.fill: parent
                    id:log_textarea
                    model: message_item

                    delegate: ItemDelegate {
                        text: model.time+model.message
                        width: parent.width
                    }

                    ScrollBar.vertical: ScrollBar {
                        parent: log_textarea
                        policy: ScrollBar.AlwaysOn
                        anchors.top: parent.top
                        anchors.topMargin: log_textarea.topPadding
                        anchors.right: parent.right
                        anchors.rightMargin: 1
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: log_textarea.bottomPadding
                    }
                }
                ListModel{
                    id:message_item
                }
            }
            RowLayout{
                Layout.alignment: Qt.AlignHCenter
                spacing: 16
                Button{
                    text:"关闭所有教室"
                    onClicked: {
                    database.timesend_clock()
                    }
                }
                Button{
                    text:"清空日志台"
                    onClicked: {
                        message_item.clear()
                    }
                }
                Button{
                    text:"刷新系统"
                    onClicked: {
                        listetimer.running = false
                        classroom_item.clear()
                        list_getroom()
                        room_id = -1
                        roominfo = " "
                        light.checked = false
                        fan.checked = false
                        ac.checked = false
                        lock.checked = false
                        classroom_title.text = "教室详情"
                        desc_show.text = "教室公告"
                        message_item.clear()
                    }
                }

            }
        }

    }
    function log_append(time,message){ //日志台信息添加
//        console.log("123123123123123")
        log_textarea.model.append({
                                      time:time,
                                      message:message
                                  })
        log_textarea.currentIndex = log_textarea.count - 1
    }
    function sg_room_update(id){ //用于点击获取教室信息和刷新教室信息
//        console.log("test")
        roominfo = database.getroom_detil(id)
        room_id = roominfo.id
        classroom_title.text = roominfo.name
        desc_show.text = roominfo.desc
        switch(roominfo.light_status){
        case 0:light.checked = false;break
        case 1:light.checked = true
        }
        switch(roominfo.fan_status){
        case 0:fan.checked = false;break
        case 1:fan.checked = true
        }
        switch(roominfo.ac_status){
        case 0:ac.checked = false;break
        case 1:ac.checked = true
        }
        switch(roominfo.lock_status){
        case 0:lock.checked = false;break
        case 1:lock.checked = true
        }
    }

    function list_getroom(){
        database.getroom()
        for(var key in database.room){
            classroom_item.append({
                       room:key,
                       id:database.room[key]
                   })
        }
//        classroom.currentIndex = classroom.count -1
        classroom.currentIndex = 1
    }

    function list_getroom_re(text){
            classroom_item.clear()
        database.getroom_re(text)
        for(var key in database.room){
            classroom_item.append({
                       room:key,
                       id:database.room[key]
                   })
        }
//        classroom.currentIndex = classroom.count -1
        classroom.currentIndex = 1
    }
}
