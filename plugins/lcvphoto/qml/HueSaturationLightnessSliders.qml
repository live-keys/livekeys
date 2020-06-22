import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Controls 2.12 as QC2
import QtGraphicalEffects 1.0
import workspace 1.0 as Workspace

Rectangle{
    id: root

    width: 280
    height: 90
    color: 'transparent'

    property alias hue: hueSlider.value
    property alias saturation: saturationSlider.value
    property alias lightness: valueSlider.value

    property QtObject defaultStyle : QtObject{
        property QtObject labelStyle: hueLabelLeft ? hueLabelLeft.defaultStyle : null
    }

    property QtObject style: defaultStyle

    Component.onCompleted: {
        hueSlider.value = 180
        saturationSlider.value = 100
        valueSlider.value = 100
    }

    Item{
        width: 280
        height: 30

        QC2.Slider{
            id: hueSlider
            anchors.top: parent.top
            anchors.topMargin: 1
            anchors.left: parent.left
            anchors.leftMargin: 40
            width: parent.width - 70
            height: 15
            from: 0
            stepSize: 1.0
            to: 360

            background: Rectangle {
                implicitHeight: 5
                width: hueSlider.availableWidth
                height: implicitHeight
                y: hueSlider.topPadding + hueSlider.availableHeight / 2 - height / 2
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
            handle: Rectangle {
                x: hueSlider.leftPadding + hueSlider.visualPosition * (hueSlider.availableWidth - width)
                y: hueSlider.topPadding + hueSlider.availableHeight / 2 - height / 2
                implicitWidth: 11
                implicitHeight: 11
                radius: 5
                color: '#9b9da0'
            }
        }

        Workspace.LabelOnRectangle{
            id: hueLabelLeft
            width: 35
            height: 22
            anchors.top: parent.top
            text: hueSlider.from
            style: root.style ? root.style.labelStyle : defaultStyle
        }

        Workspace.LabelOnRectangle{
            anchors.top: parent.top
            anchors.right: parent.right
            width: 35
            text: hueSlider.to
            style: root.style ? root.style.labelStyle : defaultStyle
        }
    }

    Rectangle{
        anchors.top: parent.top
        anchors.topMargin: 26
        width: 280
        height: 40
        color: 'transparent'

        QC2.Slider{
            id: saturationSlider
            anchors.top: parent.top
            anchors.topMargin: 1
            anchors.left: parent.left
            anchors.leftMargin: 40
            width: parent.width - 70
            height: 15
            from: 0
            stepSize: 1.0
            to: 255

            background: Rectangle{
                implicitHeight: 5
                width: parent.availableWidth
                height: implicitHeight
                y: saturationSlider.topPadding + saturationSlider.availableHeight / 2 - height / 2
                LinearGradient {
                    anchors.fill: parent
                    start: Qt.point(0, 0)
                    end: Qt.point(parent.width, 0)
                    gradient: Gradient {
                        GradientStop {
                            position: 0.000
                            color: "#000"
                        }
                        GradientStop {
                            position: 1.000
                            color: "#f00"
                        }
                    }
                }
            }
            handle: Rectangle{
                x: saturationSlider.leftPadding + saturationSlider.visualPosition * (saturationSlider.availableWidth - width)
                y: saturationSlider.topPadding + saturationSlider.availableHeight / 2 - height / 2
                implicitWidth: 11
                implicitHeight: 11
                radius: 5
                color: '#9b9da0'
            }
        }

        Workspace.LabelOnRectangle{
            anchors.top: parent.top
            width: 35
            text: saturationSlider.from
            style: root.style ? root.style.labelStyle : defaultStyle
        }

        Workspace.LabelOnRectangle{
            anchors.top: parent.top
            anchors.right: parent.right
            width: 35

            text: saturationSlider.to
            style: root.style ? root.style.labelStyle : defaultStyle
        }

    }

    Item{
        anchors.top: parent.top
        anchors.topMargin: 51
        width: 280
        height: 40

        QC2.Slider{
            id: valueSlider
            anchors.top: parent.top
            anchors.topMargin: 1
            anchors.left: parent.left
            anchors.leftMargin: 40
            width: parent.width - 70
            height: 15
            from: 0
            stepSize: 1.0
            to: 255

            background: Rectangle{
                implicitHeight: 5
                width: parent.availableWidth
                height: implicitHeight
                y: valueSlider.topPadding + valueSlider.availableHeight / 2 - height / 2
                LinearGradient {
                    anchors.fill: parent
                    start: Qt.point(0, 0)
                    end: Qt.point(parent.width, 0)
                    gradient: Gradient {
                        GradientStop {
                            position: 0.000
                            color: "#000"
                        }
                        GradientStop {
                            position: 1.000
                            color: "#fff"
                        }
                    }
                }
            }
            handle: Rectangle{
                x: valueSlider.leftPadding + valueSlider.visualPosition * (valueSlider.availableWidth - width)
                y: valueSlider.topPadding + valueSlider.availableHeight / 2 - height / 2
                implicitWidth: 11
                implicitHeight: 11
                radius: 5
                color: '#9b9da0'
            }
        }

        Workspace.LabelOnRectangle{
            anchors.top: parent.top
            text: valueSlider.from
            width: 35

            style: root.style ? root.style.labelStyle : defaultStyle
        }

        Workspace.LabelOnRectangle{
            anchors.top: parent.top
            anchors.right: parent.right
            text: valueSlider.to
            width: 35

            style: root.style ? root.style.labelStyle : defaultStyle
        }
    }
}
