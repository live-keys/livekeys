import QtQuick 2.3
import QtQuick.Shapes 1.2

Shape{
    id: root
    
    width: 180
    height: 180
    
    
    property double strokeWidth: 1
    property color color: 'white'
    
    ShapePath{
        
        strokeWidth: root.strokeWidth
        strokeColor: root.color
        fillColor: 'transparent'
        startX: root.width / 4; startY: root.height / 5
        PathLine{ x: root.width - root.width / 4; y: root.height / 5 }
        PathLine{ x: root.width; y: root.height }
        PathLine{ x: 0; y: root.height }
        PathLine{ x: root.width / 4; y: root.height / 5 }
    }
    
}
