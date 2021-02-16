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
import live 1.0
import visual.shapes 1.0

Rectangle{
    id : root

    height : 30
    width : 100
    color: '#000d18'

    property var videoCapture : null

    signal playPauseTriggered(bool paused)
    signal seekTriggered(int currentFrame)

    Item{
        id : playPause
        width : 35
        height : parent.height

        Triangle{
            anchors.centerIn: parent
            width: 11
            height: 16
            color: "#aeaeae"
            rotation: Triangle.Right
            visible: root.videoCapture ? root.videoCapture.paused : false
        }

        Text{
            anchors.centerIn : parent
            text : '||'
            color : "#aeaeae"
            font.pixelSize: 12
            font.bold: true
            visible: root.videoCapture ? !root.videoCapture.paused : false
        }

        MouseArea{
            anchors.fill : parent
            onClicked : {
                root.videoCapture.paused = !root.videoCapture.paused
                root.playPauseTriggered(root.videoCapture.paused)
            }
        }
    }

    Rectangle{
        anchors.right : parent.right
        anchors.rightMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        width  : parent.width - 70
        height : 8
        color  : "#0d1a2a"
        Rectangle{
            height : parent.height
            width : root.videoCapture && root.videoCapture.totalFrames > 0 ?
                Math.round( (parent.width / root.videoCapture.totalFrames) * root.videoCapture.currentFrame ) : 0
            color : "#aeaeae"
        }

        MouseArea{
            anchors.fill : parent
            onClicked : {
                if (!root.videoCapture) return
                root.videoCapture.seekTo(mouse.x * (root.videoCapture.totalFrames / parent.width))
                root.seekTriggered(mouse.x * (root.videoCapture.totalFrames / parent.width))
            }
        }
    }

}
