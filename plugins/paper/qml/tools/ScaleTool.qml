import QtQuick 2.3
import QtQuick.Layouts 1.12
import paper 1.0
import "../papergrapherqml/tools/ScaleToolControl.js" as ScaleToolControl

Tool{
    id: root

    toolLabel: 'Scale'

    infoBarContent: Item{
        anchors.fill: parent

        Text{
            id: selectionInfo
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20
            color: "white"
            text: "Scale"
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
                onItemChanged: item.text = "Scale Center"
            }

            Loader{
                id: scaleCenterInput
                height: 25
                width: 150
                sourceComponent: root.paperGrapherLoader.style.dropDownInput
                onItemChanged: {
                    item.valueChanged.connect(function(index, value){
                        root.implementation.options.scaleCenter = value
                    })
                }
            }

            Loader{
                height: 25
                visible: scaleCenterInput.item && scaleCenterInput.item.selectedValue === 'individual'
                sourceComponent: root.paperGrapherLoader.style.label
                onItemChanged: item.text = "Random Scale"
            }

            Loader{
                id: randomScaleInput
                height: 20
                width: 20
                visible: scaleCenterInput.item && scaleCenterInput.item.selectedValue === 'individual'
                sourceComponent: root.paperGrapherLoader.style.boolInput
                onItemChanged: {
                    item.valueChanged.connect(function(value){
                        root.implementation.options.randomScale = value
                    })
                }
            }
        }
    }

    property var pg: null
    property var implementation

    Component.onCompleted: {
        paperCanvas.onPaperReady.connect(function(paper){
            implementation = ScaleToolControl.create(paperCanvas, pg, function(circleInfo){})

            scaleCenterInput.item.setup({ model: ['individual', 'selection', 'cursor'], index: 0 })
            randomScaleInput.item.setup({ value: root.implementation.options.randomScale })

            mouseDown = implementation.onMouseDown
            mouseUp = implementation.onMouseUp
            mouseDrag = implementation.onMouseDrag
            mouseMove = implementation.onMouseMove
        })
    }

}
