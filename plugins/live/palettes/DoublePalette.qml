import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import live 1.0

LivePalette{
    id: palette

    type : "double"
    serialize : NativeValueCodeSerializer{}

    item: Rectangle{
        width: 250
        height: 90
        color: 'transparent'

        Slider{
            id: intSlider
            width: parent.width
            height: 15
            minimumValue: 0
            value: 0
            onValueChanged: palette.value = intSlider.value + fractionalSlider.value
            stepSize: 1.0
            maximumValue: 200

            style: SliderStyle{
                groove: Rectangle {
                    implicitHeight: 15
                    color: "#010305"
                }
                handle: Rectangle{
                    width: 10
                    height: 15
                    color: "#0b273f"
                }
            }

            Text{
                text: intSlider.minimumValue
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.topMargin: 20
                color: '#7d87a3'
            }
            Text{
                text: intSlider.value
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 20
                color: '#d8d9db'
            }
            Text{
                text: intSlider.maximumValue
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.topMargin: 20
                color: '#7d87a3'
            }

        }

        Slider{
            id: fractionalSlider
            width: parent.width
            height: 15
            minimumValue: 0
            value: 0
            onValueChanged: palette.value = intSlider.value + fractionalSlider.value
            stepSize: 0.05
            maximumValue: 1.0
            anchors.top: parent.top
            anchors.topMargin: 45

            style: SliderStyle{
                groove: Rectangle {
                    implicitHeight: 15
                    color: "#010305"
                }
                handle: Rectangle{
                    width: 10
                    height: 15
                    color: "#0b273f"
                }
            }

            Text{
                text: fractionalSlider.minimumValue
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.topMargin: 20
                color: '#7d87a3'
            }
            Text{
                text: fractionalSlider.value.toFixed(2)
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 20
                color: '#d8d9db'
            }
            Text{
                text: fractionalSlider.maximumValue
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.topMargin: 20
                color: '#7d87a3'
            }
        }

        Slider{
            id: zoomSlider
            width: parent.width
            height: 15
            minimumValue: 0
            value: 0
            onValueChanged: intSlider.maximumValue = value * value
            stepSize: 1.0
            maximumValue: 200

            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0

            style: SliderStyle{
                groove: Rectangle {
                    implicitHeight: 15
                    color: "transparent"
                }
                handle: Rectangle{
                    width: 10
                    height: 15
                    color: "#0b273f"
                }
            }
        }
    }

    onInit: {
        intSlider.value = Math.floor(value)
        fractionalSlider.value = value - intSlider.value
    }
    onCodeChanged:{
        intSlider.value = Math.floor(value)
        fractionalSlider.value = value - intSlider.valu
    }
}
