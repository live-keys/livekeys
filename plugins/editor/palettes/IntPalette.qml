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
                    implicitHeight: 8
                    color: '#15202c'
                }
                handle: Rectangle{
                    width: 8
                    height: 18
                    radius: 5
                    border.width: 1
                    border.color: '#233569'
                    color: '#b2b2b2'
                }
            }
        }

        SliderLabel{
            anchors.top: parent.top
            text: intSlider.minimumValue
        }
        SliderLabel{
            anchors.top: parent.top
            anchors.right: parent.right
            text: intSlider.maximumValue
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
                    color: '#b2b2b2'
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
