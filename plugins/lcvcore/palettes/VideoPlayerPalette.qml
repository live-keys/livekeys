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
import QtQuick.Controls.Styles 1.4
import editor 1.0
import live 1.0
import lcvcore 1.0

CodePalette{
    id: palette

    type : "qml/VideoDecoderView"

    item: Item{
        id: captureContainer

        property var videoDecoderView: null

        Connections{
            target: captureContainer
            function onFpsChanged(){ editFragment.updateBindings() }
            function onCurrentFrameChanged(){ editFragment.updateBindings() }
            function onPausedChanged(){ editFragment.updateBindings() }

            ignoreUnknownSignals: true
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
            value: parent.videoDecoderView ? parent.videoDecoderView.fps : 0
            onValueChanged: {
                if ( !parent || !parent.videoDecoderView )
                    return
                parent.videoDecoderView.fps = fpsSlider.value
                editFragment.writeProperties({
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
            videoCapture : parent.videoDecoderView
            width: 290
            onSeekTriggered: {
                editFragment.writeProperties({
                    'currentFrame' : videoCapture.currentFrame
                })
            }
            onPlayPauseTriggered: {
                editFragment.writeProperties({
                    'paused' : videoCapture.paused
                })
            }

            color: 'transparent'
        }
    }

    onInit: {
        captureContainer.videoDecoderView = value
        editFragment.whenBinding = function(){
            editFragment.writeProperties({
                'fps' : palette.value.fps,
                'currentFrame' : palette.value.currentFrame,
                'paused' : palette.value.paused
            })
        }
    }
    onValueFromBindingChanged: {
        captureContainer.videoDecoderView = value
    }
}
