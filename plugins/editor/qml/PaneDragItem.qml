import QtQuick 2.2

Item{
    id: root
    
    signal dragStarted()
    signal dragFinished()
    
    width: 10
    height: 30
    
    property string display: ''
    
    Text { anchors.fill: parent; text: "||"; wrapMode: Text.WordWrap; color: 'white' }
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        drag.target: draggable
        cursorShape: Qt.SizeAllCursor
    }
    Item {
        id: draggable
        anchors.fill: parent
        Drag.active: mouseArea.drag.active
        Drag.hotSpot.x: 0
        Drag.hotSpot.y: 0
        Drag.mimeData: { "text/plain": root.display }
        Drag.dragType: Drag.Automatic
        Drag.onDragStarted: root.dragStarted()
        Drag.onDragFinished: root.dragFinished()
    }
}
