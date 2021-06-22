import QtQuick 2.10
import QtQuick.Shapes 1.5

Item{
    id: root
    width: 20
    height: 12
    
    property color color: "#ccc"
    property real thickness : 1
    
    Shape{
        anchors.fill: parent
        anchors.centerIn: parent
        ShapePath {
            fillColor: "transparent"
            strokeWidth: root.thickness
            strokeColor: root.color
            strokeStyle: ShapePath.SolidLine
            startX: 0; startY: 1
            PathLine{ x: width; y: 1 }
            PathMove{ x: 0; y: height / 2 }
            PathLine{ relativeX: width; y: height / 2 }
            PathMove{ x: 0; y: height -1 }
            PathLine{ relativeX: width; y: height - 1 }
        }
    }
}
