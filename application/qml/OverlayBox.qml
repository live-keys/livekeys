import QtQuick 2.3

Item{
    id: root

    property Item box: Item{}
    onBoxChanged: {
        if ( root.box === null )
            root.destroy()
    }

    property alias backgroundVisible: background.visible
    property bool centerBox: true
    property alias boxX : box.x
    property alias boxY : box.y

    Rectangle{
        id: background
        anchors.fill: parent

        color: "#000"
        opacity: root.visible ? 0.7 : 0
        Behavior on opacity { NumberAnimation{ duration: 200 } }
    }

    MouseArea{
        anchors.fill: parent
        onClicked: mouse.accepted = true;
        onPressed: mouse.accepted = true;
        onReleased: mouse.accepted = true;
        onDoubleClicked: mouse.accepted = true;
        onPositionChanged: mouse.accepted = true;
        onPressAndHold: mouse.accepted = true;
        onWheel: wheel.accepted = true;
    }

    Item{
        id: box
        anchors.centerIn: root.centerBox ? parent : undefined
        width: root.box ? root.box.width : 0
        height: root.box ? root.box.height : 0
        children: root.box ? [root.box] : []
    }

    function closeBox(){
        root.destroy()
    }



}
