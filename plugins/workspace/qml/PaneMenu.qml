import QtQuick 2.3
import workspace 1.0

Rectangle {
    id: root
    visible: false

    property int buttonHeight: 30
    property int buttonWidth: 180

    width: buttonWidth + root.border.width * 2
    height: column.height + root.border.width * 2

    opacity: visible ? 0.95 : 0
    z: 1000

    property QtObject style: PopupMenuStyle{}

    Behavior on opacity{ NumberAnimation{ duration: 200 } }

    border.width: style.borderWidth
    border.color: style.borderColor
    color: style.backgroundColor
    radius: style.radius

    default property alias children: column.children

    Column{
        id: column

        property alias buttonWidth: root.buttonWidth
        property alias buttonHeight: root.buttonHeight
        property alias style: root.style

        width: buttonWidth
        anchors.left: parent.left
        anchors.leftMargin: root.border.width
        anchors.top: parent.top
        anchors.topMargin: root.border.width
    }
}
