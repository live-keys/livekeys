/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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
import live 1.0

LivePalette{
    id: palette

    type : "int"
    serialize : NativeValueCodeSerializer{}

    item: Rectangle{
        width: 280
        height: 40
        color: 'transparent'

        Slider{
            id: intSlider
            anchors.top: parent.top
            anchors.topMargin: 1
            anchors.left: parent.left
            anchors.leftMargin: 40
            width: parent.width - 80
            height: 15
            minimumValue: 0
            value: 0
            onValueChanged: palette.value = intSlider.value
            stepSize: 1.0
            maximumValue: 200

            style: SliderStyle{
                groove: Rectangle {
                    implicitHeight: 15
                    color: "#07131e"
                }
                handle: Rectangle{
                    width: 5
                    height: 18
                    border.width: 1
                    border.color: "#093357"
                    color: "#041f38"
                }
            }
        }

        Rectangle{
            width: 40
            height: 18
            color: "#071a2d"
            anchors.top: parent.top

            border.width: 1
            border.color: "#0e263c"

            Text{
                anchors.centerIn: parent
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 11
                font.weight: Font.Light
                text: intSlider.minimumValue
                color: '#d2d4db'
            }
        }

        Rectangle{
            width: 40
            height: 18
            color: "#071a2d"
            anchors.right: parent.right
            anchors.top: parent.top

            border.width: 1
            border.color: "#0e263c"

            Text{
                anchors.centerIn: parent
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 11
                font.weight: Font.Light
                text: intSlider.maximumValue
                color: '#d2d4db'
            }
        }

        Slider{
            id: zoomSlider
            width: parent.width
            height: 15
            minimumValue: 0
            value: 0
            onValueChanged: {
                var intMaxValue = value * value
                if ( intMaxValue < intSlider.value ){
                    var sqrt = Math.ceil(Math.sqrt(Math.floor(intSlider.value)))
                    zoomSlider.value = 15 > sqrt ? 15 : sqrt
                } else
                    intSlider.maximumValue = value * value
            }
            stepSize: 1.0
            maximumValue: 200

            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0

            style: SliderStyle{
                groove: Rectangle {
                    implicitHeight: 6
                    color: "transparent"
                }
                handle: Rectangle{
                    width: 40
                    height: 6
                    color: "#041f38"
                }
            }
        }
    }

    onInit: {
        var sqrt = Math.ceil(Math.sqrt(Math.floor(value))) + 1
        zoomSlider.value = 15 > sqrt ? 15 : sqrt

        intSlider.value = Math.floor(value)
    }
    onCodeChanged:{
        var sqrt = Math.ceil(Math.sqrt(Math.floor(value))) + 1
        zoomSlider.value = 15 > sqrt ? 15 : sqrt

        intSlider.value = Math.floor(value)
    }
}
