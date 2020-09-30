import QtQuick 2.9
import editor 1.0
import editqml 1.0
import workspace 1.0

WorkspaceControl{

    property Component highlightFactory : Highlighter{}

    function createHighlight(state){
        
        // clear multiple highlights
        if ( state.currentHighlight ){
        for ( var i = 0; i < state.currentHighlight.length; ++i ){
            state.currentHighlight[i].destroy()
        }
            state.currentHighlight = null
        }
    
        var ob = lk.layers.window.dialogs.overlayBox(highlightFactory.createObject())
        ob.centerBox = false
        ob.backgroundVisible = false
        ob.box.visible = true

        if ( !state.currentHighlight )
            state.currentHighlight = []
        state.currentHighlight.push(ob)

        return ob
    }

    function highlightGridChild(state, type, width, id = ""){
        var highlight = null
        var editorPane = lk.layers.workspace.panes.focusPane('editor')
        if ( !editorPane )
            return

        var editor = editorPane.editor
        var codeHandler = editor.documentHandler.codeHandler

        var editingFragments = codeHandler.editingFragments()

        for ( var i = 0; i < editingFragments.length; ++i ){
            var gridEdit = editingFragments[i]
            if ( gridEdit.type() === 'qml/QtQuick#Grid' ){
                var childFragments = gridEdit.getChildFragments()
                for ( var j = 0; j < childFragments.length; ++j ){
                    var child = childFragments[j]
                    if ( child.type() === type && (id === "" || child.objectId() === id)){

                        var objectContainer = child.visualParent.parent.parent.parent

                        var coords = objectContainer.mapToItem(editor, 0, 0)
                        var editorCoords = editorPane.mapGlobalPosition()
                        editor.makePositionVisible(coords.y + editorCoords.y + 33 - 80)                            
                        coords = objectContainer.mapToItem(editor, 0, 0)
                        highlight = createHighlight(state)
                        highlight.boxX = coords.x + editorCoords.x + 30
                        highlight.boxY = coords.y + editorCoords.y + 33
                        highlight.box.width = width
                        highlight.box.height = 35
                    }
                }
            }
        }
        return highlight
    }

    run: function(workspace, state, fragment){
        var highlight = null
        if ( fragment === 'fileexplorer' ){
            highlight = highlightGridChild(state, "qml/fs#VisualFileSelector", 260)
        } else if ( fragment === 'extensionpass' ){
            highlight = highlightGridChild(state, "qml/fs#ExtensionPass", 230)
        } else if ( fragment === 'imageread' ){
            highlight = highlightGridChild(state, "qml/lcvcore#ImageRead", 160)
        } else if ( fragment === 'sepia'){
            highlight = highlightGridChild(state, "qml/lcvphoto#Sepia", 100)
        } else if ( fragment === 'temperature-1' ){
            highlight = highlightGridChild(state, "qml/lcvphoto#Temperature", 250, "temperature")
        } else if ( fragment === 'temperature-2' ){
            highlight = highlightGridChild(state, "qml/lcvphoto#Temperature", 250, "temperature2")
        }
    }

}
