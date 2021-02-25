import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import lcvcore 1.0 as Cv
import lcvimgproc 1.0 as Img
import workspace 1.0 as Workspace
import fs 1.0 as Fs
import visual.input 1.0 as Input

Item{
    id: root
    width: 700
    height: 400

    property bool drawingEnabled: true
    property double brushSize: brushSizeSlider.value
    property color brushColor: colorPicker.selectedColor

    property real lastX
    property real lastY
    property QtObject image: null
    onImageChanged: {
        root.writableImage = Cv.MatOp.createWritableFromMat(image)
        imageView.image = image
        imageView.autoScale()
    }
    property var writableImage: null

    signal imagePaint(QtObject image)


    property QtObject defaultStyle: QtObject{
        property color toolbarColor: "#333"
        property color boxColor: '#333'
        property color boxBorderColor: "#666"
        property int boxBorderWidth: 1
        property real boxRadius: 3
        property QtObject labelStyle: colorPicker.defaultStyle.labelStyle
        property QtObject colorPicker: colorPicker.defaultStyle

        property Component saveButton: Workspace.TextButton{
            width: 50
            height: 25
            text: 'Save'
            onClicked: parent.clicked()
        }
        property Component brushSizeButton: Workspace.TextButton{
            width: 30
            height: 25
            text: 'B'
            onClicked: parent.clicked()
        }
    }
    property QtObject style: defaultStyle

    Cv.NavigableImageView{
        id: imageView
        anchors.left: parent.left
        anchors.leftMargin: toolbar.width + 1
        anchors.top: parent.top
        anchors.topMargin: 31

        maxWidth: root.width - anchors.leftMargin
        maxHeight: root.height - anchors.topMargin


        onPressed: {
            root.lastX = event.imageX
            root.lastY = event.imageY
        }
        onPositionChanged: {
            if ( root.drawingEnabled && root.writableImage ){
                Img.Draw.line(
                    root.writableImage,
                    Qt.point(root.lastX, root.lastY),
                    Qt.point(event.imageX, event.imageY),
                    root.brushColor,
                    root.brushSize
                );
                root.lastX = event.imageX
                root.lastY = event.imageY

                imageView.image = root.writableImage.toMat()
                root.imagePaint(imageView.image)
            }
        }
    }

    Rectangle{
        id: toolbar
        anchors.top: parent.top
        anchors.topMargin: 31
        width: 30
        height: imageView.height > 150 ? imageView.height : 150
        color: root.style.toolbarColor

        Column{
            anchors.top: parent.top
            anchors.topMargin: 10

            Item{
                visible: root.drawingEnabled
                width: 30 + (sliderBackground.visible ? sliderBackground.width : 0)
                height: 30


                Workspace.Button{
                    anchors.verticalCenter: parent.verticalCenter
                    width: 30
                    height: 30
                    content: root.style.brushSizeButton

                    onClicked: sliderBackground.visible = !sliderBackground.visible
                }

                Rectangle{
                    id: sliderBackground
                    anchors.left: parent.left
                    anchors.leftMargin: 30
                    width: 220
                    height: 30
                    color: root.style.boxColor
                    border.color: root.style.boxBorderColor
                    border.width: root.style.boxBorderWidth
                    radius: root.style.boxRadius
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

                    Workspace.LabelOnRectangle{
                        id: valueLabel
                        anchors.left: parent.left
                        anchors.leftMargin: 2
                        height: 25
                        width: 40
                        style: root.style.labelStyle
                        anchors.verticalCenter: parent.verticalCenter
                        text: brushSizeSlider.value
                    }
                }
            }

            Rectangle{
                width: colorPicker.width + 5
                height: colorPicker.height
                color: root.style.boxColor

                Workspace.ColorPicker{
                    id: colorPicker
                    anchors.left: parent.left
                    anchors.leftMargin: 3
                    colorDisplayHeight: 25
                    colorDisplayWidth: 25
                    visible: root.drawingEnabled
                    inputBoxVisible: false
                    style: root.style.colorPicker
                }
            }
        }
    }

    Rectangle{
        id: topBar
        width: imageView.width + 30 > 150 ? imageView.width + 30 : 150
        height: 30
        color: root.style.toolbarColor

        Workspace.Button{
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.verticalCenter: parent.verticalCenter
            width: 30
            height: 25
            content: root.style.saveButton

            onClicked: {
                lk.layers.window.dialogs.saveFile({}, function(url){
                    var file = Fs.UrlInfo.toLocalFile(url)
                    Cv.MatIO.write(file, imageView.image, null)
                })
            }
        }

        Item{
            height: parent.height
            width: zoomInfo.width
            anchors.left: parent.left
            anchors.leftMargin: 70
            Workspace.Label{
                id: zoomInfo
                anchors.verticalCenter: parent.verticalCenter
                text: Math.floor(imageView.scale * 100) + '%'
            }
        }

        Item{
            height: parent.height
            width: imageInfo.width
            anchors.right: parent.right
            anchors.rightMargin: 10
            Workspace.Label{
                id: imageInfo
                anchors.verticalCenter: parent.verticalCenter
                text: {
                    if ( !imageView.image )
                        return '-'
                    var dim = imageView.image.dimensions()
                    return dim.width + 'x' + dim.height + ', ' + imageView.image.channels() + ' Ch'
                }
            }
        }
    }

}
