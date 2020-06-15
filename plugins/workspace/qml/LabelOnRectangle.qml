import QtQuick 2.3
import workspace 1.0 as Workspace

Rectangle{
    id: root
    width: 50
    height: 20
    color: style.background
    radius: style.radius

    property alias label: label

    property QtObject defaultStyle: QtObject{
        property color background: '#666'
        property double radius: 3
        property QtObject textStyle: TextStyle{}
    }
    property QtObject style: defaultStyle

    property int margins: 0
    property string text: 'Label'

    Workspace.Label{
        id: label
        anchors.centerIn: parent
        text: root.text
        textStyle: root.style.textStyle
    }
}
