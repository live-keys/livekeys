import QtQuick 2.3
import workspace 1.0
import lcvcore 1.0 as Cv
import lcvimgproc 1.0 as Img
import visual.input 1.0 as Input

Tool{
    id: root

    toolLabel: 'Brush'

    property QtObject labelInfoStyle: Input.TextStyle{}
    property Component sizeButton : null

    property real brushSize: 1

    infoBarContent: Item{
        anchors.fill: parent

        Input.Label{
            id: selectionInfo
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 0
            textStyle: root.labelInfoStyle
            text: 'Gradient'
        }
    }

    property Item selectionArea : null

    property real lastX
    property real lastY

    property QtObject imageContainer
    property QtObject toolbox

    activate: function(){}
    deactivate: function(){}

    property var visualStartPoint: null
    property var visualEndPoint: null

    property Component pointComponent: Rectangle{
        width: 10
        height: 10
        color: "white"
        radius: width / 2
        border.color: 'black'
        border.width: 1
    }

    mouseDown: function(event){
        root.lastX = event.imageX
        root.lastY = event.imageY

        if ( visualStartPoint ){
            visualStartPoint.destroy()
            visualStartPoint = null
        }
        if ( visualEndPoint ){
            visualEndPoint.destroy()
            visualEndPoint = null
        }

        var scrollX = imageContainer.imageView.scrollX
        var scrollY = imageContainer.imageView.scrollY

        visualEndPoint = pointComponent.createObject(root.canvas.imageView)
        visualEndPoint.x = event.mouse.x + scrollX - 5
        visualEndPoint.y = event.mouse.y + scrollY - 5

        visualStartPoint = pointComponent.createObject(root.canvas.imageView)
        visualStartPoint.x = event.mouse.x + scrollX - 5
        visualStartPoint.y = event.mouse.y + scrollY - 5
    }

    mouseDrag: function(event){
        if ( visualEndPoint ){
            var scrollX = imageContainer.imageView.scrollX
            var scrollY = imageContainer.imageView.scrollY

            visualEndPoint.x = event.mouse.x + scrollX - 5
            visualEndPoint.y = event.mouse.y + scrollY - 5
        }
    }

    mouseUp: function(event){
        if ( imageContainer && imageContainer.drawingEnabled && imageContainer.writableImage ){
            Cv.Gradient.draw(
                imageContainer.writableImage,
                Qt.point(root.lastX, root.lastY),
                Qt.point(event.imageX, event.imageY),
                [[0, toolbox.foreground], [1, toolbox.background]]
            )

            root.lastX = event.imageX
            root.lastY = event.imageY

            imageContainer.imageView.image = imageContainer.writableImage.toMat()
            imageContainer.imagePaint(imageContainer.imageView.image)
        }

        if ( visualStartPoint ){
            visualStartPoint.destroy()
            visualStartPoint = null
        }
        if ( visualEndPoint ){
            visualEndPoint.destroy()
            visualEndPoint = null
        }
    }

}
