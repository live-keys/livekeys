import QtQuick 2.3
    
Item{
    id : root

    width : 100
    height : 50

    property bool hoverEnabled : true
    property bool containsMouse : mouseArea.containsMouse
    signal clicked()
    
    property Component content: Item{
    }
    onContentChanged: {
        contentItem = content.createObject()
    }
    
    property Item contentItem: null
    property MouseArea mouseArea : MouseArea{
        id : buttonMouseArea
        hoverEnabled: root.hoverEnabled
        anchors.fill: parent
        onClicked: root.clicked()
    }
    
    children: [contentItem, mouseArea]

}
