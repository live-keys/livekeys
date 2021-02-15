import QtQuick 2.11

Rectangle{
    id: root
    height: 24
    width: 24
    border.color: style.borderColor
    border.width: style.borderSize
    color: style.backgroundColor
    radius: style.radius
    
    property bool checked: false

    property var __checkMark : null
    
    function __initializeStyle(){
        if ( __checkMark ){
            __checkMark.destroy()
            __checkMark = null
        }
        __checkMark = style.checkMark.createObject(root)
        __checkMark.parent = root
        __checkMark.anchors.centerIn = root
        __checkMark.visible = root.checked
    }
    
    property QtObject style: CheckBoxStyle{}
    onStyleChanged: root.__initializeStyle()

    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.checked = !root.checked
            root.__checkMark.visible = root.checked
        }
    }
    
    Component.onCompleted: { root.__initializeStyle() }
}
