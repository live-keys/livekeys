// A color picker inspired by this blog post http://blog.ruslans.com/2010/12/cute-quick-colorpicker.html

import QtQuick 2.11
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.4
import visual.input 1.0 as Input
import visual.shapes 1.0

Item{
    id: root

    width: 400
    height: 250

    clip: true
    
    property QtObject style: ColorPickerStyle{}

    property color color: '#fff'
    onColorChanged: {
        hueSlider.value = color.hslHue
        saturationBrightnessSlider.color = color
        alphaSlider.value = root.enableAlpha ? color.a : 1.0
        updateColorFromHsla()
    }

    property double hue: hueSlider.value
    property double saturation: saturationBrightnessSlider.saturation
    property double brightness: saturationBrightnessSlider.brightness
    property double alpha: alphaSlider.value
    property color selectedColor: 'white'

    property bool enableAlpha: true
    
    function updateColorFromHsla(){
        var h = hue, s = saturation, b = brightness, a = alpha
        
        var lightness = (2 - s)*b
        var satHSL = s*b/((lightness <= 1) ? lightness : 2 - lightness)
        lightness /= 2

        selectedColor = Qt.hsla(h, satHSL, lightness, a)
    }
    
    function colorString(clr, a) {
        return "#" + ((Math.ceil(a*255) + 256).toString(16).substr(1, 2) + clr.toString().substr(3, 6)).toUpperCase()
    }

    function getChannelString(clr, channelNo) {
        return parseInt(clr.toString().substr(channelNo * 2 + 1, 2), 16)
    }

    Rectangle{
        anchors.fill: parent
        color: style.backgroundColor

        SaturationBrightnessSlider{
            id: saturationBrightnessSlider
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 10

            height: parent.height - 20
            width: parent.width > 300 ? parent.width - 150 : 150

            hueColor: Qt.hsla(root.hue, 1.0, 0.5, 1.0)

            onValueSelected: {
                root.updateColorFromHsla()
            }
        }

        // hue picking slider
        HueSlider{
            id: hueSlider
            width: 12
            height: parent.height - 20
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 120
            onValueChanged: root.updateColorFromHsla()
        }

        AlphaSlider{
            id: alphaSlider
            visible: root.enableAlpha
            width: 12
            height: parent.height - 20
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 100
            onValueChanged: root.updateColorFromHsla()
        }

        Rectangle{
            width: 80
            height: 50
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10
            color: 'transparent'
            border.width: 1
            border.color: root.style.colorPanelBorderColor

            Chessboard {
                anchors.fill: parent
                anchors.margins: 1
                cellSize: width / 8
            }

            Rectangle{
                anchors.fill: parent
                anchors.margins: 1
                color: root.selectedColor
            }
        }

        Column{
            anchors.top: parent.top
            anchors.topMargin: 65
            anchors.right: parent.right
            anchors.rightMargin: 10
            spacing: 3

            Input.InputBox{
                width: 80
                height: 20
                margins: 3
                style: root.style.inputBoxStyle
                text: colorString(root.selectedColor, alphaSlider.value)
            }

            Item{
                width: 80
                height: 20
                visible: root.height > 230
                Input.Label{
                    x: 3
                    text: 'H:'
                    textStyle: root.style.labelStyle
                }
                Input.InputBox{
                    anchors.right: parent.right
                    width: 60
                    height: 20
                    margins: 3
                    style: root.style.inputBoxStyle
                    text: hueSlider.value.toFixed(2)
                }
            }
            Item{
                width: 80
                height: 20
                visible: root.height > 230
                Input.Label{
                    x: 3
                    text: 'S:'
                    textStyle: root.style.labelStyle
                }
                Input.InputBox{
                    anchors.right: parent.right
                    width: 60
                    height: 20
                    margins: 3
                    style: root.style.inputBoxStyle
                    text: saturationBrightnessSlider.saturation.toFixed(2)
                }
            }
            Item{
                width: 80
                height: 20
                visible: root.height > 230
                Input.Label{
                    x: 3
                    text: 'B:'
                    textStyle: root.style.labelStyle
                }
                Input.InputBox{
                    anchors.right: parent.right
                    width: 60
                    height: 20
                    margins: 3
                    style: root.style.inputBoxStyle
                    text: saturationBrightnessSlider.brightness.toFixed(2)
                }
            }
            Item{
                width: 80
                height: 20
                Input.Label{
                    x: 3
                    text: 'R:'
                    textStyle: root.style.labelStyle
                }
                Input.InputBox{
                    anchors.right: parent.right
                    width: 60
                    height: 20
                    margins: 3
                    style: root.style.inputBoxStyle
                    text: getChannelString(root.selectedColor, 0)
                }
            }
            Item{
                width: 80
                height: 20
                Input.Label{
                    x: 3
                    text: 'G:'
                    textStyle: root.style.labelStyle
                }
                Input.InputBox{
                    anchors.right: parent.right
                    width: 60
                    height: 20
                    margins: 3
                    style: root.style.inputBoxStyle
                    text: getChannelString(root.selectedColor, 1)
                }
            }
            Item{
                width: 80
                height: 20
                Input.Label{
                    x: 3
                    text: 'B:'
                    textStyle: root.style.labelStyle
                }
                Input.InputBox{
                    anchors.right: parent.right
                    width: 60
                    height: 20
                    margins: 3
                    style: root.style.inputBoxStyle
                    text: getChannelString(root.selectedColor, 2)
                }
            }
            Item{
                width: 80
                height: 20
                Input.Label{
                    x: 3
                    text: 'A:'
                    textStyle: root.style.labelStyle
                }
                Input.InputBox{
                    anchors.right: parent.right
                    width: 60
                    height: 20
                    margins: 3
                    style: root.style.inputBoxStyle
                    text: Math.ceil(alphaSlider.value * 255)
                }
            }
        }
    }


    

}
