import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.12
import QtQuick.Controls.Styles 1.4
import QtQuick.Shapes 1.7
import QtGraphicalEffects 1.12

import paper 1.0 as Paper

Window {
    visible: true
    width: 1100
    height: 550
    maximumWidth: 1100
    maximumHeight: 550
    title: qsTr("Paperjs Sample")
    color: 'white'

    // Dialogs

    MessageDialog {
        id: messageDialog
        title: ""
        text: ""
        standardButtons: StandardButton.Ok | StandardButton.Cancel

        property var callback: function(){}

        function confirm(opt){
            if ( opt.title )
                title = opt.title
            if ( opt.text )
                text = opt.text
            if ( opt.accepted )
                callback = opt.accepted
            visible = true
        }

        onAccepted: {
            title = ''
            text = ''
            if ( callback )
                callback()
            callback = null
        }
        onRejected: {
            title = ''
            text = ''
            callback = null
        }
    }

    FileDialog{
        id: fileDialog
        title: ""
        nameFilters: ["All files (*)"]

        property var acceptedCallback: null
        property var rejectedCallback: null

        function setup(opt){
            if ( opt.hasOwnProperty('title') )
                title = opt.title
            if ( opt.hasOwnProperty('filters') )
                nameFilters = opt.filters
            if ( opt.hasOwnProperty('existing') )
                selectExisting = opt.existing
            if ( opt.hasOwnProperty('accepted') )
                acceptedCallback = opt.accepted
            if ( opt.hasOwnProperty('rejected') )
                rejectedCallback = opt.rejected
            visible = true
        }

        onAccepted: {
            title = ''
            nameFilters = ["All files (*)"]
            if ( acceptedCallback )
                acceptedCallback(fileUrl)
            acceptedCallback = null
            rejectedCallback = null
        }
        onRejected: {
            title = ''
            nameFilters = ["All files (*)"]
            if ( rejectedCallback )
                rejectedCallback()
            rejectedCallback = null
            acceptedCallback = null
        }
    }

    Toolbox{
        id: toolbox

        anchors.top: parent.top
        anchors.topMargin: 25

        width: 70
        height: parent.height - 25

        paperCanvas: paperCanvas
        propertyBar: propertyPanel
        infoBar: infobar

        ToolButton{
            id: rectangleToolButton
            tool: RectangleTool{
                id: rectangleTool
                paperCanvas: paperCanvas
                paperGrapherLoader: paperGraphLoader
                pg: paperGraphLoader.paperGrapher
            }
            content: ToolIcon{
                hover: rectangleToolButton.containsMouse
                iconSource: "qrc:/square.svg"
            }
        }

        ToolButton{
            id: circleToolButton
            tool: CircleTool{
                id: circleTool
                paperCanvas: paperCanvas
                paperGrapherLoader: paperGraphLoader
                pg: paperGraphLoader.paperGrapher
            }
            content: ToolIcon{
                hover: circleToolButton.containsMouse
                iconSource: "qrc:/circle.svg"
            }
        }

        ToolButton{
            id: selectToolButton
            tool: SelectTool{
                id: selectTool
                paperCanvas: paperCanvas
                paperGrapherLoader: paperGraphLoader
                pg: paperGraphLoader.paperGrapher
            }
            content: ToolIcon{
                hover: selectToolButton.containsMouse
                iconSource: "qrc:/mouse-pointer.svg"
            }
        }
        ToolButton{
            id: detailSelectToolButton
            tool: DetailSelectTool{
                id: detailSelectTool
                paperCanvas: paperCanvas
                paperGrapherLoader: paperGraphLoader
                pg: paperGraphLoader.paperGrapher
            }
            content: ToolIcon{
                hover: detailSelectToolButton.containsMouse
                iconSource: "qrc:/mouse-pointer-fill.svg"
            }
        }
        ToolButton{
            id: scaleToolButton
            tool: ScaleTool{
                id: scaleTool
                paperCanvas: paperCanvas
                paperGrapherLoader: paperGraphLoader
                pg: paperGraphLoader.paperGrapher
            }
            content: ToolIcon{
                hover: scaleToolButton.containsMouse
                iconSource: "qrc:/resize.svg"
            }
        }
        ToolButton{
            id: rotateToolButton
            tool: RotateTool{
                id: rotateTool
                paperCanvas: paperCanvas
                paperGrapherLoader: paperGraphLoader
                pg: paperGraphLoader.paperGrapher
            }
            content: ToolIcon{
                hover: rotateToolButton.containsMouse
                iconSource: "qrc:/rotation.svg"
            }
        }
        ToolButton{
            id: drawToolButton
            tool: DrawTool{
                id: drawTool
                paperCanvas: paperCanvas
                paperGrapherLoader: paperGraphLoader
                pg: paperGraphLoader.paperGrapher
            }
            content: ToolIcon{
                hover: drawToolButton.containsMouse
                iconSource: "qrc:/pencil.svg"
            }
        }
        ToolButton{
            id: penToolButton
            tool: PenTool{
                id: penTool
                paperCanvas: paperCanvas
                paperGrapherLoader: paperGraphLoader
                pg: paperGraphLoader.paperGrapher
            }
            content: ToolIcon{
                hover: penToolButton.containsMouse
                iconSource: "qrc:/pen.svg"
            }
        }
        ToolButton{
            id: textToolButton
            tool: TextTool{
                id: textTool
                paperCanvas: paperCanvas
                paperGrapherLoader: paperGraphLoader
                pg: paperGraphLoader.paperGrapher
            }
            content: ToolIcon{
                hover: textToolButton.containsMouse
                iconSource: "qrc:/type.svg"
            }
        }
        ToolButton{
            id: eyeDropperButton
            tool: EyeDropperTool{
                paperCanvas: paperCanvas
                paperGrapherLoader: paperGraphLoader
                pg: paperGraphLoader.paperGrapher
            }
            content: ToolIcon{
                hover: eyeDropperButton.containsMouse
                iconSource: "qrc:/eye-dropper.svg"
            }
        }
    }

    Rectangle{
        id: topPanel
        anchors.left: parent.left
        anchors.leftMargin: 0
        width: parent.width
        height: 25
        color: "#333"
        border.width: 0

        Rectangle{
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 1
            width: 80
            height: parent.height - 2
            color: "#444"

            Item{
                id: newButtonWrap
                width: 25
                height: parent.height
                anchors.left: parent.left
                anchors.leftMargin: 5

                Image{
                    id: newIcon
                    width: 15
                    height: 15
                    anchors.centerIn: parent
                    source: "qrc:/new.svg"
                }
                ColorOverlay{
                    anchors.fill: newIcon
                    source: newIcon
                    color: "white"
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        var pg = paperGraphLoader.paperGrapher
                        messageDialog.confirm({
                            title: 'New document',
                            text: 'Are you sure you want to clear the current contents of the document?',
                            accepted: function(){
                                pg.document.clear()
                                paperCanvas.paint()
                            }
                        })
                    }
                }
            }

            Item{
                id: openButtonWrap
                width: 25
                height: parent.height
                anchors.left: parent.left
                anchors.leftMargin: 30

                Image{
                    id: openIcon
                    width: 15
                    height: 15
                    anchors.centerIn: parent
                    source: "qrc:/open.svg"
                }
                ColorOverlay{
                    anchors.fill: openIcon
                    source: openIcon
                    color: "white"
                }

                Menu {
                    id: openOptions
                    title: "Open Options"
                    Action {
                        text: "Open Project";
                        onTriggered: {
                            var pg = paperGraphLoader.paperGrapher
                            var pcanvas = paperCanvas
                            fileDialog.setup({
                                 name: "Open project",
                                 filters: ["Json files (*.json)"],
                                 existing: true,
                                 accepted: function(url){
                                     var jsonString = Paper.FileIO.readUrl(url)
                                     pcanvas.paper.project.clear();
                                     pcanvas.paper.project.importJSON(jsonString.toString());
                                     paperCanvas.paint()
                                 }
                            })

                        }
                    }
//                    Action {
//                        text: "Import Svg";
//                        onTriggered: {
//                            var pg = paperGraphLoader.paperGrapher
//                            var pcanvas = paperCanvas
//                            fileDialog.setup({
//                                 name: "Import svg",
//                                 filters: ["Svg files (*.svg)"],
//                                 existing: true,
//                                 accepted: function(url){
//                                     var svgString = Paper.FileIO.readUrl(url)
//                                     var DOMParser = paperCanvas.dom.DOMParser
//                                     var domObject = new DOMParser()
//                                     var dpo = domObject.parseFromString(svgString.toString().trim(), 'image/svg+xml')
//                                     pcanvas.paper.project.importSVG(dpo, {expandShapes: true})
//                                     paperCanvas.paint()
//                                 }
//                            })
//                        }
//                    }
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        openOptions.popup()
                    }
                }


            }

            Item{
                id: saveButtonWrap
                width: 25
                height: parent.height
                anchors.left: parent.left
                anchors.leftMargin: 55

                Image{
                    id: saveIcon
                    width: 15
                    height: 15
                    anchors.centerIn: parent
                    source: "qrc:/save.svg"
                }
                ColorOverlay{
                    anchors.fill: saveIcon
                    source: saveIcon
                    color: "white"
                }

                Menu {
                    id: saveOptions
                    title: "Save Options"
                    Action {
                        text: "Save Project As...";
                        onTriggered: {
                            var pg = paperGraphLoader.paperGrapher
                            var pcanvas = paperCanvas
                            fileDialog.setup({
                                 name: "Save Project",
                                 filters: ["Json files (*.json)"],
                                 existing: false,
                                 accepted: function(url){
                                     var exportData = pcanvas.paper.project.exportJSON({ asString: true });
                                     Paper.FileIO.writeUrl(url, exportData)
                                 }
                            })
                            paperCanvas.paint()
                        }
                    }
//                    Action {
//                        text: "Export Svg...";
//                        onTriggered: {
//                            var pg = paperGraphLoader.paperGrapher
//                            var pcanvas = paperCanvas
//                            fileDialog.setup({
//                                 name: "Export svg",
//                                 filters: ["Svg files (*.svg)"],
//                                 existing: false,
//                                 accepted: function(url){
////                                     var exportData = pcanvas.paper.project.exportSVG({ asString: false, bounds: null });
//                                 }
//                            })
//                        }
//                    }
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        saveOptions.popup()
                    }
                }
            }

        }


        Rectangle{
            anchors.left: parent.left
            anchors.leftMargin: 90
            anchors.top: parent.top
            anchors.topMargin: 1
            width: 55
            height: parent.height - 2
            color: "#444"



            Item{
                id: undoButtonWrap
                width: 25
                height: parent.height
                anchors.left: parent.left
                anchors.leftMargin: 5

                Image{
                    id: undoIcon
                    width: 15
                    height: 15
                    anchors.centerIn: parent
                    source: "qrc:/arrow-back.svg"
                }
                ColorOverlay{
                    anchors.fill: undoIcon
                    source: undoIcon
                    color: "white"
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        var pg = paperGraphLoader.paperGrapher
                        pg.undo.undo()
                        paperCanvas.paint()
                    }
                }
            }
            Item{
                id: redoButtonWrap
                width: 25
                height: parent.height
                anchors.left: parent.left
                anchors.leftMargin: 30

                Image{
                    id: redoIcon
                    width: 15
                    height: 15
                    anchors.centerIn: parent
                    source: "qrc:/arrow-forward.svg"
                }
                ColorOverlay{
                    anchors.fill: redoIcon
                    source: redoIcon
                    color: "white"
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        var pg = paperGraphLoader.paperGrapher
                        pg.undo.redo()
                        paperCanvas.paint()
                    }
                }
            }

        }

        Item{
            id: infobar
            anchors.left: parent.left
            anchors.leftMargin: 140
            width: parent.width - anchors.leftMargin
            height: 25
        }
    }

    Rectangle{

        anchors.top: parent.top
        anchors.topMargin: 25

        anchors.right: parent.right
        anchors.rightMargin: 0
        width: 200
        height: parent.height - 25
        color: "#333"
        border.color: "#555"
        border.width: 1

        Item{
            id: propertyPanel
            height: 300
            width: parent.width
        }

        Rectangle{
            width: parent.width
            color: "#666"
            anchors.top: propertyPanel.bottom
            height: 1
        }

        Item{
            id: layerPanel
            height: parent.height - 300
            width: parent.width
            anchors.top: propertyPanel.bottom
            anchors.topMargin: 1

            Rectangle{
                width: parent.width
                height: 25
                color: "#444"

                Loader{
                    anchors.top: parent.top
                    anchors.topMargin: 5
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    height: 25
                    sourceComponent: paperGraphLoader.style.label
                    onItemChanged: item.text = "Layers"
                }

                Loader{
                    anchors.top: parent.top
                    anchors.topMargin: 5
                    anchors.right: parent.right
                    anchors.rightMargin: 15
                    height: 25
                    sourceComponent: paperGraphLoader.style.label
                    onItemChanged: item.text = "+"
                    MouseArea{
                        id: addLayer
                        anchors.fill: parent
                        onClicked: layerList.addLayer()
                    }
                }
            }

            ListView{
                id: layerList
                anchors.top: parent.top
                anchors.topMargin: 25
                anchors.fill: parent

                property var pg: null
                onPgChanged: {
                    updateLayers()
                }

                function updateLayers(){
                    if ( !pg ){
                        model = []
                        return
                    }
                    var layers = pg.layer.getAllUserLayers()
                    var layerModel = []
                    for ( var i = layers.length - 1; i >= 0; --i ){
                        var layer = layers[i]
                        var layerItem = {
                            name: layer.name,
                            id: layer.data.id,
                            isDefault: layer.data.isDefaultLayer,
                            visible: layer.visible,
                            active: pg.layer.isActiveLayer(layer)
                        }
                        layerModel.push(layerItem)
                    }

                    layerList.model = layerModel
                }

                function addLayer(){
                    var newLayer = pg.layer.addNewLayer('Layer ' + model.length )
                    newLayer.activate();
                    updateLayers()
                }

                delegate: Rectangle{
                    width: layerList.width
                    height: 25
                    color: modelData.active ? '#4a4a4a' : 'transparent'

                    MouseArea{
                        id: activateArea
                        anchors.fill: parent
                        onClicked : {
                            var layer = layerList.pg.layer.getLayerByID(modelData.id)
                            layerList.pg.layer.setActiveLayer(layer);
                            layerList.updateLayers()
                        }
                    }

                    Loader{
                        id: layerVisibleInput
                        height: 15
                        width: 15
                        sourceComponent: paperGraphLoader.style.boolInput
                        onItemChanged: {
                            item.setup({ value: modelData.visible })
                            item.valueChanged.connect(function(value){
                                var layer = layerList.pg.layer.getLayerByID(modelData.id)
                                layer.visible = value
                                paperCanvas.paint()
                            })
                        }
                    }

                    Loader{
                        anchors.top: parent.top
                        anchors.topMargin: 5
                        anchors.left: parent.left
                        anchors.leftMargin: 30
                        height: 25
                        sourceComponent: paperGraphLoader.style.label
                        onItemChanged: item.text = modelData.name
                    }

                    Rectangle{
                        anchors.right: parent.right
                        anchors.rightMargin: 25
                        anchors.top: parent.top
                        anchors.topMargin: 5
                        radius: 5
                        width: 16
                        height: 16
                        color: "#777"
                        visible: !modelData.isDefault
                        Loader{
                            anchors.centerIn: parent
                            sourceComponent: paperGraphLoader.style.label
                            onItemChanged: item.text = 'x'
                        }
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                var layerView = layerList
                                var layerId = modelData.id
                                messageDialog.confirm({
                                    title: 'Delete Layer',
                                    text: 'Are you sure you want to delete the current layer?',
                                    accepted: function(){
                                        layerView.pg.layer.deleteLayer(layerId)
                                        layerView.updateLayers()
                                    }
                                })
                            }
                        }
                    }
                    Rectangle{
                        anchors.right: parent.right
                        anchors.rightMargin: 5
                        anchors.top: parent.top
                        anchors.topMargin: 5
                        radius: 8
                        width: 16
                        height: 16
                        color: "#777"
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                layerList.pg.selection.clearSelection();
                                var items = layerList.pg.helper.getPaperItemsByLayerID(modelData.id);
                                for ( var i = 0; i < items.length; ++i ){
                                    layerList.pg.selection.setItemSelection(items[i], true);
                                }
                                paperCanvas.paint()
                            }
                        }
                    }
                }
            }
        }
    }

    PaperGrapherLoader{
        id: paperGraphLoader
        paperCanvas: paperCanvas

        property var openType: null

        function addFont(fontPath){
            var notosans = Paper.FileIO.read(fontPath)
            var font = openType.parse(notosans)
            var pg = paperGraphLoader.paperGrapher

            var info = pg.text.getShortInfoFromFont(font);
            pg.text.addImportedFont(info.fontFamily, info.fontStyle, font);
            return font
        }

        onPaperGrapherReady: {
            pg.styleBar = toolbox.toolStyle
            toolbox.toolStyle.onForegroundChanged.connect(function(){
                pg.styles.update({ foreground: toolbox.toolStyle.foreground })
                paperCanvas.paint()
            })
            toolbox.toolStyle.onBackgroundChanged.connect(function(){
                pg.styles.update({ background: toolbox.toolStyle.background })
                paperCanvas.paint()
            })
            toolbox.toolStyle.onOpacityChanged.connect(function(){
                pg.styles.update({ opacity: toolbox.toolStyle.opacity })
                paperCanvas.paint()
            })
            toolbox.toolStyle.onBlendModeChanged.connect(function(){
                pg.styles.update({ blendMode: toolbox.toolStyle.blendMode })
                paperCanvas.paint()
            })
            toolbox.toolStyle.onStrokeSizeChanged.connect(function(){
                pg.styles.update({ strokeSize: toolbox.toolStyle.strokeSize })
                paperCanvas.paint()
            })

            var exports = {}
            var openTypeModule = Paper.JsModule.require(':/opentype.js', {
                module : { exports: exports },
                exports : exports
            }, false)
            openType = openTypeModule.module.exports

            addFont(':/NotoSans-Regular.ttf')
            addFont(':/NotoSans-Bold.ttf')
            addFont(':/NotoSans-BoldItalic.ttf')
            addFont(':/NotoSans-Italic.ttf')

            addFont(':/NotoSerif-Regular.ttf')
            addFont(':/NotoSerif-Bold.ttf')
            addFont(':/NotoSerif-BoldItalic.ttf')
            addFont(':/NotoSerif-Italic.ttf')

            layerList.pg = pg
        }
    }

    PaperCanvas{
        id: paperCanvas

        anchors.top: parent.top
        anchors.topMargin: 30

        anchors.left: parent.left
        anchors.leftMargin: 70

        width: parent.width - 270
        height: parent.height - 30

        jsModuleLoader: Paper.JsModule

        MouseArea{
            anchors.fill: parent
            hoverEnabled: true

            property var path : null

            onPressed: {
                if ( toolbox.selectedTool ){
                    if (typeof os_win !== 'undefined') {
                        gc()
                    }
                    toolbox.selectedTool.mouseDown(mouse)
                }
            }
            onPositionChanged: {
                if ( toolbox.selectedTool ){
                    if (typeof os_win !== 'undefined') {
                        gc()
                    }
                    if ( mouse.buttons )
                        toolbox.selectedTool.mouseDrag(mouse)
                    else
                        toolbox.selectedTool.mouseMove(mouse)
                }
            }
            onDoubleClicked: {
                if ( toolbox.selectedTool ){
                    if (typeof os_win !== 'undefined') {
                        gc()
                    }
                    toolbox.selectedTool.mouseDoubleClicked(mouse)
                }
            }
            onReleased: {
                if ( toolbox.selectedTool ){
                    if (typeof os_win !== 'undefined') {
                        gc()
                    }
                    toolbox.selectedTool.mouseUp(mouse)
                }
            }
        }
    }



}
