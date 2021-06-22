import QtQuick 2.3
import workspace 1.0
import visual.input 1.0 as Input

Tool{
    id: root

    toolLabel: 'Perspective'

    property QtObject labelInfoStyle: Input.TextStyle{}
    property Component applyButton : null
    property Component cancelButton : null

    property point p1: "0,0"
    property point p2: "0,0"
    property point p3: "0,0"
    property point p4: "0,0"

    signal apply(point p1, point p2, point p3, point p4)
    signal cancel()

    infoBarContent: Item {
        anchors.fill: parent

        Input.Label{
            id: pointsInfo
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 0
            width: parent.width - 80
            textStyle: root.labelInfoStyle
            text: {
                var info = ''
                if ( root.selectionShape ){
                    info += "(" + Math.round(root.p1.x) + ", " + Math.round(root.p1.y) + ")"
                    info += "-"
                    info += "(" + Math.round(root.p2.x) + ", " + Math.round(root.p2.y) + ")"
                    info += "-"
                    info += "(" + Math.round(root.p3.x) + ", " + Math.round(root.p3.y) + ")"
                    info += "-"
                    info += "(" + Math.round(root.p4.x) + ", " + Math.round(root.p4.y) + ")"
                }
                return "Perspective: " + info
            }
        }

        Input.Button{
            id: applyBttn
            anchors.left: pointsInfo.right
            anchors.leftMargin: 5
            width: 25
            height: 25
            content: root.applyButton
            onClicked: root.apply(root.p1, root.p2, root.p3, root.p4)
        }
        Input.Button{
            id: cancelBttn
            anchors.left: applyBttn.right
            anchors.leftMargin: 5
            width: 25
            height: 25
            content: root.cancelButton
            onClicked: root.cancel()
        }
    }

    property Item selectionShape: null

    property Component selectionShapeFactory: Item{
        property alias shape: cqSelection

        signal wheel(var wheel)
        onWheel: {
            if (wheel.modifiers & Qt.ControlModifier){
                var delta = Math.abs(wheel.angleDelta.y)
                if ( delta > 1 )
                    delta = 1
                var x = wheel.x
                var y = wheel.y

                if (wheel.angleDelta.y > 0){
                    root.canvas.setScale(root.canvas.scale + (0.05 * delta), {x : x, y : y})
                } else {
                    if ( root.canvas.scale > (0.05 * delta) ){
                        root.canvas.setScale(root.canvas.scale - (0.05 * delta), null)
                    }
                }

                selectionShape.shape.setPoints(root.p1, root.p2, root.p3, root.p4, root.canvas.scale)
                selectionShape.width = root.canvas.imageView.width
                selectionShape.height = root.canvas.imageView.height

                wheel.accepted = true
            }
            else{
                wheel.accepted = false
            }
        }

        MouseArea{
            anchors.fill: parent
            onWheel: parent.wheel(wheel)
        }

        Input.ConvexQuadSelection {
            id: cqSelection

            onRegionModified: {
                root.p1.x = shape.p1.x / root.canvas.scale
                root.p1.y = shape.p1.y / root.canvas.scale
                root.p2.x = shape.p2.x / root.canvas.scale
                root.p2.y = shape.p2.y / root.canvas.scale
                root.p3.x = shape.p3.x / root.canvas.scale
                root.p3.y = shape.p3.y / root.canvas.scale
                root.p4.x = shape.p4.x / root.canvas.scale
                root.p4.y = shape.p4.y / root.canvas.scale
            }
        }

    }

    activate: function(){
        if ( selectionShape ){
            selectionShape.destroy()
            selectionShape = null
        }

        root.canvas.mouseControl.visible = false

        var dims = root.canvas.imageView.image.dimensions()

        selectionShape = selectionShapeFactory.createObject(root.canvas.imageView)
        selectionShape.width = root.canvas.imageView.width
        selectionShape.height = root.canvas.imageView.height

        root.p1 = Qt.point(0,0)
        root.p2 = Qt.point(dims.width, 0)
        root.p3 = Qt.point(dims.width, dims.height)
        root.p4 = Qt.point(0, dims.height)

        selectionShape.shape.width = dims.width * root.canvas.scale
        selectionShape.shape.height = dims.height * root.canvas.scale
        selectionShape.shape.setPoints(root.p1, root.p2, root.p3, root.p4, root.canvas.scale)
    }

    deactivate: function(){
        root.canvas.mouseControl.visible = true
        selectionShape.destroy()
        selectionShape = null
    }


}
