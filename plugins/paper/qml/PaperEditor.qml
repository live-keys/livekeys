import QtQuick 2.6
import QtQuick.Controls 2.5
import workspace.icons 1.0
import fs 1.0 as Fs
import paper 1.0 as Paper

Rectangle{
    visible: true
    width: 800
    height: 550

    property alias paperCanvas: paperCanvas

    Paper.Toolbox{
        id: toolbox

        anchors.top: parent.top
        anchors.topMargin: 25

        width: 70
        height: parent.height - 25

        paperCanvas: paperCanvas
        propertyBar: propertyPanel
        infoBar: infobar

        Paper.ToolButton{
            id: rectangleToolButton
            tool: Paper.RectangleTool{
                id: rectangleTool
                paperCanvas: paperCanvas
                paperGrapherLoader: paperGraphLoader
                pg: paperGraphLoader.paperGrapher
            }
            content: MenuIcon{}
        }

        Paper.ToolButton{
            id: circleToolButton
            tool: Paper.CircleTool{
                id: circleTool
                paperCanvas: paperCanvas
                paperGrapherLoader: paperGraphLoader
                pg: paperGraphLoader.paperGrapher
            }
            content: MenuIcon{}
        }

        Paper.ToolButton{
            id: selectToolButton
            tool: Paper.SelectTool{
                id: selectTool
                paperCanvas: paperCanvas
                paperGrapherLoader: paperGraphLoader
                pg: paperGraphLoader.paperGrapher
            }
            content: MenuIcon{}
        }
        Paper.ToolButton{
            id: detailSelectToolButton
            tool: Paper.DetailSelectTool{
                id: detailSelectTool
                paperCanvas: paperCanvas
                paperGrapherLoader: paperGraphLoader
                pg: paperGraphLoader.paperGrapher
            }
            content: MenuIcon{}
        }
        Paper.ToolButton{
            id: scaleToolButton
            tool: Paper.ScaleTool{
                id: scaleTool
                paperCanvas: paperCanvas
                paperGrapherLoader: paperGraphLoader
                pg: paperGraphLoader.paperGrapher
            }
            content: MenuIcon{}
        }
        Paper.ToolButton{
            id: rotateToolButton
            tool: Paper.RotateTool{
                id: rotateTool
                paperCanvas: paperCanvas
                paperGrapherLoader: paperGraphLoader
                pg: paperGraphLoader.paperGrapher
            }
            content: MenuIcon{}
        }
        Paper.ToolButton{
            id: drawToolButton
            tool: Paper.DrawTool{
                id: drawTool
                paperCanvas: paperCanvas
                paperGrapherLoader: paperGraphLoader
                pg: paperGraphLoader.paperGrapher
            }
            content: MenuIcon{}
        }
        Paper.ToolButton{
            id: penToolButton
            tool: Paper.PenTool{
                id: penTool
                paperCanvas: paperCanvas
                paperGrapherLoader: paperGraphLoader
                pg: paperGraphLoader.paperGrapher
            }
            content: MenuIcon{}
        }
        Paper.ToolButton{
            id: textToolButton
            tool: Paper.TextTool{
                id: textTool
                paperCanvas: paperCanvas
                paperGrapherLoader: paperGraphLoader
                pg: paperGraphLoader.paperGrapher
            }
            content: MenuIcon{}
        }
        Paper.ToolButton{
            id: eyeDropperButton
            tool: Paper.EyeDropperTool{
                paperCanvas: paperCanvas
                paperGrapherLoader: paperGraphLoader
                pg: paperGraphLoader.paperGrapher
            }
            content: MenuIcon{}
        }
    }

    Rectangle{
        id: topPanel
        anchors.left: parent.left
        anchors.leftMargin: 0
        width: parent.width
        height: 25
        color: lk.layers.workspace.themes.current.colorScheme.middleground
        border.width: 0

        Rectangle{
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 1
            width: 80
            height: parent.height - 2
            color: lk.layers.workspace.themes.current.colorScheme.middlegroundOverlay

            Item{
                id: newButtonWrap
                width: 25
                height: parent.height
                anchors.left: parent.left
                anchors.leftMargin: 5

                Image{
                    id: newIcon
                    width: 10
                    height: 12
                    source: lk.layers.workspace.themes.current.topNewIcon
                    anchors.centerIn: parent
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
                    height: 12
                    source: lk.layers.workspace.themes.current.topOpenFileIcon
                    anchors.centerIn: parent
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
                    source: lk.layers.workspace.themes.current.topSaveIcon
                    anchors.centerIn: parent
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
            color: lk.layers.workspace.themes.current.colorScheme.middlegroundOverlay

            Item{
                id: undoButtonWrap
                width: 25
                height: parent.height
                anchors.left: parent.left
                anchors.leftMargin: 5

                Rectangle{
                    id: undoIcon
                    width: 15
                    height: 15
                    anchors.centerIn: parent
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

                Rectangle{
                    id: redoIcon
                    width: 15
                    height: 15
                    anchors.centerIn: parent
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
        color: lk.layers.workspace.themes.current.colorScheme.middleground
        border.color: lk.layers.workspace.themes.current.colorScheme.middlegroundBorder
        border.width: 1

        Item{
            id: propertyPanel
            height: 300
            width: parent.width
        }

        Rectangle{
            width: parent.width
            color: lk.layers.workspace.themes.current.colorScheme.middlegroundOverlayBorder
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
                color: lk.layers.workspace.themes.current.colorScheme.middlegroundOverlay

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
                    color: modelData.active ? lk.layers.workspace.themes.current.colorScheme.middlegroundOverlay : 'transparent'

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

    Paper.PaperGrapherLoader{
        id: paperGraphLoader
        paperCanvas: paperCanvas

        property var openType: null

        function addFont(fontPath){
            var notosans = Fs.File.open(fontPath, Fs.File.ReadOnly)
            var content = notosans.readAll()
            var font = openType.parse(content)
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
            var openTypeModule = Paper.JsModule.require(lk.layers.workspace.pluginsPath() + '/paper/papergrapherjs/opentype.js', {
                module : { exports: exports },
                exports : exports
            }, false)
            openType = openTypeModule.module.exports

            addFont(lk.layers.workspace.pluginsPath() + '/paper/fonts/NotoSans-Regular.ttf')
            /*addFont(lk.layers.workspace.pluginsPath() + '/paper/fonts/NotoSans-Bold.ttf')
            addFont(lk.layers.workspace.pluginsPath() + '/paper/fonts/NotoSans-BoldItalic.ttf')
            addFont(lk.layers.workspace.pluginsPath() + '/paper/fonts/NotoSans-Italic.ttf')

            addFont(lk.layers.workspace.pluginsPath() + '/paper/fonts/NotoSerif-Regular.ttf')
            addFont(lk.layers.workspace.pluginsPath() + '/paper/fonts/NotoSerif-Bold.ttf')
            addFont(lk.layers.workspace.pluginsPath() + '/paper/fonts/NotoSerif-BoldItalic.ttf')
            addFont(lk.layers.workspace.pluginsPath() + '/paper/fonts/NotoSerif-Italic.ttf')*/

            layerList.pg = pg
        }
    }

    Paper.PaperCanvas{
        id: paperCanvas

        anchors.top: parent.top
        anchors.topMargin: 30

        anchors.left: parent.left
        anchors.leftMargin: 70

        width: parent.width - 270
        height: parent.height - 30


        xmldomPath: lk.layers.workspace.pluginsPath() + '/paper/paperjs/xmldom.js'
        domScriptPath: lk.layers.workspace.pluginsPath() + '/paper/paperjs/paper-dom.js'
        paperScriptPath: lk.layers.workspace.pluginsPath() + '/paper/paperjs/paper-full.js'

        jsModuleLoader: Paper.JsModule

        MouseArea{
            anchors.fill: parent
            hoverEnabled: true

            property var path : null

            onPressed: {
                if ( toolbox.selectedTool ){
                    //if (typeof os_win !== 'undefined') {
                        gc()
                    //}
                    toolbox.selectedTool.mouseDown(mouse)
                }
            }
            onPositionChanged: {
                if ( toolbox.selectedTool ){
                    //if (typeof os_win !== 'undefined') {
                        gc()
                    //}
                    if ( mouse.buttons )
                        toolbox.selectedTool.mouseDrag(mouse)
                    else
                        toolbox.selectedTool.mouseMove(mouse)
                }
            }
            onDoubleClicked: {
                if ( toolbox.selectedTool ){
                    //if (typeof os_win !== 'undefined') {
                        gc()
                    //}
                    toolbox.selectedTool.mouseDoubleClicked(mouse)
                }
            }
            onReleased: {
                if ( toolbox.selectedTool ){
                    //if (typeof os_win !== 'undefined') {
                        gc()
                    //}
                    toolbox.selectedTool.mouseUp(mouse)
                }
            }
        }
    }


}
