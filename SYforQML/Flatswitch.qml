import QtQuick 2.12
import QtQuick.Controls 2.12

Switch {
    id: control
    text: qsTr("Switch")
    font.pixelSize: 20

    indicator: Rectangle {
        implicitWidth: 48
        implicitHeight: 26
        x: control.leftPadding
        y: parent.height / 2 - height / 2
        radius: 13
        color: control.checked ? "#FE0062" : "#ffffff"
        border.color: control.checked ? "#FE0062" : "#999999"

        Rectangle {
            x: control.checked ? parent.width - width : 0
            width: 26
            height: 26
            radius: 13
            color: control.down ? "#cccccc" : "#ffffff"
            border.color: control.checked ? (control.down ? "#FE0062" : "#FF5C9E") : "#999999"
            Behavior on x {
                NumberAnimation { duration: 130 }
            }
        }
    }

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: "#000000"
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
