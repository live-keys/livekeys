import QtQuick 2.3
import QtQuick.Shapes 1.2

Shape{
    id: root
    anchors.centerIn: parent
    
    width: 180
    height: 180
    
    property double strokeWidth: 20
    property color color: 'white'
    
    ShapePath{
        strokeWidth: root.strokeWidth
        strokeColor: root.color
        fillColor: 'transparent'
        capStyle: ShapePath.RoundCap
        joinStyle: ShapePath.RoundJoin
        startX: root.width - root.width / 3; startY: 0
        PathLine{ x: root.width; y: 0 }
        PathLine{ x: root.width; y: root.height / 3 }
    }
    
    ShapePath{
        strokeWidth: root.strokeWidth
        strokeColor: root.color
        fillColor: 'transparent'
        capStyle: ShapePath.RoundCap
        joinStyle: ShapePath.RoundJoin
        startX: root.width; startY: 0
        PathLine{ x: root.width / 1.6; y: root.height - root.height / 1.6 }
    }
    
    ShapePath{
        strokeWidth: root.strokeWidth
        strokeColor: root.color
        fillColor: 'transparent'
        capStyle: ShapePath.RoundCap
        joinStyle: ShapePath.RoundJoin
        startX: 0; startY: root.height - root.height / 3
        PathLine{ x: 0; y: root.height }
        PathLine{ x: root.width / 3; y: root.height }
    }
    
    ShapePath{
        strokeWidth: root.strokeWidth
        strokeColor: root.color
        fillColor: 'transparent'
        capStyle: ShapePath.RoundCap
        joinStyle: ShapePath.RoundJoin
        startX: 0; startY: root.height
        PathLine{ x: root.width - root.width / 1.6; y: root.height / 1.6 }
    }
}