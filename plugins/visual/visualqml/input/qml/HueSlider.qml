import QtQuick 2.3
import QtQuick.Controls 1.6
import QtQuick.Controls.Styles 1.3
import QtGraphicalEffects 1.0

Slider{
    id: hueSlider
    width: 10
    height: 200
    minimumValue: 0
    value: 0
    stepSize: 1 / 255
    maximumValue: 1.0
    orientation: Qt.Vertical

    style: SliderStyle{
        groove: Rectangle {
            implicitHeight: 8
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
            width: 12
            height: 12
            radius: 6
            color: '#ccc'
        }
    }
}
