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
        startX: 0; startY: root.height * 0.5
        PathLine{ x: root.width * 0.4; y: root.height * 0.9 }
        PathLine{ x: root.width; y: 0 }
    }
}
