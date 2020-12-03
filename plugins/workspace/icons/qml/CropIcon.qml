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
        capStyle: ShapePath.RoundCap
        joinStyle: ShapePath.RoundJoin
        startX: 0; startY: root.height / 5
        PathLine{ x: root.width - root.width / 5; y: root.height / 5 }
        PathLine{ x: root.width - root.width / 5; y: root.height }
    }
    
    ShapePath{
        strokeWidth: root.strokeWidth
        strokeColor: root.color
        fillColor: 'transparent'
        capStyle: ShapePath.RoundCap
        joinStyle: ShapePath.RoundJoin
        startX: 0 + root.width / 5; startY: 0
        PathLine{ x: root.width / 5; y: root.height - root.height / 5 }
        PathLine{ x: root.width; y: root.height - root.height / 5 }
    }
}
