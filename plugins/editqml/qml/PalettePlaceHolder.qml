import QtQuick 2.3
import visual.shapes 1.0
import workspace.icons 1.0 as Icons

Rectangle{
    id: root
    color: '#333'

    property string title: ''

    Text{
        id: paletteBoxTitle
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 50
        clip: true
        text: root.title
        color: '#82909b'
    }

}
