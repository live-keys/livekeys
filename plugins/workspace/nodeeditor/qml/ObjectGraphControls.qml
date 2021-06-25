import QtQuick 2.3

QtObject{

    objectName: "objectgraph"

    function removeActiveItem(){
        var activeItem = lk.layers.workspace.panes.activeItem
        if ( activeItem.objectName === 'objectGraph' ){
            if ( activeItem.selectedEdge ){
                activeItem.removeEdge(activeItem.selectedEdge)
            }
        }
    }

    function nodeEditMode(){
        var panes = lk.layers.workspace.panes
        panes.reset()
        var fe = panes.createPane('editor', {}, [400, 0])

        var containerUsed = panes.container
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

        panes.splitPaneHorizontallyWith(containerUsed, 0, fe)
        fe.document = project.openTextFile(project.active.path)


        var editor = fe
        var codeHandler = editor.documentHandler.codeHandler
        var rootPosition = lk.layers.workspace.extensions.editqml.rootPosition = codeHandler.findRootPosition()
        lk.layers.workspace.extensions.editqml.shapeImports(editor, codeHandler)
        lk.layers.workspace.extensions.editqml.shapeRootObject(editor, editor.documentHandler.codeHandler, function(){
            var palettesForRoot = codeHandler.findPalettes(rootPosition)
            var pos = palettesForRoot.declaration.position
            palettesForRoot.data = lk.layers.workspace.extensions.editqml.paletteControls.filterOutPalettes(palettesForRoot.data)
            var oc = lk.layers.workspace.extensions.editqml.paletteControls.shapePalette(
                editor,
                palettesForRoot.data.length > 0 ? palettesForRoot.data[0] : "",
                pos
            )
            oc.contentWidth = Qt.binding(function(){
                return oc.containerContentWidth > oc.editorContentWidth ? oc.containerContentWidth : oc.editorContentWidth
            })

            editor.editor.rootShaped = true

            var pb = lk.layers.workspace.extensions.editqml.paletteControls.openPaletteByName('NodePalette', oc.editingFragment, oc.groupsContainer.children[0], oc)

            pb.child.resize(oc.width - 50, editor.height - 170)
        })

    }
}
