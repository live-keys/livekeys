/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import live 1.0
import editor 1.0
import lcvcore 1.0 as Cv
import lcvimgproc 1.0 as Img
import workspace 1.0 as Workspace

CodePalette{
    id: palette

    type : "qml/lcvimgproc#TransformImage"

    property QtObject paletteStyle : lk ? lk.layers.workspace.extensions.editqml.paletteStyle : null

    property QtObject defaultStyle: QtObject{
        property color toolbarColor: "#333"
        property color toolbarBorder: "#444"
        property color boxColor: '#333'
        property color boxBorderColor: "#666"
        property int boxBorderWidth: 1
        property real boxRadius: 3
        property color toolIconColor: 'grey'
        property color toolIconHighlightBackground: '#333'
        property Component scrollStyle: imageView.defaultStyle.scrollStyle
        property QtObject labelStyle: QtObject{
            property color background: '#666'
            property double radius: 3
            property QtObject textStyle: Workspace.TextStyle{}
        }
        property Component saveButton: Workspace.TextButton{
            width: 50
            height: 25
            text: 'Save'
            onClicked: parent.clicked()
        }
        property Component applyButton: null
        property Component cancelButton: null
    }
    property QtObject style: QtObject{
        property color toolbarColor: paletteStyle ? paletteStyle.colorScheme.middleground : palette.defaultStyle.toolbarColor
        property color toolbarBorder: paletteStyle ? paletteStyle.colorScheme.middlegroundBorder : palette.defaultStyle.toolbarColor
        property color boxColor: paletteStyle ? paletteStyle.colorScheme.background : palette.defaultStyle.boxColor
        property color boxBorderColor: paletteStyle ? paletteStyle.colorScheme.backgroundBorder : palette.defaultStyle.boxBorderColor
        property int boxBorderWidth: 1
        property real boxRadius: 3
        property color toolIconColor: paletteStyle ? paletteStyle.colorScheme.foregroundFaded : palette.defaultStyle.toolIconColor
        property color toolIconHighlightBackground: paletteStyle ? paletteStyle.colorScheme.middlegroundOverlayDominant : palette.defaultStyle.toolIconHighlightBackground
        property Component scrollStyle: paletteStyle ? paletteStyle.scrollStyle : palette.defaultStyle.scrollStyle
        property QtObject labelStyle: paletteStyle ? paletteStyle.labelStyle : palette.defaultStyle.labelStyle
        property Component saveButton: paletteStyle ? paletteStyle.buttons.save : palette.defaultStyle.saveButton
        property Component applyButton: paletteStyle ? paletteStyle.buttons.apply : palette.defaultStyle.applyButton
        property Component cancelButton: paletteStyle ? paletteStyle.buttons.cancel : palette.defaultStyle.cancelButton
    }

    item: Item{
        id: paletteItem
        width: 500
        height: 300

        property QtObject transformImage: null
        onTransformImageChanged: {
            if ( transformImage ){
                imageView.image = Qt.binding(function(){ return transformImage.result })
                imageView.autoScale()
            }
        }

        Workspace.Toolbox{
            id: toolbox

            anchors.top: parent.top
            anchors.topMargin: 25

            width: 50
            height: parent.height - infobar.height

            style: QtObject{
                property color background: palette.style.toolbarColor
                property color borderColor: palette.style.toolbarBorder
                property double borderWidth: 1
            }

            canvas: canvas
            infoBar: infobar

            Workspace.ToolButton{
                id: cropButton
                tool: Cv.CropTool{
                    id: cropTool
                    canvas: imageView
                    labelInfoStyle: palette.style.labelStyle.textStyle
                    applyButton: palette.style.applyButton
                    cancelButton: palette.style.cancelButton

                    property Component cropImageFactory : Cv.Crop{}

                    onApply: {
                        var crop = cropImageFactory.createObject(paletteItem.transformImage)
                        crop.region = Qt.rect(x, y, width, height)
                        paletteItem.transformImage.transformations.push(crop)
                        paletteItem.transformImage.exec()
                        toolbox.activateTool(null)
                    }

                    onCancel: toolbox.activateTool(null)
                }
                content: Rectangle{
                    width: 20
                    height: 20
                    color: cropButton.containsMouse || toolbox.selectedTool === cropTool ? palette.style.toolIconHighlightBackground : 'transparent'
                    radius: 2
                    Workspace.CropIcon{
                        anchors.centerIn: parent
                        strokeWidth: 1
                        color: palette.style.toolIconColor
                        width: 12
                        height: 12
                    }
                }
            }

            Workspace.ToolButton{
                id: resizeButton

                tool: Img.ResizeTool{
                    id: resizeTool
                    canvas: imageView
                    labelInfoStyle: palette.style.labelStyle.textStyle
                    applyButton: palette.style.applyButton
                    cancelButton: palette.style.cancelButton
                    preserveAspectButton: Rectangle{
                        width: 20
                        height: 20
                        radius: 2
                        color: parent.containsMouse ? Qt.lighter(palette.style.toolIconHighlightBackground) : palette.style.toolIconHighlightBackground

                        Workspace.TwoWayArrowIcon{
                            anchors.centerIn: parent
                            width: 14
                            height: 14
                            strokeWidth: 1
                            color: resizeTool.preserveAspect ? palette.style.toolIconColor : 'transparent'
                        }
                    }

                    property Component resizeImageFactory : Img.Resize{}

                    onApply: {
                        var resize = resizeImageFactory.createObject(paletteItem.transformImage)
                        resize.size = Qt.size(width, height)
                        paletteItem.transformImage.transformations.push(resize)
                        paletteItem.transformImage.exec()
                        toolbox.activateTool(null)
                    }

                    onCancel: toolbox.activateTool(null)
                }
                content: Rectangle{
                    width: 20
                    height: 20
                    color: resizeButton.containsMouse || toolbox.selectedTool === resizeTool ? palette.style.toolIconHighlightBackground : 'transparent'
                    radius: 2

                    Workspace.ResizeIcon{
                        anchors.centerIn: parent
                        strokeWidth: 1
                        color: palette.style.toolIconColor
                        width: 12
                        height: 12
                    }
                }
            }

            Workspace.ToolButton{
                id: rotateButton
                tool: Img.RotateTool{
                    id: rotateTool
                    canvas: imageView
                    labelInfoStyle: palette.style.labelStyle.textStyle
                    applyButton: palette.style.applyButton
                    cancelButton: palette.style.cancelButton

                    property Component rotateImageFactory : Img.Rotate{}

                    onApply: {
                        var rotation = rotateImageFactory.createObject(paletteItem.transformImage)
                        rotation.degrees = -angle
                        paletteItem.transformImage.transformations.push(rotation)
                        paletteItem.transformImage.exec()
                        toolbox.activateTool(null)
                    }
                    onCancel: toolbox.activateTool(null)
                }
                content: Rectangle{
                    width: 20
                    height: 20
                    color: rotateButton.containsMouse || toolbox.selectedTool === rotateTool ? palette.style.toolIconHighlightBackground : 'transparent'
                    radius: 2

                    Workspace.RotateIcon{
                        anchors.centerIn: parent
                        strokeWidth: 1
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
        paletteItem.transformImage = value
    }
}
