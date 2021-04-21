import QtQuick 2.3
import paper 1.0
import "CircleToolControl.js" as CircleToolControl

Tool{
    id: root

    toolLabel: 'Circle'

    infoBarContent: Item{
        anchors.fill: parent

        Text{
            id: selectionInfo
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
            color: "white"
            text: "Circle"
        }
    }

    property var pg: null
    property var implementation

    Component.onCompleted: {
        paperCanvas.onPaperReady.connect(function(paper){
            implementation = CircleToolControl.create(paperCanvas, pg, function(circleInfo){})

            mouseDown = implementation.onMouseDown
            mouseUp = implementation.onMouseUp
            mouseDrag = implementation.onMouseDrag
        })
    }

}
