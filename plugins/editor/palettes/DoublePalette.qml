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
        width: 330
        height: 30
        color: 'transparent'

        Slider{
            id: intSlider
            anchors.top: parent.top
            anchors.topMargin: 1
            anchors.left: parent.left
            anchors.leftMargin: leftLabel.width + 3
            width: parent.width - leftLabel.width - rightLabel.width - 10
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

        Slider{
            id: fractionalSlider

            anchors.left: parent.left
            anchors.leftMargin: leftLabel.width + 3
            anchors.top: parent.top
            anchors.topMargin: 15

            width: intSlider.width

            height: 10
            minimumValue: 0
            value: 0
            onValueChanged: {
                if (fractionalSlider.value !== 1.0)
                    palette.value = intSlider.value + fractionalSlider.value

                if ( !palette.isBindingChange() )
                    extension.write(palette.value)
            }
            stepSize: 0.01
            maximumValue: 1.0

            style: SliderStyle{
                groove: Rectangle {
                    implicitHeight: 1
                    color: 'transparent'
                }
                handle: Triangle{
                    width: 8
                    height: 8
                    color: '#9b9da0'
                    rotation: Triangle.Top
                }
            }
        }

        Label{
            id: leftLabel
            mode: 1
            anchors.top: parent.top
            anchors.left: parent.left
            up: function(){
                if (intSlider.minimumValue === 0 && intSlider.maximumValue > 25)
                    intSlider.minimumValue = 25
                else if (intSlider.minimumValue === -25 && intSlider.maximumValue > 0)
                    intSlider.minimumValue = 0
                else if (intSlider.minimumValue < 0 && intSlider.minimumValue / 2 < intSlider.maximumValue)
                    intSlider.minimumValue = intSlider.minimumValue / 2
                else if (intSlider.minimumValue > 0 && 2*intSlider.minimumValue < intSlider.maximumValue)
                    intSlider.minimumValue = 2*intSlider.minimumValue

                if (intSlider.value < intSlider.minimumValue )
                    intSlider.value = intSlider.minimumValue
            }
            down: function(){
                if (intSlider.minimumValue === 0)
                    intSlider.minimumValue = -25
                else if (intSlider.minimumValue === 25)
                    intSlider.minimumValue = 0
                else if (intSlider.minimumValue < 0)
                    intSlider.minimumValue = 2*intSlider.minimumValue
                else if (intSlider.minimumValue > 0)
                    intSlider.minimumValue = intSlider.minimumValue / 2

                if (intSlider.value < intSlider.minimumValue )
                    intSlider.value = intSlider.minimumValue
            }
            text: intSlider.minimumValue
        }

        Label{
            id: rightLabel
            mode: 2
            anchors.top: parent.top
            anchors.right: parent.right

            up: function(){
                if (intSlider.maximumValue === 0)
                    intSlider.maximumValue = 25
                else if (intSlider.maximumValue === -25)
                    intSlider.maximumValue = 0
                else if (intSlider.maximumValue > 0)
                    intSlider.maximumValue = 2*intSlider.maximumValue
                else if (intSlider.maximumValue < 0)
                    intSlider.maximumValue = intSlider.maximumValue / 2

                if (intSlider.value > intSlider.maximumValue)
                    intSlider.value = intSlider.maximumValue
            }
            down: function(){
                if (intSlider.maximumValue === 0 && intSlider.minimumValue < -25)
                    intSlider.maximumValue = -25
                else if (intSlider.maximumValue === 25 && intSlider.minimumValue < 0)
                    intSlider.maximumValue = 0
                else if (intSlider.maximumValue < 0 && 2*intSlider.maximumValue > intSlider.minimumValue)
                    intSlider.maximumValue = 2*intSlider.maximumValue
                else if (intSlider.maximumValue > 0 && intSlider.maximumValue / 2 > intSlider.minimumValue)
                    intSlider.maximumValue = intSlider.maximumValue / 2

                if (intSlider.value > intSlider.maximumValue)
                    intSlider.value = intSlider.maximumValue
            }

            text: intSlider.maximumValue
        }

    }

    onExtensionChanged: {
        extension.whenBinding = function(){
            extension.write(palette.value)
        }
    }

    onInit: {
        intSlider.value = Math.floor(value)
        fractionalSlider.value = value - intSlider.value
    }
}
