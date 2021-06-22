import QtQuick 2.3

Rectangle{
    id: toolbar
    anchors.top: parent.top
    anchors.topMargin: 31

    property int columns: 1

    property QtObject selectedTool: null

    property QtObject defaultStyle : QtObject{
        property color background: 'transparent'
        property color borderColor: 'transparent'
        property double borderSize: 0
        property double radius: 2
    }

    property QtObject style: defaultStyle

    width: 30
    height: imageView.height > 150 ? imageView.height : 150
    color: root.style.toolbarColor


    Grid{
        anchors.top: parent.top
        anchors.topMargin: 10
        spacing: 2
        columns: 1


    }
}
