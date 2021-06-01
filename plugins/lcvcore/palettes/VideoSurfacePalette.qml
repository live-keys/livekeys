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
import fs 1.0 as Fs
import lcvcore 1.0 as Cv
import lcvimgproc 1.0 as Img
import visual.input 1.0 as Input

CodePalette{
    id: palette

    type : "qml/lcvcore#VideoSurfaceView"

    property QtObject theme: lk.layers.workspace.themes.current

    property QtObject style: QtObject{
        property color toolbarColor: theme.colorScheme.middleground
        property color boxColor: theme.colorScheme.background
        property color boxBorderColor: theme.colorScheme.backgroundBorder
        property real boxRadius: 3
        property QtObject labelStyle: theme.inputLabelStyle
        property Component saveButton: theme.buttons.save
    }

    item: Rectangle{
        id: paletteItem
        width: 500
        height: 300
        color: '#111'

        property QtObject videoSurfaceView: null
        onVideoSurfaceViewChanged: {
            videoSurface = Qt.binding(function(){ return videoSurfaceView.timeline.properties.videoSurface })
            imageView.image = Qt.binding(function(){ return videoSurfaceView.image })
        }

        property var videoSurface: null
        onVideoSurfaceChanged: {
            if ( videoSurface ){
                var scaleW = (imageView.maxWidth - 10) / videoSurface.imageWidth
                var scaleH = (imageView.maxHeight - 10) / videoSurface.imageHeight
                var scale = scaleW > scaleH ? scaleH : scaleW
                imageView.setScale(scale)
            }
        }

        Rectangle{
            id: topBar
            width: imageView.width > parent.width ? imageView.width : parent.width
            height: 30
            color: palette.style.toolbarColor

            Input.Button{
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
                Input.Label{
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
                Input.Label{
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
        paletteItem.videoSurfaceView = value
    }
    onValueFromBindingChanged: {
        paletteItem.videoSurfaceView = value
    }
}
