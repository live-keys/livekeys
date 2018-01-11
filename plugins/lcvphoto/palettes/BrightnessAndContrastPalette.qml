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
import editor 1.0
import lcvphoto 1.0

LivePalette{
    id: palette

    type : "double"
    serialize : BrightnessAndContrastSerializer{}

    item: Rectangle{
        id: adjustmentBox
        width: 280
        height: 68
        color: 'transparent'

        property BrightnessAndContrast bandc: null

        Slider{
            id: brightnessSlider
            anchors.top: parent.top
            anchors.topMargin: 1
            anchors.left: parent.left
            anchors.leftMargin: 40
            width: parent.width - 80
            height: 15
            minimumValue: -200
            value: adjustmentBox.bandc ? adjustmentBox.bandc.brightness : 0
            onValueChanged: {
                adjustmentBox.bandc.brightness = value
                palette.value = adjustmentBox.bandc
            }
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
                text: brightnessSlider.minimumValue
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
                text: brightnessSlider.maximumValue
                color: '#d2d4db'
            }
        }

        Slider{
            id: contrastSlider

            anchors.left: parent.left
            anchors.leftMargin: 40
            anchors.top: parent.top
            anchors.topMargin: 31

            width: parent.width - 80

            height: 15
            minimumValue: 0
            value: adjustmentBox.bandc ? adjustmentBox.bandc.contrast : 0
            onValueChanged: {
                adjustmentBox.bandc.contrast = value
                palette.value = adjustmentBox.bandc
            }
            stepSize: 0.01
            maximumValue: 3.0

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
            anchors.topMargin: 30

            border.width: 1
            border.color: "#0e263c"

            Text{
                anchors.centerIn: parent
                font.family: "Open Sans, sans-serif"
                font.weight: Font.Light
                font.pixelSize: 11
                text: contrastSlider.minimumValue
                color: '#d2d4db'
            }
        }

        Rectangle{
            width: 40
            height: 18
            color: "#071a2d"
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.topMargin: 30

            border.width: 1
            border.color: "#0e263c"

            Text{
                anchors.centerIn: parent
                font.family: "Open Sans, sans-serif"
                font.pixelSize: 11
                text: contrastSlider.maximumValue.toFixed(1)
                color: '#d2d4db'
            }
        }
    }

    onInit: {
        adjustmentBox.bandc = value
    }
    onCodeChanged:{
        adjustmentBox.bandc = value
    }
}
