import QtQuick 2.0
import paper 1.0
import "PenToolControl.js" as PenToolControl

Tool{
    id: root

    toolLabel: 'Pen'

    infoBarContent: Item{
        anchors.fill: parent

        Text{
            id: selectionInfo
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
            color: "white"
            text: "Pen"
        }
    }

    property var pg: null
    property var implementation: null

    Component.onCompleted: {
        paperCanvas.onPaperReady.connect(function(paper){
            implementation = PenToolControl.create(paperCanvas, pg, function(rect){})

            mouseMove = implementation.onMouseMove
            mouseDown = implementation.onMouseDown
            mouseUp = implementation.onMouseUp
            mouseDrag = implementation.onMouseDrag
        })
    }

}
