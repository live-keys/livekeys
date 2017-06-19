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
import QtGraphicalEffects 1.0
import live 1.0

LivePalette{
    id: palette

    type : "color"
    serialize : NativeValueCodeSerializer{}

    item: Rectangle{
        id: root

        property color selectedColor: "#fff"
        onSelectedColorChanged: palette.value = selectedColor

        width: 280
        height: 90
        color: 'transparent'

        Rectangle{
            width: 280
            height: 30
            color: 'transparent'
            Slider{
                id: hueSlider
                anchors.top: parent.top
                anchors.topMargin: 1
                anchors.left: parent.left
                anchors.leftMargin: 40
                width: parent.width - 80
                height: 15
                minimumValue: 0
                value: 0
                onValueChanged:
                    root.selectedColor = Qt.hsva(value / 255, root.selectedColor.hsvSaturation, root.selectedColor.hsvValue)
                stepSize: 1.0
                maximumValue: 255

                style: SliderStyle{
                    groove: Rectangle {
                        implicitHeight: 15
                         LinearGradient {
                            anchors.fill: parent
                            start: Qt.point(0, 0)
                            end: Qt.point(parent.width, 0)
                            gradient: Gradient {
                                GradientStop {
                                    position: 0.000
                                    color: Qt.rgba(1, 0, 0, 1)
                                }
                                GradientStop {
                                    position: 0.167
                                    color: Qt.rgba(1, 1, 0, 1)
                                }
                                GradientStop {
                                    position: 0.333
                                    color: Qt.rgba(0, 1, 0, 1)
                                }
                                GradientStop {
                                    position: 0.500
                                    color: Qt.rgba(0, 1, 1, 1)
                                }
                                GradientStop {
                                    position: 0.667
                                    color: Qt.rgba(0, 0, 1, 1)
                                }
                                GradientStop {
                                    position: 0.833
                                    color: Qt.rgba(1, 0, 1, 1)
                                }
                                GradientStop {
                                    position: 1.000
                                    color: Qt.rgba(1, 0, 0, 1)
                                }
                            }
                        }
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
                    text: hueSlider.minimumValue
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
                    text: hueSlider.maximumValue
                    color: '#d2d4db'
                }
            }

        }

        Rectangle{
            anchors.top: parent.top
            anchors.topMargin: 30
            width: 280
            height: 40
            color: 'transparent'

            Slider{
                id: saturationSlider
                anchors.top: parent.top
                anchors.topMargin: 1
                anchors.left: parent.left
                anchors.leftMargin: 40
                width: parent.width - 80
                height: 15
                minimumValue: 0
                value: 0
                onValueChanged:
                    root.selectedColor = Qt.hsva(root.selectedColor.hsvHue, value / 255, root.selectedColor.hsvValue, 1)
                stepSize: 1.0
                maximumValue: 255

                style: SliderStyle{
                    groove: Rectangle {
                        implicitHeight: 15
                         LinearGradient {
                            anchors.fill: parent
                            start: Qt.point(0, 0)
                            end: Qt.point(parent.width, 0)
                            gradient: Gradient {
                                GradientStop {
                                    position: 0.000
                                    color: Qt.hsva(
                                        root.selectedColor.hsvHue, 0, root.selectedColor.hsvValue, 1
                                    )
                                }
                                GradientStop {
                                    position: 1.000
                                    color: Qt.hsva(root.selectedColor.hsvHue, 255, root.selectedColor.hsvValue, 1)
                                }
                            }
                        }
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
                    text: saturationSlider.minimumValue
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
                    text: saturationSlider.maximumValue
                    color: '#d2d4db'
                }
            }

        }

        Rectangle{
            anchors.top: parent.top
            anchors.topMargin: 60
            width: 280
            height: 40
            color: 'transparent'

            Slider{
                id: valueSlider
                anchors.top: parent.top
                anchors.topMargin: 1
                anchors.left: parent.left
                anchors.leftMargin: 40
                width: parent.width - 80
                height: 15
                minimumValue: 0
                value: 0
                onValueChanged:
                    root.selectedColor = Qt.hsva(root.selectedColor.hsvHue, root.selectedColor.hsvSaturation, value / 255, 1)
                stepSize: 1.0
                maximumValue: 255

                style: SliderStyle{
                    groove: Rectangle {
                        implicitHeight: 15
                         LinearGradient {
                            anchors.fill: parent
                            start: Qt.point(0, 0)
                            end: Qt.point(parent.width, 0)
                            gradient: Gradient {
                                GradientStop {
                                    position: 0.000
                                    color: Qt.hsva(
                                        root.selectedColor.hsvHue, root.selectedColor.hsvSaturation, 0, 1
                                    )
                                }
                                GradientStop {
                                    position: 1.000
                                    color: Qt.hsva(root.selectedColor.hsvHue, root.selectedColor.hsvSaturation, 1, 1)
                                }
                            }
                        }
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
                    text: saturationSlider.minimumValue
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
                    text: saturationSlider.maximumValue
                    color: '#d2d4db'
                }
            }

}

    }

    onInit: {
        root.selectedColor = value
    }
    onCodeChanged:{
        root.selectedColor = value
    }
}
