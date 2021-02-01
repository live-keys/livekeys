import QtQuick 2.3
import QtGraphicalEffects 1.0

Rectangle{
    width: 22
    height: 22
    color: hover ? hoverBackgroundColor : backgroundColor

    property color backgroundColor: 'transparent'
    property color hoverBackgroundColor: '#555'

    property bool hover: false
    property url iconSource: ''

    Image{
        id: rotateIcon
        width: 15
        height: 15
        anchors.centerIn: parent
        mipmap: true
        source: parent.iconSource
    }

    ColorOverlay{
        anchors.fill: rotateIcon
        source: rotateIcon
        color: "white"
    }
}
