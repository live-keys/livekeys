import QtQuick 2.3
import QtQuick.Controls 2.10
import QtGraphicalEffects 1.0
import paper 1.0
import workspace.icons 1.0
import '../papergrapherqml/tools/SelectToolControl.js' as SelectToolControl

Tool{
    id: root

    toolLabel: 'Select'

    infoBarContent: Item{
        anchors.fill: parent

        Text{
            id: selectionInfo
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
            color: "white"
            text: "Select"
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
                title: "Edit"
                Action { text: "Copy"; onTriggered: { root.pg.edit.copySelectionToClipboard(); paperCanvas.paint() } }
                Action { text: "Paste"; onTriggered: { root.pg.edit.pasteObjectsFromClipboard(); paperCanvas.paint() } }
                Action { text: "Delete"; onTriggered: { root.pg.selection.deleteSelection(); paperCanvas.paint() } }
            }

            Menu {
                title: "Select"
                Action { text: "Select All"; onTriggered: { root.pg.selection.selectAllItems(); paperCanvas.paint() } }
                Action { text: "Deselect All"; onTriggered: { root.pg.selection.clearSelection(); paperCanvas.paint() } }
                Action { text: "Invert Selection"; onTriggered: { root.pg.selection.invertItemSelection(); paperCanvas.paint() } }
                Action { text: "Random Selection"; onTriggered: { root.pg.selection.selectRandomItems(); paperCanvas.paint() } }
            }

            Menu{
                title: "Group"
                Action { text: "Group"; onTriggered: { root.pg.group.groupSelection(); paperCanvas.paint() } }
                Action { text: "Ungroup"; onTriggered: { root.pg.group.ungroupSelection(); paperCanvas.paint() } }
            }

            Menu{
                title: "Layer"
                Action { text: "Move to Active"; onTriggered: { root.pg.layer.addSelectedItemsToActiveLayer(); paperCanvas.paint() } }
            }

            Menu{
                title: "Order"
                Action { text: "Bring to front"; onTriggered: { root.pg.order.bringSelectionToFront(); paperCanvas.paint() } }
                Action { text: "Send to back"; onTriggered: { root.pg.order.sendSelectionToBack(); paperCanvas.paint() } }
            }

            Menu{
                title: "Compound Path"
                Action { text: "Create Compound Path"; onTriggered: { root.pg.compoundPath.createFromSelection(); paperCanvas.paint() } }
                Action { text: "Release Compound Path"; onTriggered: { root.pg.compoundPath.releaseSelection(); paperCanvas.paint() } }
            }

            Menu{
                title: "Operations"
                Action { text: "Unite"; onTriggered: { root.pg.boolean.booleanUnite(); paperCanvas.paint() } }
                Action { text: "Intersect"; onTriggered: { root.pg.boolean.booleanIntersect(); paperCanvas.paint() } }

                Action { text: "Subtract"; onTriggered: { root.pg.boolean.booleanSubtract(); paperCanvas.paint() } }
                Action { text: "Exclude"; onTriggered: { root.pg.boolean.booleanExclude(); paperCanvas.paint() } }
                Action { text: "Divide"; onTriggered: { root.pg.boolean.booleanDivide(); paperCanvas.paint() } }
            }

            Menu{
                title: "Text"
                Action{ text: 'Text to outlines'; onTriggered: { root.pg.text.convertSelectionToOutlines(); paperCanvas.paint() } }
            }
        }
    }

    property var pg: null
    property var implementation

    Component.onCompleted: {
        paperCanvas.onPaperReady.connect(function(paper){
            implementation = SelectToolControl.create(paperCanvas, pg, function(selectInfo){})

            mouseDown = implementation.onMouseDown
            mouseUp = implementation.onMouseUp
            mouseMove = implementation.onMouseMove
            mouseDrag = implementation.onMouseDrag
            deactivate = implementation.deactivate
        })
    }


}
