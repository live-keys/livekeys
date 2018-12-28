/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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
import QtQuick.Controls.Styles 1.4
import editor 1.0
import live 1.0
import lcvcore 1.0

CodePalette{
    id: palette

    type : "qml/VideoCapture"

    item: Item{
        id: captureContainer

        property VideoCapture videoCapture: null

        Connections{
            target: captureContainer.videoCapture
            onFpsChanged: extension.updateBindings()
            onCurrentFrameChanged: extension.updateBindings()
            onPausedChanged: extension.updateBindings()
        }

        width: 290
        height: 60

        Slider{
            id: fpsSlider
            anchors.right: parent.right
            anchors.rightMargin: 12
            width: parent.width - 70
            height: 15
            minimumValue: 0
            value: parent.videoCapture.fps
            onValueChanged: {
                parent.videoCapture.fps = fpsSlider.value
                extension.writeProperties({
                    'fps' : fpsSlider.value
                })
            }
            stepSize: 1.0
            maximumValue: 150

            style: SliderStyle{
                groove: Rectangle {
                    implicitHeight: 4
                    color: '#15202c'
                }
                handle: Rectangle{
                    width: 6
                    height: 12
                    radius: 4
                    border.width: 1
                    border.color: '#233569'
                    color: '#b2b2b2'
                }
            }
        }

        VideoControls{
            anchors.top: parent.top
            anchors.topMargin: 20
            videoCapture : parent.videoCapture
            width: 290
            onSeekTriggered: {
                extension.writeProperties({
                    'currentFrame' : videoCapture.currentFrame
                })
            }
            onPlayPauseTriggered: {
                extension.writeProperties({
                    'paused' : videoCapture.paused
                })
            }

            color: 'transparent'
        }
    }

    onInit: {
        captureContainer.videoCapture = value
    }

    onExtensionChanged: {
        extension.whenBinding = function(){
            extension.writeProperties({
                'fps' : palette.value.fps,
                'currentFrame' : palette.value.currentFrame,
                'paused' : palette.value.paused
            })
        }
    }
}
