import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import live 1.0
import editor 1.0
import lcvcore 1.0 as Cv
import lcvimgproc 1.0 as Img
import workspace 1.0 as Workspace
import workspace.icons 1.0 as Icons
import fs 1.0 as Fs

CodePalette{
    id: palette

    type : "qml/lcvcore#PerspectiveProjection"

    property QtObject theme: lk.layers.workspace.themes.current

    property QtObject style: QtObject{
        property color toolbarColor: theme.colorScheme.middleground
        property color toolbarBorder: theme.colorScheme.middlegroundBorder
        property color boxColor: theme.colorScheme.background
        property color boxBorderColor: theme.colorScheme.backgroundBorder
        property int boxBorderWidth: 1
        property real boxRadius: 3
        property color toolIconColor: theme.colorScheme.foregroundFaded
        property color toolIconHighlightBackground: theme.colorScheme.middlegroundOverlayDominant
        property QtObject labelStyle: theme.inputLabelStyle
        property Component saveButton: theme.buttons.save
        property Component applyButton: theme.buttons.apply
        property Component cancelButton: theme.buttons.cancel
    }

    property var paletteControls: lk.layers.workspace.extensions.editqml.paletteControls

    item: Item{
        id: paletteItem
        width: 500
        height: 300

        property QtObject perspectiveProjection: null
        onPerspectiveProjectionChanged: {
            if ( perspectiveProjection ){
                imageView.image = Qt.binding(function(){ return perspectiveProjection.result ? perspectiveProjection.result : null  })
                imageView.autoScale()
            }
        }

        Workspace.Toolbox{
            id: toolbox

            anchors.top: parent.top
            anchors.topMargin: 25

            width: 30
            height: parent.height - infobar.height

            style: QtObject{
                property color background: palette.style.toolbarColor
                property color borderColor: palette.style.toolbarBorder
                property double borderWidth: 1
            }

            canvas: canvas
            infoBar: infobar

            Workspace.ToolButton{
                id: perspectiveButton
                tool: Cv.PerspectiveTool {
                    id: perspectiveTool

                    canvas: imageView
                    labelInfoStyle: palette.style.labelStyle.textStyle
                    applyButton: palette.style.applyButton
                    cancelButton: palette.style.cancelButton

                    onApply: {
                        var value = '['
                        value += 'Qt.point(' + Math.round(p1.x) + ", " + Math.round(p1.y) +'), '
                        value += 'Qt.point(' + Math.round(p2.x) + ", " + Math.round(p2.y) +'), '
                        value += 'Qt.point(' + Math.round(p3.x) + ", " + Math.round(p3.y) +'), '
                        value += 'Qt.point(' + Math.round(p4.x) + ", " + Math.round(p4.y) +')]'


                        editFragment.writeProperties({'points': {"__ref": value} })
                        paletteItem.perspectiveProjection.points = [p1, p2, p3, p4]
                        toolbox.activateTool(null)
                    }
                    onCancel: toolbox.activateTool(null)
                }
                content: Rectangle{
                    width: 20
                    height: 20
                    color: perspectiveButton.containsMouse || toolbox.selectedTool === perspectiveTool ? palette.style.toolIconHighlightBackground : 'transparent'
                    radius: 2
                    layer.enabled: true
                    layer.samples: 8

                    Icons.PerspectiveIcon{
                        anchors.centerIn: parent
                        strokeWidth: 1.2
                        color: palette.style.toolIconColor
                        width: 12
                        height: 12
                    }
                }
            }
        }

        Rectangle{
            anchors.left: parent.left
            anchors.leftMargin: 0
            width: parent.width
            height: 25
            color: palette.style.toolbarColor
            border.color: palette.style.toolbarBorder
            border.width: 1

            Workspace.Button{
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                width: 25
                height: 25
                content: palette.style.saveButton

                onClicked: {
                    lk.layers.window.dialogs.saveFile({}, function(url){
                        var file = Fs.UrlInfo.toLocalFile(url)
                        Cv.MatIO.write(file, imageView.image, null)
                    })
                }
            }

            Item{
                id: infobar
                anchors.left: parent.left
                anchors.leftMargin: 50
                height: parent.height
                clip: true
                width: parent.width - dimensionsPanel.width - 50 - anchors.leftMargin
            }

            Item{
                height: parent.height
                width: zoomInfo.width
                anchors.right: dimensionsPanel.left
                anchors.rightMargin: 20
                Workspace.Label{
                    id: zoomInfo
                    textStyle: palette.style.labelStyle.textStyle
                    anchors.verticalCenter: parent.verticalCenter
                    text: Math.floor(imageView.scale * 100) + '%'
                }
            }


            Item{
                id: dimensionsPanel
                height: parent.height
                width: imageInfo.width
                anchors.right: parent.right
                anchors.rightMargin: 10
                Workspace.Label{
                    id: imageInfo
                    textStyle: palette.style.labelStyle.textStyle
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

        Rectangle{
            id: optionsPanel
            anchors.right: parent.right
            anchors.rightMargin: 0
            width: 200
            height: parent.height
            color: palette.style.toolbarColor
            border.color: palette.style.toolbarBorder
            border.width: 1

            visible: children.length > 0
        }

        Cv.NavigableImageView{
            id: imageView
            anchors.top: parent.top
            anchors.topMargin: 31

            anchors.left: parent.left
            anchors.leftMargin: 50

            width: paletteItem.width - 50
            height: paletteItem.height - 31

            style: QtObject{
               property Component scrollStyle: ScrollViewStyle {
                    transientScrollBars: false
                    handle: Item {
                        implicitWidth: 10
                        implicitHeight: 10
                        Rectangle {
                            color: "#1f2227"
                            anchors.fill: parent
                        }
                    }
                    scrollBarBackground: Item{
                        implicitWidth: 10
                        implicitHeight: 10
                        Rectangle{
                            anchors.fill: parent
                            color: 'transparent'
                        }
                    }
                    decrementControl: null
                    incrementControl: null
                    frame: Item{}
                    corner: Rectangle{color: 'transparent'}
                }
            }
        }


        ResizeArea{
            minimumWidth: 400
            minimumHeight: 200
        }

    }

    onInit: {
        paletteItem.perspectiveProjection = value
    }
    onValueFromBindingChanged: {
        paletteItem.perspectiveProjection = value
    }
}
