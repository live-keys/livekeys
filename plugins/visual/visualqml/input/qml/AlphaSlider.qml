import QtQuick 2.3
import QtQuick.Controls 1.6
import QtQuick.Controls.Styles 1.3
import QtGraphicalEffects 1.0
import visual.shapes 1.0

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
        groove: Item{
            implicitHeight: 8
            Chessboard {
                anchors.fill: parent; cellSize: 4
            }
            LinearGradient {
               anchors.fill: parent
               start: Qt.point(parent.width, 0)
               end: Qt.point(0, 0)
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
        handle: Rectangle{
            width: 11
            height: 11
            radius: 5
            color: '#ccc'
        }
    }
}
