import QtQuick 2.3
import editor 1.0
import editor.private 1.0
import base 1.0
import live 1.0
import fs 1.0 as Fs
import lcvcore 1.0 as Cv

WorkspaceExtension{
    id: root

    objectName : "editcv"
    commands: { return {} }

    interceptFile: function(path, mode){
        if ( Fs.Path.hasExtensions(path, 'jpg')){
            var document = project.openFile(path, mode)
            if ( !document )
                return

            var fe = lk.layers.workspace.panes.focusPane('matImage')
            if ( !fe ){
                fe = lk.layers.workspace.panes.createPane('matImage', {}, [400, 0])

                var editor = lk.layers.workspace.panes.focusPane('editor')
                if ( editor ){
                    lk.layers.workspace.panes.splitPaneHorizontallyWith(
                        editor.parentSplitView, editor.parentSplitViewIndex(), fe
                    )
                } else {
                    lk.layers.workspace.panes.container.splitPane(0, fe)
                }

                var containerPanes = lk.layers.workspace.panes.container.panes
                if ( containerPanes.length > 2 && containerPanes[2].width > 500 + containerPanes[0].width){
                    containerPanes[0].width = containerPanes[0].width * 2
                    fe.width = 400
                }
            }

            var mat = Cv.MatIO.decode(document.content)
            fe.image = mat
            fe.title = path
            return fe
        }
    }

    panes: {
        "matImage" : {
            create: function(p, s){
                var pane = root.imagePaneFactory.createObject(p)
                if ( s )
                    pane.paneInitialize(s)
                return pane
            },
            single: false
        }
    }

    menuInterceptors: [
        {
            whenItem: 'timelineResizableSegment',
            intercept: function(pane, item){
                if ( item.currentSegment instanceof Cv.VideoSegment || item.currentSegment instanceof Cv.ImageSegment ){
                    var segment = item.currentSegment
                    return [{
                        name : "Create Filter",
                        action : function(){
                            if ( !project.isDirProject() ){
                                lk.layers.workspace.messages.pushError(
                                    "Cannot adjust for non-directory project. Open the project as a directory to adjust.", 3000
                                )
                            }

                            if ( segment.filters === '' ){
                                lk.layers.workspace.wizards.addFile(
                                    project.dir(),
                                    {
                                        'extension': 'qml',
                                        'heading' : 'Add adjustments file in ' + project.dir()
                                    },
                                    function(file){
                                        var pd = project.openTextFile(file, Document.Edit)
                                        pd.insert(
                                            0,
                                            pd.contentLength(),
                                            'import QtQuick 2.3\n' +
                                            'import base 1.0\n' +
                                            'import lcvcore 1.0\n' +
                                            'import lcvimgproc 1.0\n' +
                                            'import lcvphoto 1.0\n' +
                                            '\n' +
                                            'StreamFilter{\n' +
                                            '    id: streamFilter\n' +
                                            '    StreamValue{\n' +
                                            '        id: streamValue\n' +
                                            '        valueType: \'qml/object\'\n' +
                                            '    }\n' +
                                            '    StreamSink{\n' +
                                            '        id: streamSink\n' +
                                            '        input: streamValue.value\n' +
                                            '    }\n' +
                                            '}\n',
                                            ProjectDocument.Palette
                                        )
                                        pd.save()

                                        var pane = lk.layers.workspace.panes.createPane('editor', {}, [400, 0])

                                        var panes = lk.layers.workspace.panes.findPanesByType('editor')
                                        if ( panes.length === 0 ){
                                            var foundPane = panes[0]
                                            var foundPaneIndex = foundPane.parentSplitViewIndex

                                            lk.layers.workspace.panes.splitPaneVerticallyWith(foundPane.parentSplitView, foundPaneIndex, pane)
                                        } else {
                                            var containerUsed = lk.layers.workspace.panes.container
                                            if ( containerUsed.orientation === Qt.Vertical ){
                                                for ( var i = 0; i < containerUsed.panes.length; ++i ){
                                                    if ( containerUsed.panes[i].paneType === 'splitview' &&
                                                         containerUsed.panes[i].orientation === Qt.Horizontal )
                                                    {
                                                        containerUsed = containerUsed.panes[i]
                                                        break
                                                    }
                                                }
                                            }
                                            lk.layers.workspace.panes.splitPaneHorizontallyWith(containerUsed, 0, pane)
                                        }

                                        pane.document = pd
                                        segment.filters = file.path

                                        lk.layers.workspace.extensions.editqml.shapeAllInEditor(pane)
                                    }
                                )

                            } else {
                                var path = segment.filters

                                var pd = project.openTextFile(path, Document.Edit)

                                var pane = lk.layers.workspace.panes.createPane('editor', {}, [400, 0])

                                var panes = lk.layers.workspace.panes.findPanesByType('editor')
                                if ( panes.length === 0 ){
                                    var foundPane = panes[0]
                                    var foundPaneIndex = foundPane.parentSplitViewIndex
                                    lk.layers.workspace.panes.splitPaneVerticallyWith(foundPane.parentSplitView, foundPaneIndex, pane)
                                } else {
                                    var containerUsed = lk.layers.workspace.panes.container
                                    if ( containerUsed.orientation === Qt.Vertical ){
                                        for ( var i = 0; i < containerUsed.panes.length; ++i ){
                                            if ( containerUsed.panes[i].paneType === 'splitview' &&
                                                 containerUsed.panes[i].orientation === Qt.Horizontal )
                                            {
                                                containerUsed = containerUsed.panes[i]
                                                break
                                            }
                                        }
                                    }
                                    lk.layers.workspace.panes.splitPaneHorizontallyWith(containerUsed, 0, pane)
                                }

                                pane.document = pd
                                lk.layers.workspace.extensions.editqml.shapeAllInEditor(pane)
                            }
                        }
                    },{
                        name : "Create Filter using Nodes",
                        action : function(){
                            if ( !project.isDirProject() ){
                                lk.layers.workspace.messages.pushError(
                                    "Cannot adjust for non-directory project. Open the project as a directory to adjust.", 3000
                                )
                            }

                            if ( segment.filters === '' ){
                                lk.layers.workspace.wizards.addFile(
                                    project.dir(),
                                    {
                                        'extension': 'qml',
                                        'heading' : 'Add adjustments file in ' + project.dir()
                                    },
                                    function(file){
                                        var pd = project.openTextFile(file, Document.Edit)
                                        pd.insert(
                                            0,
                                            pd.contentLength(),
                                            'import QtQuick 2.3\n' +
                                            'import base 1.0\n' +
                                            'import lcvcore 1.0\n' +
                                            'import lcvimgproc 1.0\n' +
                                            'import lcvphoto 1.0\n' +
                                            '\n' +
                                            'StreamFilter{\n' +
                                            '    id: streamFilter\n' +
                                            '    StreamValue{\n' +
                                            '        id: videoFrame\n' +
                                            '        valueType: \'qml/object\'\n' +
                                            '    }\n' +
                                            '    StreamSink{\n' +
                                            '        id: videoSurface\n' +
                                            '        input: videoFrame.value\n' +
                                            '    }\n' +
                                            '}\n',
                                            ProjectDocument.Palette
                                        )
                                        pd.save()

                                        var pane = lk.layers.workspace.panes.createPane('editor', {}, [400, 0])

                                        var panes = lk.layers.workspace.panes.findPanesByType('editor')
                                        if ( panes.length === 0 ){
                                            var foundPane = panes[0]
                                            var foundPaneIndex = foundPane.parentSplitViewIndex

                                            lk.layers.workspace.panes.splitPaneVerticallyWith(foundPane.parentSplitView, foundPaneIndex, pane)
                                        } else {
                                            var containerUsed = lk.layers.workspace.panes.container
                                            if ( containerUsed.orientation === Qt.Vertical ){
                                                for ( var i = 0; i < containerUsed.panes.length; ++i ){
                                                    if ( containerUsed.panes[i].paneType === 'splitview' &&
                                                         containerUsed.panes[i].orientation === Qt.Horizontal )
                                                    {
                                                        containerUsed = containerUsed.panes[i]
                                                        break
                                                    }
                                                }
                                            }
                                            lk.layers.workspace.panes.splitPaneHorizontallyWith(containerUsed, 0, pane)
                                        }

                                        pane.document = pd
                                        segment.filters = file.path

                                        var editor = pane
                                        var codeHandler = editor.code.language
                                        var rootPosition = lk.layers.workspace.extensions.editqml.rootPosition = codeHandler.findRootPosition()
                                        lk.layers.workspace.extensions.editqml.shapeImports(editor, codeHandler)
                                        lk.layers.workspace.extensions.editqml.shapeRootObject(editor, editor.code.language, function(){
                                            var palettesForRoot = codeHandler.findPalettes(rootPosition)
                                            var pos = palettesForRoot.declaration.position
                                            palettesForRoot.data = lk.layers.workspace.extensions.editqml.paletteControls.filterOutPalettes(palettesForRoot.data)
                                            var oc = lk.layers.workspace.extensions.editqml.paletteControls.shapePalette(
                                                editor,
                                                palettesForRoot.data.length > 0 ? palettesForRoot.data[0].name: "",
                                                pos
                                            )
                                            oc.contentWidth = Qt.binding(function(){
                                                return oc.containerContentWidth > oc.editorContentWidth ? oc.containerContentWidth : oc.editorContentWidth
                                            })

                                            codeHandler.rootShaped = true

                                            var pb = lk.layers.workspace.extensions.editqml.paletteControls.openPaletteInObjectContainer(oc, 'NodePalette')

                                            pb.child.resize(oc.width - 50, editor.height - 170)
                                        })
                                    }
                                )

                            } else {
                                var path = segment.filters

                                var pd = project.openTextFile(path, Document.Edit)

                                var pane = lk.layers.workspace.panes.createPane('editor', {}, [400, 0])

                                var panes = lk.layers.workspace.panes.findPanesByType('editor')
                                if ( panes.length === 0 ){
                                    var foundPane = panes[0]
                                    var foundPaneIndex = foundPane.parentSplitViewIndex
                                    lk.layers.workspace.panes.splitPaneVerticallyWith(foundPane.parentSplitView, foundPaneIndex, pane)
                                } else {
                                    var containerUsed = lk.layers.workspace.panes.container
                                    if ( containerUsed.orientation === Qt.Vertical ){
                                        for ( var i = 0; i < containerUsed.panes.length; ++i ){
                                            if ( containerUsed.panes[i].paneType === 'splitview' &&
                                                 containerUsed.panes[i].orientation === Qt.Horizontal )
                                            {
                                                containerUsed = containerUsed.panes[i]
                                                break
                                            }
                                        }
                                    }
                                    lk.layers.workspace.panes.splitPaneHorizontallyWith(containerUsed, 0, pane)
                                }

                                pane.document = pd

                                var editor = pane
                                var codeHandler = editor.code.language
                                var rootPosition = lk.layers.workspace.extensions.editqml.rootPosition = codeHandler.findRootPosition()
                                lk.layers.workspace.extensions.editqml.shapeImports(editor, codeHandler)
                                lk.layers.workspace.extensions.editqml.shapeRootObject(editor, editor.code.language, function(){
                                    var palettesForRoot = codeHandler.findPalettes(rootPosition)
                                    var pos = palettesForRoot.declaration.position
                                    palettesForRoot.data = lk.layers.workspace.extensions.editqml.paletteControls.filterOutPalettes(palettesForRoot.data)
                                    var oc = lk.layers.workspace.extensions.editqml.paletteControls.shapePalette(
                                        editor,
                                        palettesForRoot.data.length > 0 ? palettesForRoot.data[0].name: "",
                                        pos
                                    )
                                    oc.contentWidth = Qt.binding(function(){
                                        return oc.containerContentWidth > oc.editorContentWidth ? oc.containerContentWidth : oc.editorContentWidth
                                    })

                                    codeHandler.rootShaped = true

                                    var pb = lk.layers.workspace.extensions.editqml.paletteControls.openPaletteInObjectContainer(oc, 'NodePalette')

                                    pb.child.resize(oc.width - 50, editor.height - 170)
                                })

                            }
                        }
                    }]
                }


                return []
            }
        }
    ]

    property Component imagePaneFactory : Component{
        MatViewPane{}
    }
}
