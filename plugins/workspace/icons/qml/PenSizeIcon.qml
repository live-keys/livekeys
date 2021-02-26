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
        property real r: root.height / 2
        property real circleX: r
        property real circleY: r
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

    }

}
