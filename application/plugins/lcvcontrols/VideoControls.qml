/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
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

import QtQuick 2.1

Rectangle{
    id : root

    height : 25
    width : 100

    property Item videoCapture : Item{
        property bool paused : true
        property int totalFrames : 0
        property int currentFrame : 0
    }

    Rectangle{
        id : playPause
        width : 25
        height : parent.height
        color : "#113344"
        MouseArea{
            anchors.fill : parent
            onClicked : {
                root.videoCapture.paused = !root.videoCapture.paused
            }
        }
        Text{
            anchors.centerIn : parent
            text : root.videoCapture.paused ? '>' : '||'
            color : "#fff"
        }
    }

    Rectangle{
        anchors.right : parent.right
        anchors.top   : parent.top
        width  : parent.width - playPause.width
        height : parent.height
        color  : "#192a3b"
        Rectangle{
            height : parent.height
            width : root.videoCapture.totalFrames > 0 ?
                Math.round( (parent.width / root.videoCapture.totalFrames) * root.videoCapture.currentFrame ) : 0
            color : "#153848"
        }

        MouseArea{
            anchors.fill : parent
            onClicked : {
                root.videoCapture.seekTo(mouse.x * (root.videoCapture.totalFrames / parent.width ))
            }
        }
    }

}
