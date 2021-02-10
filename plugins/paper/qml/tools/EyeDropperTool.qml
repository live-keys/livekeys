import QtQuick 2.3
import QtQuick.Layouts 1.12
import paper 1.0
import "../papergrapherqml/tools/EyeDropperToolControl.js" as EyeDropperToolControl

Tool{
    id: root

    toolLabel: 'EyeDropper'

    infoBarContent: Item{
        anchors.fill: parent

        Text{
            id: selectionInfo
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
            color: "white"
            text: "EyeDropper"
        }
    }

    optionsPanelContent: Item{
        anchors.fill: parent
    }

    property var implementation: null
    property var pg

    Component.onCompleted: {
        paperCanvas.onPaperReady.connect(function(paper){
            implementation = EyeDropperToolControl.create(paperCanvas, pg)

            mouseDown = implementation.onMouseDown
            mouseMove = implementation.onMouseMove
        })

    }
}
