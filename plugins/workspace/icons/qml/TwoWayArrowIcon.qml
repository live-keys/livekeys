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
        
        ShapePath{
            strokeWidth: 1
            strokeColor: root.color
            fillColor: root.color
            startX: root.width - root.width / 6; 
            startY: root.height / 2 - root.height / 6
            PathLine{ 
                x: root.width; 
                y: (root.height / 2)
            }
            PathLine{ 
                x: root.width - root.width / 6
                y: root.height / 2 + root.height / 6
            }
        }
        
        ShapePath{
            strokeWidth: root.strokeWidth
            strokeColor: root.color
            fillColor: 'transparent'
            capStyle: ShapePath.RoundCap
            joinStyle: ShapePath.RoundJoin
            startX: root.strokeWidth
            startY: root.height / 2
            PathLine{ 
                x: root.width - root.strokeWidth
                y: root.height / 2 
            }
        }
        
        ShapePath{
            strokeWidth: 1
            strokeColor: root.color
            fillColor: root.color
            startX: root.width / 6; 
            startY: root.height / 2 - root.height / 6
            PathLine{ 
                x: 0 
                y: root.height / 2
            }
            PathLine{ 
                x: root.width / 6
                y: root.height / 2 + root.height / 6
            }
        }
    }

}
