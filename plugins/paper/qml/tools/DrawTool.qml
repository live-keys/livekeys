import QtQuick 2.0
import paper 1.0
import QtQuick.Layouts 1.12
import "../papergrapherqml/tools/DrawToolControl.js" as DrawToolControl

Tool{
    id: root

    toolLabel: 'Draw'

    infoBarContent: Item{
        anchors.fill: parent

        Text{
            id: selectionInfo
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20
            color: "white"
            text: "Draw"
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
                onItemChanged: item.text = "Point Distance"
            }
            Loader{
                id: pointDistanceInput
                height: 25
                width: 150
                sourceComponent: root.paperGrapherLoader.style.intInput
                onItemChanged: {
                    item.valueChanged.connect(function(value){
                        root.implementation.options.pointDistance = value
                    })
                }
            }

            Loader{
                height: 25
                sourceComponent: root.paperGrapherLoader.style.label
                onItemChanged: item.text = "Parallel Lines"
            }

            Loader{
                id: parallelLinesInput
                height: 20
                width: 20
                sourceComponent: root.paperGrapherLoader.style.boolInput
                onItemChanged: {
                    item.valueChanged.connect(function(value){
                        root.implementation.options.drawParallelLines = value
                    })
                }
            }

            Loader{
                height: 25
                sourceComponent: root.paperGrapherLoader.style.label
                onItemChanged: item.text = "Parallel Line Count"
            }

            Loader{
                id: linesInput
                height: 25
                width: 150
                sourceComponent: root.paperGrapherLoader.style.intInput
                onItemChanged: {
                    item.valueChanged.connect(function(value){
                        root.implementation.options.lines = value
                    })
                }
            }

            Loader{
                height: 25
                sourceComponent: root.paperGrapherLoader.style.label
                onItemChanged: item.text = "Line distance"
            }

            Loader{
                id: lineDistanceInput
                height: 25
                width: 150
                sourceComponent: root.paperGrapherLoader.style.numberInput
                onItemChanged: {
                    item.valueChanged.connect(function(value){
                        root.implementation.options.lineDistance = value
                    })
                }
            }
        }
    }

    //TODO

    property var components: { return {
        lineDistance: {
            type: 'float',
            label: 'Line distance',
            requirements : {drawParallelLines: true},
            min: 0
        },
        closePath: {
            type: 'list',
            label: 'Close path',
            options: [ 'near start', 'always', 'never' ]
        },
        smoothPath: {
            type: 'boolean',
            label: 'Smooth path'
        }
    } }


    property var pg: null
    property var implementation

    Component.onCompleted: {
        paperCanvas.onPaperReady.connect(function(paper){
            implementation = DrawToolControl.create(paperCanvas, root.pg, function(drawInfo){})

            mouseDown = implementation.onMouseDown
            mouseUp = implementation.onMouseUp
            mouseDrag = implementation.onMouseDrag

            pointDistanceInput.item.setup({ min: 1, value: root.implementation.options.pointDistance })
            parallelLinesInput.item.setup({ value: false })
            linesInput.item.setup({ value: root.implementation.options.parallelLines })
            lineDistanceInput.item.setup({ value: root.implementation.options.lineDistance, min: 0 })
        })
    }

}
