import QtQuick 2.3
import workspace 1.0

Rectangle{
    id: root
    visible: false
    opacity: visible ? 1.0 : 0
    z: 1000

    property QtObject defaultStyle: QtObject{
        property color backgroundColor: "#03070b"
        property double radius: 0

        property QtObject itemStyle: TextStyle{}
        property QtObject itemHoverStyle: TextStyle{}
    }

    property QtObject style: defaultStyle

    color : style.backgroundColor
    radius: style.radius

    width: 180

    Behavior on opacity{ NumberAnimation{ duration: 200 } }

    default property alias children: column.children

    Column{
        id: column


    }

}
