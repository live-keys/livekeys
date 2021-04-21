import QtQuick 2.3
import lcvcore 1.0 as Cv
import live 1.0
import visual.shapes 1.0

Rectangle{
    id: root

    property color lineColor: '#141b20'
    property alias histogram: colorHistogram
    property QtObject input: null

    color: 'transparent'
    width: 300
    height: 200
    border.width: 1
    border.color: lineColor

    LineGrid{
        color: parent.lineColor
        anchors.fill: parent
        colSpacing: parent.width / 12
        rowSpacing: parent.height / 8
    }

    Cv.ColorHistogram{
        id: colorHistogram
        anchors.fill: parent
        anchors.margins: 10
        input: root.input
    }
}
