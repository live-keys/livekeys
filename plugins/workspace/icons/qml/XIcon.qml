import QtQuick 2.10
import QtQuick.Shapes 1.2

Shape{
    id: root
    anchors.centerIn: parent
    
    width: 180
    height: 180
    
    property double strokeWidth: 30
    property color color: 'white'
    
    ShapePath{
        strokeWidth: root.strokeWidth
        strokeColor: root.color
        fillColor: 'transparent'
        capStyle: ShapePath.RoundCap
        joinStyle: ShapePath.RoundJoin
        startX: 0; startY: 0
        PathLine{ x: root.width; y: root.height }
        PathMove{ x: root.width; y: 0 }
        PathLine{ x: root.width; y: 0 }
        PathLine{ x: 0; y: root.height }
    }
}
