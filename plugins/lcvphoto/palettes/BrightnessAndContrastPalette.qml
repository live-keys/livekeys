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
import lcvphoto 1.0

CodePalette{
    id: palette

    type : "qml/BrightnessAndContrast"

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
                if ( !isBindingChange() ){
                    extension.writeProperties({
                        'brightness' : adjustmentBox.bandc.brightness
                    })
                }
            }
            stepSize: 1.0
            maximumValue: 200

            style: SliderStyle{
                groove: Rectangle {
                    implicitHeight: 8
                    color: "#15202c"
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

        Label{
            anchors.top: parent.top
            text: brightnessSlider.minimumValue
        }
        Label{
            anchors.top: parent.top
            anchors.right: parent.right
            text: brightnessSlider.maximumValue
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
                if ( !isBindingChange() ){
                    extension.writeProperties({
                        'contrast' : adjustmentBox.bandc.contrast
                    })
                }
            }
            stepSize: 0.01
            maximumValue: 3.0

            style: SliderStyle{
                groove: Rectangle {
                    implicitHeight: 8
                    color: "#15202c"
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

        Label{
            anchors.top: parent.top
            anchors.topMargin: 30
            text: contrastSlider.minimumValue
        }
        Label{
            anchors.top: parent.top
            anchors.topMargin: 30
            anchors.right: parent.right
            text: contrastSlider.maximumValue
        }
    }

    onInit: {
        adjustmentBox.bandc = value
    }

    onExtensionChanged: {
        extension.whenBinding = function(){
            extension.writeProperties({
                'brightness' : palette.value.brightness,
                'contrast' : palette.value.contrast
            })
        }
    }
}
