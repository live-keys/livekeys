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
import editor 1.0
import live 1.0

CodePalette{
    id: palette
    type : "qml/double"

    item: Rectangle{
        width: 280
        height: 60
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
            onValueChanged: {
                palette.value = intSlider.value + fractionalSlider.value
                if ( !palette.isBindingChange() )
                    extension.write(palette.value)
            }
            stepSize: 1.0
            maximumValue: 200

            style: SliderStyle{
                groove: Rectangle {
                    implicitHeight: 5
                    color: '#0b111c'
                }
                handle: Rectangle{
                    width: 11
                    height: 11
                    radius: 5
                    color: '#9b9da0'
                }
            }
        }

        Label{
            anchors.top: parent.top
            text: intSlider.minimumValue
        }

        Label{
            anchors.top: parent.top
            anchors.right: parent.right
            text: intSlider.maximumValue
        }

        Slider{
            id: fractionalSlider

            anchors.left: parent.left
            anchors.leftMargin: 40
            anchors.top: parent.top
            anchors.topMargin: 26

            width: parent.width - 80

            height: 15
            minimumValue: 0
            value: 0
            onValueChanged: {
                palette.value = intSlider.value + fractionalSlider.value
                if ( !palette.isBindingChange() )
                    extension.write(palette.value)
            }
            stepSize: 0.01
            maximumValue: 1.0

            style: SliderStyle{
                groove: Rectangle {
                    implicitHeight: 5
                    color: '#0b111c'
                }
                handle: Rectangle{
                    width: 11
                    height: 11
                    radius: 5
                    color: '#9b9da0'
                }
            }
        }

        Label{
            anchors.top: parent.top
            anchors.topMargin: 25
            text: fractionalSlider.minimumValue.toFixed(1)
        }
        Label{
            anchors.top: parent.top
            anchors.topMargin: 25
            anchors.right: parent.right
            text: fractionalSlider.maximumValue.toFixed(1)
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
                    color: '#9b9da0'
                }
            }
        }
    }

    onExtensionChanged: {
        extension.whenBinding = function(){
            extension.write(palette.value)
        }
    }

    onInit: {
        var sqrt = Math.ceil(Math.sqrt(Math.floor(value))) + 1
        zoomSlider.value = 15 > sqrt ? 15 : sqrt

        intSlider.value = Math.floor(value)
        fractionalSlider.value = value - intSlider.value
    }
}
