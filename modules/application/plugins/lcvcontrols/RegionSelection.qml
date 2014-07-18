import QtQuick 2.0

Item{

    id : root

    property Item item : Item{}

    MouseArea{
        anchors.fill : parent
        onClicked : {
            root.item.regionX = mouse.x - 10
            root.item.regionY = mouse.y - 10
        }
    }
    Rectangle{
        width : root.item.regionWidth
        height : root.item.regionHeight
        color : "transparent"
        border.width : 1
        border.color : "#ff0000"
        x: root.item.regionX
        y: root.item.regionY
    }

}
