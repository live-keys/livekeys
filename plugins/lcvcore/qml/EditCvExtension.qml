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
                        editor.parentSplitter, editor.parentSplitterIndex(), fe
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
        "matImage" : function(p, s){
            var pane = root.imagePaneFactory.createObject(p)
            if ( s )
                pane.paneInitialize(s)
            return pane
        },
    }

    menuInterceptors: [
        {
            whenItem: 'timelineResizableSegment',
            intercept: function(pane, item){
                if ( item.currentSegment instanceof Cv.VideoSegment || item.currentSegment instanceof Cv.ImageSegment ){
                    var segment = item.currentSegment
                    return [{
                        name : "Adjust",
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
                                            var foundPaneIndex = foundPane.parentSplitterIndex

                                            lk.layers.workspace.panes.splitPaneVerticallyWith(foundPane.parentSplitter, foundPaneIndex, pane)
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
                                    var foundPaneIndex = foundPane.parentSplitterIndex
                                    lk.layers.workspace.panes.splitPaneVerticallyWith(foundPane.parentSplitter, foundPaneIndex, pane)
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
