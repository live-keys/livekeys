import QtQuick 2.10
import QtQuick.Shapes 1.2

Shape{
    id: root

    width: 180
    height: 180

    property double usedWidth: width * 0.83
    property double leftPadding: width * 0.085

    property double strokeWidth: height / 10
    property color color: 'darkgrey'
    property color alternateColor: 'grey'

    ShapePath{
        strokeWidth: root.strokeWidth
        strokeColor: root.color
        fillColor: root.color
        capStyle: ShapePath.RoundCap
        joinStyle: ShapePath.RoundJoin
        startX: root.width * 0.4; startY: 0
        PathLine{ x: root.leftPadding + root.usedWidth * 0.6; y: 0 }

        PathMove{ x: root.leftPadding + root.strokeWidth / 2; y: root.strokeWidth }
        PathLine{ x: root.leftPadding + root.usedWidth - root.strokeWidth / 2; y: root.strokeWidth }

        PathMove{ x: root.leftPadding + root.strokeWidth / 2; y: root.height * 0.3 }
        PathLine{ x: root.leftPadding + root.usedWidth - root.strokeWidth / 2; y: root.height * 0.3 }
        PathLine{ x: root.leftPadding + root.usedWidth - root.strokeWidth / 2 - root.usedWidth * 0.1; y: root.height - root.strokeWidth / 2 }
        PathLine{ x: root.leftPadding + root.strokeWidth + root.usedWidth * 0.1; y: root.height - root.strokeWidth / 2 }
        PathLine{ x: root.leftPadding + root.strokeWidth / 2; y: root.height * 0.3 }

    }

    ShapePath{
        strokeWidth: root.usedWidth / 4.5
        strokeColor: root.alternateColor
        fillColor: 'transparent'
        capStyle: ShapePath.RoundCap
        joinStyle: ShapePath.RoundJoin
        startX: root.width * 0.4; startY: 0


        PathMove{ x: root.leftPadding + root.usedWidth * 0.30; y: root.height * 0.4 }
        PathLine{ x: root.leftPadding + root.usedWidth * 0.35; y: root.height - root.strokeWidth / 2 - root.height * 0.1}
        PathMove{ x: root.leftPadding + root.usedWidth * 0.75; y: root.height * 0.4 }
        PathLine{ x: root.leftPadding + root.usedWidth * 0.70; y: root.height - root.strokeWidth / 2 - root.height * 0.1}
    }
}
