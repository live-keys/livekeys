import QtQuick 2.3
import QtQuick.Shapes 1.5

Item{
    id: root
    width: 100
    height: 100
    clip: true

    property color color: 'grey'
    property double strokeWidth: 5
    
    layer.enabled: true
    layer.samples: 4

    Shape{
        id: circle
        anchors.fill: parent
        property real r: root.height / 5
        property real circleX: 2 * r + 1
        property real circleY: root.height - r - 1
        ShapePath {
            strokeColor: root.color
            fillColor: 'transparent'
            strokeWidth: root.strokeWidth
            startX: root.width - root.width / 10 - root.strokeWidth / 2
            startY: (root.height / 5) * 3

            PathArc {
                x: root.width / 2
                y: root.height / 5 + root.strokeWidth / 2
                radiusX: root.height / 5 * 2 - root.strokeWidth / 2
                radiusY: root.height / 5 * 2 - root.strokeWidth / 2
                useLargeArc: true
            }
        }
        
        ShapePath{
            strokeWidth: 1
            strokeColor: root.color
            fillColor: root.color
            startX: root.width / 2 - root.width / 8; 
            startY: root.strokeWidth
            PathLine{ 
                x: root.width / 2 + root.width / 10; 
                y: (root.height / 5) + root.strokeWidth / 2 
            }
            PathLine{ 
                x: root.width / 2 - root.width / 8
                y: root.height / 5 * 2 + root.strokeWidth
            }
        }
    }

}
