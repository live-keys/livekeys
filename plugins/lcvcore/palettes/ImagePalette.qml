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

    type : "qml/lcvcore#Mat"

    property QtObject paletteStyle : lk ? lk.layers.workspace.extensions.editqml.paletteStyle : null

    property QtObject defaultStyle: QtObject{
        property color toolbarColor: "#333"
        property color boxColor: '#333'
        property color boxBorderColor: "#666"
        property int boxBorderWidth: 1
        property real boxRadius: 3
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
    }
    property QtObject style: QtObject{
        property color toolbarColor: paletteStyle ? paletteStyle.colorScheme.middleground : palette.defaultStyle.toolbarColor
        property color boxColor: paletteStyle ? paletteStyle.colorScheme.background : palette.defaultStyle.boxColor
        property color boxBorderColor: paletteStyle ? paletteStyle.colorScheme.backgroundBorder : palette.defaultStyle.boxBorderColor
        property int boxBorderWidth: 1
        property real boxRadius: 3
        property QtObject labelStyle: paletteStyle ? paletteStyle.labelStyle : palette.defaultStyle.labelStyle
        property Component saveButton: paletteStyle ? paletteStyle.buttons.save : palette.defaultStyle.saveButton
    }

    item: Item{
        id: paletteItem
        width: 500
        height: 300

        property QtObject image: null
        onImageChanged: {
            imageView.image = image
            imageView.autoScale()
        }

        Rectangle{
            id: topBar
            width: imageView.width + 30 > 150 ? imageView.width + 30 : 150
            height: 30
            color: palette.style.toolbarColor

            Workspace.Button{
                anchors.left: parent.left
                anchors.leftMargin: 30
                anchors.verticalCenter: parent.verticalCenter
                width: 30
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

        Cv.NavigableImageView{
            id: imageView
            anchors.top: parent.top
            anchors.topMargin: 31

            maxWidth: paletteItem.width
            maxHeight: paletteItem.height - anchors.topMargin

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
        paletteItem.image = value
    }
}
