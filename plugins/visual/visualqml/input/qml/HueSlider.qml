import QtQuick 2.3
import QtQuick.Controls 2.10
import QtGraphicalEffects 1.0

Slider{
    id: hueSlider
    width: 10
    height: 200
    from: 0
    value: 0
    stepSize: 1 / 255
    to: 1.0
    orientation: Qt.Vertical

    handle: Rectangle {
        x: hueSlider.leftPadding + hueSlider.availableWidth / 2 - width / 2
        y: hueSlider.topPadding + hueSlider.visualPosition * (hueSlider.availableHeight - height)
        implicitWidth: 12
        implicitHeight: 12
        radius: 6
        color: "#ccc"
    }

    background: Rectangle {
        x: (hueSlider.width - 8) / 2
        y: hueSlider.topPadding + hueSlider.availableHeight / 2 - height / 2
        implicitWidth: 8
        implicitHeight: hueSlider.availableHeight
        radius: 2
        color: "#bdbebf"

         LinearGradient {
            anchors.fill: parent
            start: Qt.point(0, parent.height)
            end: Qt.point(0, 0)
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
}
