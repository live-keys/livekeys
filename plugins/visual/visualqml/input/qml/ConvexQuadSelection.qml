import QtQuick 2.3
import QtQuick.Shapes 1.12

Shape{
    id: root


    property QtObject defaultStyle: QtObject{
       property color background: "#33333333"
       property color lineColor: "#6da2f0"
       property int lineWidth: 3
       property color pointColor: "#6da2f0"
       property double pointDim: 10
    }

    property point p1: Qt.point(d.x, d.y)
    property point p2: Qt.point(a.x, a.y)
    property point p3: Qt.point(b.x, b.y)
    property point p4: Qt.point(c.x, c.y)

    property QtObject style: defaultStyle

    function setPoints(P1, P2, P3, P4, scale){
        ul.x = P1.x * scale - ul.radius
        ul.y = P1.y * scale - ul.radius

        ur.x = P2.x * scale - ur.radius
        ur.y = P2.y * scale - ur.radius

        br.x = P3.x * scale - br.radius
        br.y = P3.y * scale - br.radius

        bl.x = P4.x * scale - bl.radius
        bl.y = P4.y * scale - bl.radius
    }

    signal regionModified()

    function differentSide(s1, s2, l1, l2){
        if (Math.abs(l1.x-l2.x) < 0.001){
            if ((s1.x-l1.x)*(s2.x-l1.x) < 0){
                return true
            }
            return false
        } else {
            var k = (l1.y - l2.y)/(l1.x - l2.x)
            var n = l1.y - k*l1.x

            if ((k*s1.x + n - s1.y)*(k*s2.x + n - s2.y) < 0){
                return true
            }
            return false
        }
    }

    function sameSide(s1, s2, l1, l2){
        return !differentSide(s1, s2, l1, l2)
    }

    Rectangle {
        id: ul
        width: root.style.pointDim
        height: root.style.pointDim
        radius: root.style.pointDim / 2
        color: root.style.pointColor
        x: -radius
        y: -radius
        MouseArea {
            anchors.fill: parent
            drag.target: parent
            drag.axis: Drag.XAndYAxis
            drag.smoothed: false
            property bool dragActive: drag.active

            onDragActiveChanged: {
                if (drag.active) return

                if (sameSide(d, b, a, c) || differentSide(d, a, b, c) || differentSide(d, c, a, b)){
                    ul.x = (a.x + c.x) / 2 - ul.radius
                    ul.y = (a.y + c.y) / 2 - ul.radius

                    var x_diff = ul.x - br.x
                    var y_diff = ul.y - br.y
                    var norm = Math.sqrt(x_diff*x_diff + y_diff*y_diff)
                    var len = 10

                    ul.x += len * x_diff/norm
                    ul.y += len * y_diff/norm
                }

                if (d.x < 0)
                    ul.x = -ul.radius
                if (d.x >= root.parent.width - 1)
                    ul.x = root.parent.width - 2 - ul.radius

                if (d.y < 0)
                    ul.y = -ul.radius
                if (d.y >= root.parent.height - 1)
                    ul.y = root.parent.height - 2 - ul.radius


                root.regionModified()
            }
        }
    }

    Rectangle {
        id: ur
        width: root.style.pointDim
        height: root.style.pointDim
        radius: root.style.pointDim / 2
        color: root.style.pointColor
        x: parent.width-radius
        y: -radius

        MouseArea {
            anchors.fill: parent
            drag.target: parent
            drag.axis: Drag.XAndYAxis

            drag.smoothed: true
            property bool dragActive: drag.active

            onDragActiveChanged: {
                if (drag.active) return

                if (sameSide(a, c, d, b) || differentSide(a, d, b, c) || differentSide(a, b, c, d)){
                    ur.x = (b.x + d.x) / 2 - ur.radius
                    ur.y = (b.y + d.y) / 2 - ur.radius

                    var x_diff = ur.x - bl.x
                    var y_diff = ur.y - bl.y
                    var norm = Math.sqrt(x_diff*x_diff + y_diff*y_diff)
                    var len = 10

                    ur.x += len * x_diff/norm
                    ur.y += len * y_diff/norm
                }

                if (a.x < 0)
                    ur.x = -ur.radius
                if (a.x >= root.parent.width - 1)
                    ur.x = root.parent.width - 2 - ur.radius

                if (a.y < 0)
                    ur.y = -ur.radius
                if (a.y >= root.parent.height - 1)
                    ur.y = root.parent.height - 2 - ur.radius


                root.regionModified()
            }
        }
    }

    Rectangle {
        id: br
        width: root.style.pointDim
        height: root.style.pointDim
        radius: root.style.pointDim / 2
        color: root.style.pointColor
        x: parent.width-radius
        y: parent.height-radius
        MouseArea {
            anchors.fill: parent
            drag.target: parent
            drag.axis: Drag.XAndYAxis

            drag.smoothed: true
            property bool dragActive: drag.active

            onDragActiveChanged: {
                if (drag.active) return

                if (sameSide(b, d, a, c) || differentSide(b, a, c, d) || differentSide(b, c, a, d)){
                    br.x = (a.x + c.x) / 2 - br.radius
                    br.y = (a.y + c.y) / 2 - br.radius

                    var x_diff = br.x - ul.x
                    var y_diff = br.y - ul.y
                    var norm = Math.sqrt(x_diff*x_diff + y_diff*y_diff)
                    var len = 10

                    br.x += len * x_diff/norm
                    br.y += len * y_diff/norm
                }

                if (b.x < 0)
                    br.x = -br.radius
                if (b.x >= root.parent.width - 1)
                    br.x = root.parent.width - 2 - br.radius

                if (b.y < 0)
                    br.y = -br.radius
                if (b.y >= root.parent.height - 1)
                    br.y = root.parent.height - 2 - br.radius

                root.regionModified()
            }
        }
    }

    Rectangle {
        id: bl
        width: root.style.pointDim
        height: root.style.pointDim
        radius: root.style.pointDim / 2
        color: root.style.pointColor
        x: -radius
        y: parent.height-radius
        MouseArea {
            anchors.fill: parent
            drag.target: parent
            drag.axis: Drag.XAndYAxis

            drag.smoothed: true
            property bool dragActive: drag.active

            onDragActiveChanged: {
                if (drag.active) return

                if (sameSide(c, a, b, d) || differentSide(c, b, a, d) || differentSide(c, d, a, b)){
                    bl.x = (b.x + d.x) / 2 - bl.radius
                    bl.y = (b.y + d.y) / 2 - bl.radius

                    var x_diff = bl.x - ur.x
                    var y_diff = bl.y - ur.y
                    var norm = Math.sqrt(x_diff*x_diff + y_diff*y_diff)
                    var len = 10

                    bl.x += len * x_diff/norm
                    bl.y += len * y_diff/norm
                }

                if (c.x < 0)
                    bl.x = -bl.radius
                if (c.x >= root.parent.width - 1)
                    bl.x = root.parent.width - 2 - bl.radius

                if (c.y < 0)
                    bl.y = -bl.radius
                if (c.y >= root.parent.height - 1)
                    bl.y = root.parent.height - 2 - bl.radius

                root.regionModified()
            }
        }
    }

    ShapePath {
        startX: ul.x + ul.radius
        startY: ul.y + ul.radius
        strokeWidth: root.style.lineWidth
        strokeColor: root.style.lineColor
        fillColor: root.style.background

        PathLine {
            id: a
            x: ur.x + ur.radius
            y: ur.y + ur.radius
        }
        PathLine {
            id: b
            x: br.x + br.radius
            y: br.y + br.radius
        }
        PathLine {
            id: c
            x: bl.x + bl.radius
            y: bl.y + bl.radius
        }

        PathLine {
            id: d
            x: ul.x + ul.radius
            y: ul.y + ul.radius
        }
    }

}
