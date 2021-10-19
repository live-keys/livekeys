import QtQuick 2.3
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.0

import QtQuick.Controls 2.1

import paper 1.0

Rectangle{
    id: root
    width: 70
    height: 300
    color: style.box.background

    default property alias contents: layout.children

    property QtObject toolStyle : QtObject{
        property color foreground: 'black'
        property color background: 'white'
        property double opacity: 1
        property string blendMode : 'normal'
        property double strokeSize: 1
        property double opacityToBackground: 1

        function setup(opt){
            if ( opt.strokeSize ){
                strokeSizeInput.item.setup({ value: opt.strokeSize })
                root.toolStyle.strokeSize = opt.strokeSize
            }
        }
    }

    signal pickColor(var toolbar, string type)

    property QtObject style : PaperGrapherConfig{}

    function activateTool(tool){
        if ( root.selectedTool ){
            if ( root.selectedTool.infoBarContent ){
                root.selectedTool.infoBarContent.parent = null
            }
            if ( root.selectedTool.optionsPanelContent ){
                root.selectedTool.optionsPanelContent.parent = null
            }
            root.selectedTool.deactivate()
        }

        root.selectedTool = tool

        if ( tool.infoBarContent ){
            tool.infoBarContent.parent = root.infoBar
        }
        if ( tool.optionsPanelContent ){
            tool.optionsPanelContent.parent = root.propertyBar
        }
    }


    property Tool selectedTool: null
    property QtObject paperCanvas: null
    property QtObject infoBar: null
    property QtObject propertyBar: null

    GridLayout{
        id: layout
        anchors.margins: 0
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        columns: 2
        columnSpacing: 0

        function activateTool(tool){
            root.activateTool(tool)
        }
    }

    Item{
        anchors.top: layout.bottom
        anchors.topMargin: 30
        width: 50
        height: 50


        Chessboard{
            color: 'white'
            width: 25
            height: 25
            cellSize: 25 / 6
            x: 30
            y: 20

            Rectangle{
                width: 25
                height: 25
                border.width: 3
                border.color: root.toolStyle.background
                color: "transparent"

                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        borderColorLoader.item.setup({ color: root.toolStyle.background })
                    }
                }

                Loader{
                    id: borderColorLoader
                    anchors.fill: parent
                    sourceComponent: root.style.colorInput
                    onItemChanged: {
                        item.onValueChanged.connect(function(value){
                            root.toolStyle.background = value
                        })
                    }
                }
            }

            Rectangle{
                x: 3
                y: 3
                color: style.box.background
                width: 19
                height: 19
            }

        }

        Chessboard{
            x: 15
            color: 'white'
            width: 25
            height: 25
            cellSize: 25 / 6

            Rectangle{
                border.width: 1
                border.color: 'black'
                width: 25
                height: 25

                color: root.toolStyle.foreground

                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        foregroundColorLoader.item.setup({color: root.toolStyle.foreground })
                    }
                }

                Loader{
                    id: foregroundColorLoader
                    anchors.fill: parent
                    sourceComponent: root.style.colorInput
                    onItemChanged: {
                        item.onValueChanged.connect(function(value){
                            root.toolStyle.foreground = value
                        })
                    }
                }
            }
        }
    }

//    Item{
//        anchors.top: layout.bottom
//        anchors.topMargin: 100
//        width: 50
//        height: 50

//        Loader{
//            sourceComponent: root.style.label
//            anchors.horizontalCenter: parent.horizontalCenter
//            onItemChanged: {
//                item.text = 'Opacity'
//            }
//        }

//        Loader{
//            anchors.top: parent.top
//            anchors.topMargin: 18
//            width: parent.width - 10
//            height: parent.height
//            anchors.horizontalCenter: parent.horizontalCenter
//            sourceComponent: root.style.intDropDown
//            onItemChanged: {
//                item.onValueChanged.connect(function(value){
//                    root.toolStyle.opacity = value / 100
//                })
//            }
//        }
//    }

//    Item{
//        anchors.top: layout.bottom
//        anchors.topMargin: 150
//        width: 70
//        height: 50

//        Loader{
//            sourceComponent: root.style.label
//            anchors.horizontalCenter: parent.horizontalCenter
//            onItemChanged: {
//                item.text = 'Blending'
//            }
//        }

//        Loader{
//            anchors.top: parent.top
//            anchors.topMargin: 18
//            width: parent.width - 4
//            height: 20
//            anchors.horizontalCenter: parent.horizontalCenter
//            sourceComponent: root.style.dropDownInput
//            onItemChanged: {
//                item.setup({ model: [
//                   'normal',
//                   'multiply',
//                   'screen',
//                   'overlay',
//                   'soft-light',
//                   'hard-light',
//                   'color-dodge',
//                   'color-burn',
//                   'darken',
//                   'lighten',
//                   'differende',
//                   'exclusion',
//                   'hue',
//                   'saturation',
//                   'luminosity',
//                   'color',
//                   'add',
//                   'subtract',
//                   'average',
//                   'pin-light',
//                   'negation',
//                   'source-over',
//                   'source-in',
//                   'source-atop',
//                   'destination-over',
//                   'destination-in',
//                   'destination-out',
//                   'destination-atop',
//                   'lighter',
//                   'darker',
//                   'copy',
//                   'xor'
//                ], index: 0 })

//                item.onValueChanged.connect(function(index, value){
//                    root.toolStyle.blendMode = value
//                })
//            }
//        }
//    }

    Item{
        anchors.top: layout.bottom
        anchors.topMargin: 150
        width: parent.width
        height: 50

        Loader{
            sourceComponent: root.style.label
            anchors.horizontalCenter: parent.horizontalCenter
            onItemChanged: {
                item.text = 'Background'
            }
        }

        Loader{
            anchors.top: parent.top
            anchors.topMargin: 18
            width: parent.width - 10
            height: 20
            anchors.horizontalCenter: parent.horizontalCenter
            sourceComponent: root.style.intDropDown
            onItemChanged: {
                item.setup( { index: 10 })
                item.onValueChanged.connect(function(index, value){
                    root.toolStyle.opacityToBackground = value / 100
                })
            }
        }
    }

    Item{
        anchors.top: layout.bottom
        anchors.topMargin: 100
        width: 70
        height: 50

        Loader{
            sourceComponent: root.style.label
            anchors.horizontalCenter: parent.horizontalCenter
            onItemChanged: {
                item.text = 'Stroke'
            }
        }

        Loader{
            id: strokeSizeInput
            anchors.top: parent.top
            anchors.topMargin: 18
            width: parent.width - 10
            height: 20
            anchors.horizontalCenter: parent.horizontalCenter
            sourceComponent: root.style.intInput
            onItemChanged: {
                item.onValueChanged.connect(function(value){
                    root.toolStyle.strokeSize = value
                })
            }
        }
    }

}
