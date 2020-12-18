import QtQuick 2.3
import QtQuick.Shapes 1.2

Shape{
    id: root
    
    width: 200
    height: 200
    
    property color color: 'white'
    property double padding: root.width / 8
    
    ShapePath{
        strokeWidth: 1
        strokeColor: root.color
        fillColor: root.color
        startX: root.width / 8
        startY: 0
        PathLine{ 
            x: root.width - root.padding - root.width / 4
            y: 0
        }
        PathLine{ 
            x: root.width - root.padding
            y: root.height / 4
        }
        PathLine{ 
            x: root.width - root.padding
            y: root.height
        }
        PathLine{ 
            x: root.padding
            y: root.height
        }
    }
}