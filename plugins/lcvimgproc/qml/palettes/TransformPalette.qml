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
import workspace.icons 1.0 as Icons

CodePalette{
    id: palette

    type : "qml/lcvimgproc#TransformImage"

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

    function addTransformation(name){
        var p = palette.item
        while (p && p.objectName !== "paletteGroup")
        {
            p = p.parent
        }

        if (!p) return null

        var oc = p.parent.parent.parent.parent
        if (oc.objectName === "objectContainer"){ // inside shaping
            var position =
                p.editingFragment.valuePosition() +
                p.editingFragment.valueLength() - 1
            paletteControls.addItemToRuntimeWithNotification(oc, position, "TransformImage", name, false)

            var cont = p.groupsContainer
            return cont.children[cont.children.length - 1]

        } else { // inside palette
            var ef = p.editingFragment
            while (p && p.objectName !== "editorType")
            {
                p = p.parent
            }
            var codeHandler = p.documentHandler.codeHandler
            var position = ef.valuePosition() + ef.valueLength() - 1

            var childEf = paletteControls.addItemToRuntime(codeHandler, ef, position, "TransformImage", name)
            childEf.visualParent = p
            return childEf
        }
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

                    onApply: {
                        var crop = addTransformation("Crop")
                        var valueToAssign = '"' + Math.round(x) + "," + Math.round(y) + "," + Math.round(width) + "x" + Math.round(height) + '"'

                        var fragment = null
                        if (crop.editingFragment){ //objectContainer
                            fragment = paletteControls.addPropertyByName(crop, "region")
                            crop.expand()
                        } else {
                            if (!crop)
                                return
                            var codeHandler = crop.visualParent.documentHandler.codeHandler
                            fragment = paletteControls.addPropertyByFragment(crop, codeHandler, "region")
                        }
                        var toWrite = '"' + Math.round(x) + "," + Math.round(y) + "," + Math.round(width) + "x" + Math.round(height) + '"'
                        fragment.write({"__ref": toWrite})
                        fragment.commit(Qt.rect(x, y, width, height))
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
                    Icons.CropIcon{
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

                        Icons.TwoWayArrowIcon{
                            anchors.centerIn: parent
                            width: 14
                            height: 14
                            strokeWidth: 1
                            color: resizeTool.preserveAspect ? palette.style.toolIconColor : 'transparent'
                        }
                    }

                    onApply: {
                        var resize = addTransformation("Resize")

                        var fragment = null
                        if (resize.editingFragment){
                            fragment = paletteControls.addPropertyByName(resize, "size")
                            resize.expand()
                        } else {
                            if (!resize)
                                return
                            var codeHandler = resize.visualParent.documentHandler.codeHandler
                            fragment = paletteControls.addPropertyByFragment(resize, codeHandler, "size")
                        }
                        var toWrite = '"' + Math.round(width) + "x" + Math.round(height) + '"'
                        fragment.write({"__ref": toWrite})
                        fragment.commit(Qt.size(width, height))
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

                    Icons.ResizeIcon{
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

                    onApply: {

                        var rotate = addTransformation("Rotate")

                        var fragment = null
                        if (rotate.editingFragment){
                            fragment = paletteControls.addPropertyByName(rotate, "degrees")
                            rotate.expand()
                        } else {
                            if (!rotate)
                                return
                            var codeHandler = rotate.visualParent.documentHandler.codeHandler
                            fragment = paletteControls.addPropertyByFragment(rotate, codeHandler, "degrees")
                        }
                        fragment.write(-angle)
                        fragment.commit(-angle)
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

                    Icons.RotateIcon{
                        anchors.centerIn: parent
                        strokeWidth: 1
                        color: palette.style.toolIconColor
                        width: 12
                        height: 12
                    }
                }
            }

            Workspace.ToolButton{
                id: perspectiveButton
                tool: Cv.PerspectiveTool {
                    id: perspectiveTool

                    canvas: imageView
                    labelInfoStyle: palette.style.labelStyle.textStyle
                    applyButton: palette.style.applyButton
                    cancelButton: palette.style.cancelButton

                    onApply: {
                        var perspective = addTransformation("Perspective")
                        var fragment = null
                        if (perspective.editingFragment){
                            fragment = paletteControls.addPropertyByName(perspective, "points")
                            perspective.expand()
                        } else {
                            if (!perspective)
                                return
                            var codeHandler = perspective.visualParent.documentHandler.codeHandler
                            fragment = paletteControls.addPropertyByFragment(perspective, codeHandler, "points")
                        }
                        var value = '['
                        value += 'Qt.point(' + Math.round(p1.x) + ", " + Math.round(p1.y) +'), '
                        value += 'Qt.point(' + Math.round(p2.x) + ", " + Math.round(p2.y) +'), '
                        value += 'Qt.point(' + Math.round(p3.x) + ", " + Math.round(p3.y) +'), '
                        value += 'Qt.point(' + Math.round(p4.x) + ", " + Math.round(p4.y) +')]'
                        fragment.write({"__ref": value})
                        fragment.commit([p1, p2, p3, p4])
                        paletteItem.transformImage.exec()
                        toolbox.activateTool(null)
                    }
                    onCancel: toolbox.activateTool(null)
                }
                content: Rectangle{
                    width: 20
                    height: 20
                    color: perspectiveButton.containsMouse || toolbox.selectedTool === rotateTool ? palette.style.toolIconHighlightBackground : 'transparent'
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
        paletteItem.transformImage = value
    }
    onValueFromBindingChanged: {
        paletteItem.transformImage = value
    }
}
