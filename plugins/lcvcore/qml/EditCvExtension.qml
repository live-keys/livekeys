import QtQuick 2.3
import editor 1.0
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

    property Component imagePaneFactory : Component{
        MatViewPane{}
    }
}
