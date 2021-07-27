import QtQuick 2.3
import workspace 1.0
import visual.input 1.0 as Input

Tool{
    id: root

    toolLabel: 'Crop'

    property QtObject labelInfoStyle: Input.TextStyle{}
    property Component applyButton : null
    property Component cancelButton : null

    signal apply(double x, double y, double width, double height)
    signal cancel()

    property double selectedX: 0
    property double selectedY: 0
    property double selectedWidth: 0
    property double selectedHeight: 0

    infoBarContent: Item{
        anchors.fill: parent

        Input.Label{
            id: selectionInfo
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 0
            textStyle: root.labelInfoStyle
            text: {
                var info = ''
                if ( root.selectionArea ){
                    info += Math.round(root.selectedX) + ',' + Math.round(root.selectedY) + '   ' +
                            Math.round(root.selectedWidth) + 'x' + Math.round(root.selectedHeight)
                }
                return "Crop: " + info
            }
        }

        Input.Button{
            anchors.left: parent.left
            anchors.leftMargin: 140
            width: 25
            height: 25
            content: root.applyButton
            onClicked: root.apply(root.selectedX, root.selectedY, root.selectedWidth, root.selectedHeight)
        }
        Input.Button{
            anchors.left: parent.left
            anchors.leftMargin: 168
            width: 25
            height: 25
            content: root.cancelButton
            onClicked: root.cancel()
        }
    }

    function recalculateSelection(){
        selectionArea.rectangleSelection.x = root.selectedX * root.canvas.scale
        selectionArea.rectangleSelection.y = root.selectedY * root.canvas.scale
        selectionArea.rectangleSelection.width = root.selectedWidth * root.canvas.scale
        selectionArea.rectangleSelection.height = root.selectedHeight * root.canvas.scale
    }

    property Item selectionArea : null

    property Component selectionAreaFactory: Item{

        property alias rectangleSelection: rectangleSelection

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

                selectionArea.rectangleSelection.x = root.selectedX * root.canvas.scale
                selectionArea.rectangleSelection.y = root.selectedY * root.canvas.scale
                selectionArea.rectangleSelection.width = root.selectedWidth * root.canvas.scale
                selectionArea.rectangleSelection.height = root.selectedHeight * root.canvas.scale

                selectionArea.width = root.canvas.imageView.width
                selectionArea.height = root.canvas.imageView.height

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


        Input.RectangleSelection{
            id: rectangleSelection

            onRegionModified: {
                root.selectedX = selectionArea.rectangleSelection.x / root.canvas.scale
                root.selectedY = selectionArea.rectangleSelection.y / root.canvas.scale
                root.selectedWidth = selectionArea.rectangleSelection.width / root.canvas.scale
                root.selectedHeight = selectionArea.rectangleSelection.height / root.canvas.scale
            }

            onWheel: parent.wheel(wheel)
        }
    }

    activate: function(){
        if ( selectionArea ){
            selectionArea.destroy()
            selectionArea = null
        }

        root.canvas.mouseControl.visible = false

        selectionArea = selectionAreaFactory.createObject(root.canvas.imageView)
        selectionArea.width = root.canvas.imageView.width
        selectionArea.height = root.canvas.imageView.height

        selectedX = 0
        selectedY = 0

        var dims = root.canvas.imageView.image.dimensions()

        selectedWidth = dims.width
        selectedHeight = dims.height

        recalculateSelection()
    }

    deactivate: function(){
        root.canvas.mouseControl.visible = true
        selectionArea.destroy()
        selectionArea = null
    }

}
