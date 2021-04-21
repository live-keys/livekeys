import QtQuick 2.3
import QtQuick.Controls 2.10
import QtGraphicalEffects 1.0
import visual.shapes 1.0

Slider{
    id: alphaSlider
    width: 10
    height: 200
    from: 0
    value: 1
    stepSize: 1 / 255
    to: 1.0
    orientation: Qt.Vertical

    handle: Rectangle {
        x: alphaSlider.leftPadding + alphaSlider.availableWidth / 2 - width / 2
        y: alphaSlider.topPadding + alphaSlider.visualPosition * (alphaSlider.availableHeight - height)
        implicitWidth: 12
        implicitHeight: 12
        radius: 6
        color: "#ccc"
    }

    background: Rectangle {
        x: (alphaSlider.width - 8) / 2
        y: alphaSlider.topPadding + alphaSlider.availableHeight / 2 - height / 2
        implicitWidth: 8
        implicitHeight: alphaSlider.availableHeight
        radius: 2
        color: "#bdbebf"

        Chessboard {
            anchors.fill: parent; cellSize: 4
        }
        LinearGradient {
           anchors.fill: parent
           start: Qt.point(0, 0)
           end: Qt.point(0, parent.height)
           gradient: Gradient {
               GradientStop {
                   position: 0.000
                   color: Qt.rgba(1, 1, 1, 1)
               }
               GradientStop {
                   position: 1.000
                   color: Qt.rgba(0, 0, 0, 0)
               }
           }
        }
    }
}




