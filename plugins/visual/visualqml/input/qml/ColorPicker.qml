// A color picker inspired by this blog post http://blog.ruslans.com/2010/12/cute-quick-colorpicker.html

import QtQuick 2.11
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.4
import visual.input 1.0 as Input
import visual.shapes 1.0

Rectangle {
    id: root

    signal colorChanged(color changedColor)

    width: 400
    height: 250
    color: style.backgroundColor

    clip: true
    
    property QtObject style: ColorPickerStyle{}

    property double hue: hueSlider.value
    property double saturation: saturationBrightnessSlider.saturation
    property double brightness: saturationBrightnessSlider.brightness
    property double alpha: alphaSlider.value
    property color selectedColor: 'transparent'
    
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
    
    SaturationBrightnessSlider{
        id: saturationBrightnessSlider
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10

        height: parent.height - 20
        width: parent.width > 300 ? parent.width - 150 : 150

        hueColor: {
            var v = 1.0 - root.hue

            if(0.0 <= v && v < 0.16) {
                return Qt.rgba(1.0, 0.0, v/0.16, 1.0)
            } else if(0.16 <= v && v < 0.33) {
                return Qt.rgba(1.0 - (v-0.16)/0.17, 0.0, 1.0, 1.0)
            } else if(0.33 <= v && v < 0.5) {
                return Qt.rgba(0.0, ((v-0.33)/0.17), 1.0, 1.0)
            } else if(0.5 <= v && v < 0.76) {
                return Qt.rgba(0.0, 1.0, 1.0 - (v-0.5)/0.26, 1.0)
            } else if(0.76 <= v && v < 0.85) {
                return Qt.rgba((v-0.76)/0.09, 1.0, 0.0, 1.0)
            } else if(0.85 <= v && v <= 1.0) {
                return Qt.rgba(1.0, 1.0 - (v-0.85)/0.15, 0.0, 1.0)
            } else {
                throw new Error("Hue value is outside of expected boundaries: [0, 1]")
            }
        }
        
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
