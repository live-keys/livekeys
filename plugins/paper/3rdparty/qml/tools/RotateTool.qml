import QtQuick 2.3
import QtQuick.Layouts 1.12
import paper 1.0
import "RotateToolControl.js" as RotateToolControl

Tool{
    id: root

    toolLabel: 'Rotate'

    infoBarContent: Item{
        anchors.fill: parent

        Text{
            id: selectionInfo
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20
            color: "white"
            text: "Rotate"
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
                onItemChanged: item.text = "Center"
            }

            Loader{
                id: rotationCenterInput
                height: 25
                width: 150
                sourceComponent: root.paperGrapherLoader.style.dropDownInput
                onItemChanged: {
                    item.valueChanged.connect(function(index, value){
                        root.implementation.options.rotationCenter = value
                    })
                }
            }

            Loader{
                height: 25
                visible: rotationCenterInput.item && rotationCenterInput.item.selectedValue === 'individual'
                sourceComponent: root.paperGrapherLoader.style.label
                onItemChanged: item.text = "Random Speed"
            }

            Loader{
                id: randomSpeedInput
                height: 20
                width: 20
                visible: rotationCenterInput.item && rotationCenterInput.item.selectedValue === 'individual'
                sourceComponent: root.paperGrapherLoader.style.boolInput
                onItemChanged: {
                    item.valueChanged.connect(function(value){
                        root.implementation.options.randomSpeed = value
                    })
                }
            }

            Loader{
                height: 25
                visible: rotationCenterInput.item && rotationCenterInput.item.selectedValue === 'individual'
                sourceComponent: root.paperGrapherLoader.style.label
                onItemChanged: item.text = "Lookat"
            }

            Loader{
                id: lookAtInput
                height: 20
                width: 20
                visible: rotationCenterInput.item && rotationCenterInput.item.selectedValue === 'individual'
                sourceComponent: root.paperGrapherLoader.style.boolInput
                onItemChanged: {
                    item.valueChanged.connect(function(value){
                        root.implementation.options.lookAt = value
                    })
                }
            }
        }
    }


    property var pg: null
    property var implementation

    Component.onCompleted: {
        paperCanvas.onPaperReady.connect(function(paper){
            implementation = RotateToolControl.create(paperCanvas, pg, function(circleInfo){})

            rotationCenterInput.item.setup({ model: ['individual', 'selection', 'cursor'], index: 0 })
            randomSpeedInput.item.setup({ value: root.implementation.options.randomSpeed })
            lookAtInput.item.setup({ value: root.implementation.options.lookAt })

            mouseDown = implementation.onMouseDown
            mouseUp = implementation.onMouseUp
            mouseDrag = implementation.onMouseDrag
            mouseMove = implementation.onMouseMove
        })
    }

}
