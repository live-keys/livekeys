import QtQuick 2.3
import QtQuick.Controls 2.10
import QtGraphicalEffects 1.0
import paper 1.0
import workspace.icons 1.0
import "../papergrapherqml/tools/DetailSelectToolControl.js" as DetailSelectToolControl

Tool{
    id: root

    toolLabel: 'DetailSelect'

    property Component toolProperties: Item {
        width: 200
        height: 300

        Rectangle{
            anchors.centerIn: parent
            width: 50
            height: 30
            color: 'red'
        }
    }

    infoBarContent: Item{
        anchors.fill: parent

        Text{
            id: selectionInfo
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
            color: "white"
            text: "Detail Select"
        }

        Rectangle{
            anchors.left: selectionInfo.right
            anchors.leftMargin: 30
            width: 25
            height: 25
            color: 'transparent'

            MenuIcon{
                id: menuIcon
                anchors.centerIn: parent
                width: 12
                height: 12
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    selectionMenu.popup()
                }
            }
        }

        Menu{
            id: selectionMenu

            Menu {
                title: "Selection"
                Action { text: "Select All"; onTriggered: { pg.selection.selectAllSegments(); paperCanvas.paint() } }
                Action { text: "Deselect All"; onTriggered: { pg.selection.clearSelection(); paperCanvas.paint() } }
                Action { text: "Invert Selection"; onTriggered: { pg.selection.invertSegmentSelection(); paperCanvas.paint() } }
            }

            Menu {
                title: "Segment"
                Action { text: "Switch Handles"; onTriggered: { pg.selection.switchSelectedHandles(); paperCanvas.paint() } }
                Action { text: "Remove Segments"; onTriggered: { pg.selection.removeSelectedSegments(); paperCanvas.paint() } }
                Action { text: "Split Path"; onTriggered: { pg.selection.splitPathAtSelectedSegments(); paperCanvas.paint() } }
            }
        }
    }

    property var implementation: null
    property var pg: null

    Component.onCompleted: {
        paperCanvas.onPaperReady.connect(function(paper){
            implementation = DetailSelectToolControl.create(paperCanvas, pg, function(selectionEvent){})

            mouseDoubleClicked = implementation.onMouseDoubleClicked
            mouseMove = implementation.onMouseMove
            mouseDown = implementation.onMouseDown
            mouseUp = implementation.onMouseUp
            mouseDrag = implementation.onMouseDrag
        })
    }

}
