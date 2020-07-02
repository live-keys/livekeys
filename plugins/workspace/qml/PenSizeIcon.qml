import QtQuick 2.3
import QtQuick.Shapes 1.5

Item{
    id: root
    width: 50
    height: 50
    clip: true

    property color color: 'grey'
    property color penColor: color
    property color circleColor: color

    Shape{
        id: circle
        anchors.fill: parent
        property real r: root.height / 5
        property real circleX: 2 * r + 1
        property real circleY: root.height - r - 1
        ShapePath {
            strokeColor: root.circleColor
            fillColor: 'transparent'
            strokeWidth: 1
            startX: circle.circleX
            startY: circle.circleY - circle.r

            PathArc{
                x: circle.circleX
                y: circle.circleY + circle.r
                radiusX: circle.r; radiusY: circle.r
            }
            PathArc{
                x: circle.circleX
                y: circle.circleY - circle.r
                radiusX: circle.r
                radiusY: circle.r
            }
        }

        ShapePath{
            strokeColor: root.penColor
            strokeWidth: root.height / 15
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap


            startX: root.width - root.width / 4
            startY: strokeWidth
            PathLine {
                x: circle.r * 2
                y: root.height - circle.r
            }
        }
    }

}
