import QtQuick 2.3
import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.2
import workspace 1.0 as Workspace
import workspace.icons 1.0
import lcvimgproc 1.0 as Img
import visual.input 1.0 as Input

Workspace.Tool{
    id: root

    toolLabel: 'Brush'

    property QtObject labelInfoStyle: Input.TextStyle{}
    property QtObject labelBoxStyle: Input.LabelOnRectangleStyle{}
    property QtObject formButtonStyle: Input.RectangleButtonStyle{}
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
            text: 'Brush'
        }

        Input.Button{
            anchors.left: selectionInfo.right
            anchors.leftMargin: 15
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: 15
            content: Input.RectangleButton{
                width: parent ? parent.width : 20
                height: parent ? parent.height: 20

                style: root.formButtonStyle

                content: PenSizeIcon{
                    anchors.centerIn: parent
                    width: parent.parent.width / 2.5
                    height: parent.parent.height / 2.5
                }
                onClicked: parent.clicked()
            }

            onClicked: sliderBackground.visible = !sliderBackground.visible
        }

        Rectangle{
            id: sliderBackground
            anchors.left: selectionInfo.right
            anchors.leftMargin: 30
            width: 220
            height: 30
            color: 'transparent'
            visible: false

            Slider{
                id: brushSizeSlider
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: valueLabel.width + 5
                width: parent.width - valueLabel.width - 10
                height: 15
                minimumValue: 1
                value: 1
                onValueChanged: root.brushSize = value
                stepSize: 1.0
                maximumValue: 500

                style: SliderStyle{
                    groove: Rectangle {
                        implicitHeight: 5
                        color: '#0b111c'
                    }
                    handle: Rectangle{
                        width: 11
                        height: 11
                        radius: 5
                        color: '#9b9da0'
                    }
                }
            }

            Input.LabelOnRectangle{
                id: valueLabel
                anchors.left: parent.left
                anchors.leftMargin: 2
                height: 25
                width: 40
                style: root.labelBoxStyle
                anchors.verticalCenter: parent.verticalCenter
                text: brushSizeSlider.value
            }
        }

    }

    property Item selectionArea : null

    property real lastX
    property real lastY

    property QtObject imageContainer
    property QtObject toolbox

    activate: function(){
        sliderBackground.visible = false
    }
    deactivate: function(){}

    mouseDown: function(event){
        root.lastX = event.imageX
        root.lastY = event.imageY
    }

    mouseDrag: function(event){
        if ( imageContainer && imageContainer.drawingEnabled && imageContainer.writableImage ){
            Img.Draw.line(
                imageContainer.writableImage,
                Qt.point(root.lastX, root.lastY),
                Qt.point(event.imageX, event.imageY),
                toolbox.foreground,
                root.brushSize
            );
            root.lastX = event.imageX
            root.lastY = event.imageY

            imageContainer.imageView.image = imageContainer.writableImage.toMat()
            imageContainer.imagePaint(imageContainer.imageView.image)
        }
    }

}
