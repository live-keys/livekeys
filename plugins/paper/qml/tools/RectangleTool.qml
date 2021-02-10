import QtQuick 2.3
import QtQuick.Layouts 1.12
import paper 1.0
import "../papergrapherqml/tools/RectangleToolControl.js" as RectangleToolControl

Tool{
    id: root

    toolLabel: 'Rectangle'

    infoBarContent: Item{
        anchors.fill: parent

        Text{
            id: selectionInfo
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
            color: "white"
            text: "Rectangle"
        }
    }

    optionsPanelContent: Item{
        anchors.fill: parent

        GridLayout{
            id: layout
            columns: 1
            anchors.margins: 5
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.righ

            Loader{
                height: 25
                sourceComponent: root.paperGrapherLoader.style.label
                onItemChanged: item.text = "Rounded Corners"
            }

            Loader{
                id: roundedCornersInput
                height: 20
                width: 20
                sourceComponent: root.paperGrapherLoader.style.boolInput
                onItemChanged: {
                    item.valueChanged.connect(function(value){
                        root.implementation.options.roundedCorners = value
                    })
                }
            }

            Loader{
                height: 25
                sourceComponent: root.paperGrapherLoader.style.label
                onItemChanged: item.text = "Corner Radius"
            }

            Loader{
                id: cornerRadiusInput
                height: 20
                width: 150
                sourceComponent: root.paperGrapherLoader.style.intInput
                onItemChanged: {
                    item.valueChanged.connect(function(value){
                        root.implementation.options.cornerRadius = value
                    })
                }
            }
        }
    }

    property var implementation: null
    property var pg

    Component.onCompleted: {
        paperCanvas.onPaperReady.connect(function(paper){
            implementation = RectangleToolControl.create(paperCanvas, pg, function(rect){
                selectionInfo.text =
                    'Rectangle ' + Math.floor(rect.x) + ',' + Math.floor(rect.y) + ' ' +
                    Math.floor(rect.width) + 'x' + Math.floor(rect.height)
            })

            roundedCornersInput.item.setup({ value: false })
            cornerRadiusInput.item.setup({ min: 1, value: root.implementation.options.cornerRadius })

            mouseDown = implementation.onMouseDown
            mouseUp = implementation.onMouseUp
            mouseDrag = implementation.onMouseDrag
        })
    }
}
