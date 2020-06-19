import QtQuick 2.3

Rectangle{
    id: root
    property Item target : parent
    property int rulersSize: 5

    width: rulersSize
    height: rulersSize
    radius: rulersSize
    color: "#667"
    anchors.right: target.right
    anchors.bottom: target.bottom

    property double minimumHeight: 50
    property double minimumWidth : 50

    MouseArea {
        anchors.fill: parent
        drag{
            target: parent;
            axis: Drag.XAndYAxis
        }
        cursorShape: Qt.SizeFDiagCursor
        onMouseYChanged: {
            if(drag.active){
                root.target.height = root.target.height + mouseY
                if(root.target.height < root.minimumHeight)
                    root.target.height = root.minimumHeight
            }
        }
        onMouseXChanged: {
            if(drag.active){
                root.target.width = root.target.width + mouseX
                if(root.target.width < root.minimumWidth)
                    root.target.width = root.minimumWidth
            }
        }
    }
}
