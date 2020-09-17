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
import workspace 1.0 as Workspace

CodePalette{
    id: palette
    type : "qml/int"

    property QtObject paletteStyle : lk ? lk.layers.workspace.extensions.editqml.paletteStyle : null

    item: Rectangle{
        width: 330
        height: 23
        color: 'transparent'


        Workspace.InputBox{
            id: numberInput
            anchors.left: parent.left
            width: 70
            height: 25
            style: paletteStyle ? paletteStyle.inputStyle : defaultStyle
            text: intSlider.value
        }

        Slider{
            id: intSlider
            anchors.top: parent.top
            anchors.topMargin: 3
            anchors.left: parent.left
            anchors.leftMargin: numberInput.width + leftLabel.width + 5
            width: parent.width - numberInput.width - leftLabel.width - rightLabel.width - 10
            height: 15
            minimumValue: 0
            value: 0
            onValueChanged: {
                palette.value = intSlider.value
                if ( !palette.isBindingChange() )
                    extension.write(palette.value)
            }
            stepSize: 1.0
            maximumValue: 25

            style: SliderStyle{
                groove: Rectangle {
                    implicitHeight: 5
                    color: paletteStyle ? paletteStyle.backgroundColor : '#0b111c'
                }
                handle: Rectangle{
                    width: 11
                    height: 11
                    radius: 5
                    color: '#9b9da0'
                }
            }
            activeFocusOnPress: true
            wheelEnabled: intSlider.activeFocus
        }

        Workspace.NumberLabel{
            id: leftLabel
            mode: 1
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: numberInput.width + 2

            width: 50
            height: 25
            wheelEnabled: leftLabel.activeFocus || numberInput.inputActiveFocus || intSlider.activeFocus
            style: palette.paletteStyle ? palette.paletteStyle.labelStyle : leftLabel.defaultStyle

            up: function(){
                if (intSlider.minimumValue === 0 && intSlider.maximumValue > 25)
                    intSlider.minimumValue = 25
                else if (intSlider.minimumValue === -25 && intSlider.maximumValue > 0)
                    intSlider.minimumValue = 0
                else if (intSlider.minimumValue < -25 && intSlider.minimumValue / 2 < intSlider.maximumValue)
                    intSlider.minimumValue = intSlider.minimumValue / 2
                else if (intSlider.minimumValue > 0 && 2*intSlider.minimumValue < intSlider.maximumValue)
                    intSlider.minimumValue = 2*intSlider.minimumValue

                if (intSlider.minimumValue > 25600) intSlider.minimumValue = 25600


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

                if (intSlider.minimumValue < -25600) intSlider.minimumValue = -25600


                if (intSlider.value < intSlider.minimumValue )
                    intSlider.value = intSlider.minimumValue
            }
            text: intSlider.minimumValue
        }



        Workspace.NumberLabel{
            id: rightLabel
            mode: 2
            anchors.top: parent.top
            anchors.right: parent.right

            width: 50
            height: 25
            wheelEnabled: rightLabel.activeFocus || numberInput.inputActiveFocus || intSlider.activeFocus

            style: palette.paletteStyle ? palette.paletteStyle.labelStyle : leftLabel.defaultStyle

            up: function(){
                if (intSlider.maximumValue === 0)
                    intSlider.maximumValue = 25
                else if (intSlider.maximumValue === -25)
                    intSlider.maximumValue = 0
                else if (intSlider.maximumValue > 0)
                    intSlider.maximumValue = 2*intSlider.maximumValue
                else if (intSlider.maximumValue < 0)
                    intSlider.maximumValue = intSlider.maximumValue / 2

                if (intSlider.maximumValue > 25600) intSlider.maximumValue = 25600


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
                else if (intSlider.maximumValue > 25 && intSlider.maximumValue / 2 > intSlider.minimumValue)
                    intSlider.maximumValue = intSlider.maximumValue / 2

                if (intSlider.maximumValue < -25600) intSlider.maximumValue = -25600


                if (intSlider.value > intSlider.maximumValue)
                    intSlider.value = intSlider.maximumValue
            }

            text: intSlider.maximumValue
        }
    }

    onInit: {
        var floorValue = Math.floor(value)
        if (floorValue < intSlider.minimumValue || floorValue > intSlider.maximumValue){
            if (floorValue > 0){
                while (intSlider.maximumValue < floorValue){
                    intSlider.minimumValue = intSlider.maximumValue
                    intSlider.maximumValue = 2*intSlider.maximumValue
                }
            } else if (floorValue < 0){
                intSlider.minimumValue = -25
                intSlider.maximumValue = 0
                while (intSlider.minimumValue > floorValue){
                    intSlider.maximumValue = intSlider.minimumValue
                    intSlider.minimumValue = 2*intSlider.minimumValue

                }
            }
        }
        intSlider.value = floorValue
    }

    onExtensionChanged: {
        extension.whenBinding = function(){
            extension.write(palette.value)
        }
    }
}
