/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
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
import QtGraphicalEffects 1.0
import live 1.0
import editor 1.0
import workspace 1.0 as Workspace
import visual.input 1.0 as Input

Item{
    id: root
    width: colorDisplay.width + (inputLoader.visible ? input.width + 2 : 0)
    height: 25 + (hsvPicker.visible ? hsvPicker.height + 2 : 0)

    property double colorDisplayWidth: 40
    property double colorDisplayHeight: 25

    property bool inputBoxVisible: true

    property QtObject defaultStyle: QtObject{
        property QtObject input: input.defaultStyle
        property double colorDisplayBoderWidth: 1
        property color  colorDisplayBoderColor: "#323232"
        property double colorDisplayRadius: 2
        property color adjustmentBackground: 'transparent'
        property color adjustmentBorderColor: 'transparent'
        property int adjustmentBorderWidth: 0
        property real adjustmentRadius: 3
        property QtObject labelStyle: hueLabelLeft.defaultStyle
    }
    property QtObject style: defaultStyle

    property color selectedColor: "#fff"

    function toggleHsvPicker(){
        hsvPicker.visible = !hsvPicker.visible
    }

    Rectangle{
        id: colorDisplay
        anchors.top: parent.top
        anchors.left: parent.left
        width: root.colorDisplayWidth
        height: root.colorDisplayHeight
        color: 'transparent'
        radius: root.style.colorDisplayRadius
        border.width: root.style.colorDisplayBoderWidth
        border.color: root.style.colorDisplayBoderColor

        Rectangle{
            anchors.centerIn: parent
            width: parent.width - 6
            height: parent.height - 6
            color: root.selectedColor
        }

        MouseArea{
            anchors.fill: parent
            onClicked: {
                root.toggleHsvPicker()
            }
        }
    }

    Loader{
        id: inputLoader
        visible: root.inputBoxVisible
        active: root.inputBoxVisible
        anchors.left: parent.left
        anchors.leftMargin: colorDisplay.width + 2

        Input.InputBox{
            id: input
            height: 25
            anchors.top: parent.top
            style: root.style.input

            text: root.selectedColor
            onKeyPressed: {
                if ( event.key === Qt.Key_Return )
                    root.selectedColor = text
            }
        }
    }

    Rectangle{
        id: hsvPicker
        width: 285
        height: 90
        anchors.top: parent.top
        anchors.topMargin: 27

        color: root.style.adjustmentBackground
        radius: root.style.adjustmentRadius
        border.color: root.style.adjustmentBorderColor
        border.width: root.style.adjustmentBorderWidth

        visible: false

        property real hsvHue : 0
        property real hsvSaturation : 0
        property real hsvValue: 0

        function setHsv(clr) {
            var r = Math.round(clr.r * 255);
            var g = Math.round(clr.g * 255);
            var b = Math.round(clr.b * 255);
            var max = Math.max(r, g, b), min = Math.min(r, g, b),
                d = max - min,
                h,
                s = (max === 0 ? 0 : d / max),
                v = max / 255;

            switch (max) {
                case min: h = 0; break;
                case r: h = (g - b) + d * (g < b ? 6: 0); h /= 6 * d; break;
                case g: h = (b - r) + d * 2; h /= 6 * d; break;
                case b: h = (r - g) + d * 4; h /= 6 * d; break;
            }

            hsvPicker.hsvHue = h
            hsvPicker.hsvSaturation = s
            hsvPicker.hsvValue = v
        }

        property color selectedColor: root.selectedColor
        onSelectedColorChanged: {
            setHsv(root.selectedColor)
        }

        Item{
            anchors.left: parent.left
            anchors.leftMargin: 2
            anchors.top: parent.top
            anchors.topMargin: 4
            width: 280
            height: 22

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
                    root.selectedColor = Qt.hsva(value / 255, hsvPicker.hsvSaturation, hsvPicker.hsvValue)
                stepSize: 1.0
                maximumValue: 255

                style: SliderStyle{
                    groove: Rectangle {
                        implicitHeight: 5
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
                        width: 11
                        height: 11
                        radius: 5
                        color: '#9b9da0'
                    }
                }
            }

            Input.LabelOnRectangle{
                id: hueLabelLeft
                width: 35
                height: 22
                anchors.top: parent.top
                text: hueSlider.minimumValue
                style: root.style.labelStyle
            }

            Input.LabelOnRectangle{
                anchors.top: parent.top
                anchors.right: parent.right
                width: 35
                height: 22
                text: hueSlider.maximumValue
                style: root.style.labelStyle
            }
        }

        Item{
            anchors.left: parent.left
            anchors.leftMargin: 2
            anchors.top: parent.top
            anchors.topMargin: 34
            width: 280
            height: 22

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
                onValueChanged: {
                    root.selectedColor = Qt.hsva(hsvPicker.hsvHue, value / 255, hsvPicker.hsvValue, 1)
                }
                stepSize: 1.0
                maximumValue: 255

                style: SliderStyle{
                    groove: Rectangle {
                        implicitHeight: 5
                         LinearGradient {
                            anchors.fill: parent
                            start: Qt.point(0, 0)
                            end: Qt.point(parent.width, 0)
                            gradient: Gradient {
                                GradientStop {
                                    position: 0.000
                                    color: Qt.hsva(
                                        hsvPicker.hsvHue, 0, 1, 1
                                    )
                                }
                                GradientStop {
                                    position: 1.000
                                    color: Qt.hsva(root.hsvHue, 255, 1, 1)
                                }
                            }
                        }
                    }
                    handle: Rectangle{
                        width: 11
                        height: 11
                        radius: 5
                        color: '#9b9da0'
                    }
                }
            }

            Input.LabelOnRectangle{
                anchors.top: parent.top
                width: 35
                height: 22
                text: saturationSlider.minimumValue
                style: root.style.labelStyle
            }

            Input.LabelOnRectangle{
                anchors.top: parent.top
                anchors.right: parent.right
                width: 35
                height: 22

                text: saturationSlider.maximumValue
                style: root.style.labelStyle
            }

        }

        Item{
            anchors.left: parent.left
            anchors.leftMargin: 2
            anchors.top: parent.top
            anchors.topMargin: 64
            width: 280
            height: 22

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
                    root.selectedColor = Qt.hsva(hsvPicker.hsvHue, hsvPicker.hsvSaturation, value / 255, 1)
                stepSize: 1.0
                maximumValue: 255

                style: SliderStyle{
                    groove: Rectangle {
                        implicitHeight: 5
                         LinearGradient {
                            anchors.fill: parent
                            start: Qt.point(0, 0)
                            end: Qt.point(parent.width, 0)
                            gradient: Gradient {
                                GradientStop {
                                    position: 0.000
                                    color: Qt.hsva(
                                        hsvPicker.hsvHue, hsvPicker.hsvSaturation, 0, 1
                                    )
                                }
                                GradientStop {
                                    position: 1.000
                                    color: Qt.hsva(hsvPicker.hsvHue, hsvPicker.hsvSaturation, 1, 1)
                                }
                            }
                        }
                    }
                    handle: Rectangle{
                        width: 11
                        height: 11
                        radius: 5
                        color: '#9b9da0'
                    }
                }
            }

            Input.LabelOnRectangle{
                anchors.top: parent.top
                text: valueSlider.minimumValue
                width: 35
                height: 22

                style: root.style.labelStyle
            }

            Input.LabelOnRectangle{
                anchors.top: parent.top
                anchors.right: parent.right
                text: valueSlider.maximumValue
                width: 35
                height: 22

                style: root.style.labelStyle
            }
        }
    }
}
