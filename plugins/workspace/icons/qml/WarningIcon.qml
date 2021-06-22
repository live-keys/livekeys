import QtQuick 2.3
import QtQuick.Shapes 1.2

Shape{
    id: root
    
    width: 24
    height: 24
    
    property double strokeWidth: 1
    property double exclamationWidth: 2
    property color color: 'white'
    property color colorAlternate: 'grey'
    
    ShapePath{
        strokeWidth: root.strokeWidth
        strokeColor: root.colorAlternate
        fillColor: 'transparent'
        startX: root.width / 2; startY: 0
        PathLine{ x: root.width; y: root.height }
        PathLine{ x: 0; y: root.height }
        PathLine{ x: root.width / 2; y: 0 }
    }
    
    ShapePath{
        strokeWidth: root.exclamationWidth
        strokeColor: root.color
        fillColor: 'transparent'
        startX: root.width / 2; startY: root.height / 2.5
        PathLine{ x: root.width / 2; y: root.height - root.height / 3 }
    }
    
    ShapePath{
        strokeWidth: root.exclamationWidth
        strokeColor: root.color
        fillColor: 'transparent'
        startX: root.width / 2; startY: root.height - root.height / 6 - root.height / 100
        PathLine{ x: root.width / 2; y: root.height - root.height / 6 }
    }
}
