import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import lcvcore 1.0 as Cv
import lcvimgproc 1.0 as Img
import workspace 1.0 as Workspace
import workspace.icons 1.0 as Icons
import fs 1.0 as Fs
import visual.input 1.0 as Input
import visual.shapes 1.0

Item{
    id: root
    width: 700
    height: 400

    property bool drawingEnabled: true
//    property double brushSize: brushSizeSlider.value
//    property color brushColor: colorPicker.selectedColor

    property QtObject image: null
    property QtObject imageView: imageView
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
        property color toolHighlightColor: "#444"
        property QtObject labelStyle: colorPicker.style.labelStyle
        property QtObject labelBoxStyle: Workspace.LabelOnRectangleStyle{}
        property QtObject formButtonStyle: Workspace.RectangleButtonStyle{}

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
        anchors.leftMargin: toolbox.width + 1
        anchors.top: parent.top
        anchors.topMargin: 31

        maxWidth: root.width - anchors.leftMargin
        maxHeight: root.height - anchors.topMargin

        onPressed: {
            if ( toolbox.selectedTool ){
                toolbox.selectedTool.mouseDown(event)
            }
        }
        onReleased: {
            if ( toolbox.selectedTool ){
                toolbox.selectedTool.mouseUp(event)
            }
        }
        onPositionChanged: {
            if ( toolbox.selectedTool && ( event.mouse.buttons & Qt.LeftButton ) ){
                toolbox.selectedTool.mouseDrag(event)
            }
        }
    }

    Rectangle{
        color: toolbox.color
        width: 50
        height: parent.height

        Workspace.Toolbox{
            id: toolbox
            anchors.top: parent.top
            anchors.topMargin: 31
            width: parent.width
            height: 100

            property color foreground: 'black'
            property color background: 'white'

            canvas: imageView
            infoBar: infobar
            color: root.style.toolbarColor
            border.width: 0
            border.color: 'transparent'

            Workspace.ToolButton{
                id: brushTool
                tool: Cv.BrushTool{
                    id: cropTool
                    canvas: imageView
                    imageContainer: root
                    toolbox: toolbox
                    formButtonStyle: root.style.formButtonStyle
                    labelBoxStyle: root.style.labelBoxStyle
                }
                content: Rectangle{
                    width: 20
                    height: 20
                    color: brushTool.containsMouse || toolbox.selectedTool === cropTool ? root.style.toolHighlightColor : 'transparent'
                    radius: 2
                    Icons.BrushIcon{
                        anchors.centerIn: parent
                        width: 12
                        height: 12
                    }
                }
            }

            Workspace.ToolButton{
                id: gradientTool
                tool: Cv.GradientTool{
                    id: rotateTool
                    canvas: imageView
                    imageContainer: root
                    toolbox: toolbox

                }
                content: Rectangle{
                    width: 20
                    height: 20
                    color: gradientTool.containsMouse || toolbox.selectedTool === rotateTool ? root.style.toolHighlightColor : 'transparent'
                    radius: 2

                    Icons.GradientIcon{
                        anchors.centerIn: parent
                        width: 12
                        height: 12
                    }
                }
            }
        }

        Item{
            anchors.top: toolbox.bottom
            anchors.topMargin: 30
            width: 50
            height: 50

            Chessboard{
                x: 5
                color: 'white'
                width: 25
                height: 25
                cellSize: 25 / 6

                Rectangle{
                    border.width: 1
                    border.color: 'black'
                    width: 25
                    height: 25

                    color: toolbox.foreground

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {

                            if ( colorPickerBox.visible && colorPicker.selection === 'foreground'){
                                colorPickerBox.visible = false
                            } else {
                                colorPicker.selection = ''
                                colorPicker.color = toolbox.foreground
                                colorPicker.selection = 'foreground'
                                colorPickerBox.visible = true
                            }
                        }
                    }

                    Loader{
                        id: foregroundColorLoader
                        anchors.fill: parent
                        sourceComponent: root.style.colorInput
                        onItemChanged: {
                            item.onValueChanged.connect(function(value){
                                root.toolStyle.foreground = value
                            })
                        }
                    }
                }
            }


            Chessboard{
                color: 'white'
                width: 25
                height: 25
                cellSize: 25 / 6
                x: 20
                y: 20

                Rectangle{
                    width: 25
                    height: 25
                    border.width: 3
                    border.color: toolbox.background
                    color: "transparent"

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            if ( colorPickerBox.visible && colorPicker.selection === 'background'){
                                colorPickerBox.visible = false
                            } else {
                                colorPicker.selection = ''
                                colorPicker.color = toolbox.background
                                colorPicker.selection = 'background'
                                colorPickerBox.visible = true
                            }
                        }
                    }

                    Loader{
                        id: borderColorLoader
                        anchors.fill: parent
                        sourceComponent: root.style.colorInput
                        onItemChanged: {
                            item.onValueChanged.connect(function(value){
                                root.toolStyle.background = value
                            })
                        }
                    }
                }

                Rectangle{
                    x: 3
                    y: 3
                    color: toolbox.color
                    width: 19
                    height: 19
                }
            }

            Rectangle{
                id: colorPickerBox
                visible: false
                anchors.top: parent.top
                anchors.topMargin: 30
                anchors.left: parent.left
                anchors.leftMargin: toolbox.width

                width: colorPicker.width
                height: colorPicker.height
                color: colorPicker.style.backgroundColor

                Item{
                    anchors.right: parent.right
                    anchors.rightMargin: 3
                    anchors.top: parent.top
                    anchors.topMargin: 3
                    width: 25
                    height: 25
                    Icons.XIcon{
                        width: 8
                        height: 8
                        anchors.centerIn: parent
                        strokeWidth: 1
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            colorPickerBox.visible = false
                        }
                    }
                }

                Input.ColorPicker{
                    id: colorPicker
                    anchors.top: parent.top
                    anchors.topMargin: 27
                    height: 180

                    property string selection: 'foreground'

                    style: Input.ColorPickerStyle{
                        backgroundColor: root.style.toolbarColor
                        colorPanelBorderColor: root.style.boxBorderColor
    //                    inputBoxStyle: theme.inputStyle
//                        labelStyle: root.style.textStyle
                    }

                    onSelectedColorChanged: {
                        if ( selection === 'background' ){
                            toolbox.background = selectedColor
                        } else if ( selection === 'foreground' ){
                            toolbox.foreground = selectedColor
                        }
                    }
                }
            }
        }
    }

    Rectangle{
        id: topBar
        width: imageView.width + 30 > 250 ? imageView.width + 30 : 250
        height: 30
        visible: root.image ? true : false
        color: root.style.toolbarColor

        Workspace.Button{
            anchors.left: parent.left
            anchors.leftMargin: 5
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
            anchors.leftMargin: 35
            Workspace.Label{
                id: zoomInfo
                anchors.verticalCenter: parent.verticalCenter
                textStyle: root.style.labelStyle.textStyle
                text: Math.floor(imageView.scale * 100) + '%'
            }
        }

        Item{
            id: infobar
            anchors.left: parent.left
            anchors.leftMargin: 70
            height: parent.height
            clip: true
            width: parent.width - zoomInfo.width - dimensionsPanel.width - 50 - anchors.leftMargin
        }

        Item{
            id: dimensionsPanel
            height: parent.height
            width: imageInfo.width
            anchors.right: parent.right
            anchors.rightMargin: 10
            Workspace.Label{
                id: imageInfo
                anchors.verticalCenter: parent.verticalCenter
                textStyle: root.style.labelStyle.textStyle
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
