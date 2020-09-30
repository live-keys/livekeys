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
    
    function highlightGridChild(state, type, width, xOffset = 0){
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
                    if ( child.type() === type ){

                        var objectContainer = child.visualParent.parent.parent.parent

                        var coords = objectContainer.mapToItem(editor, 0, 0)
                        var editorCoords = editorPane.mapGlobalPosition()
                        editor.makePositionVisible(coords.y + editorCoords.y + 33 - 80)                            
                        coords = objectContainer.mapToItem(editor, 0, 0)
                        highlight = createHighlight(state)
                        highlight.boxX = coords.x + editorCoords.x + 20 + xOffset
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
        if ( fragment === 'loading' ){
            highlight = highlightGridChild(state, 'qml/lcvcore#ImageFile', 160)
        } else if ( fragment === 'levels' ){
            highlight = highlightGridChild(state, 'qml/lcvphoto#Levels', 120)
        } else if ( fragment === 'hsl' ){
            highlight = highlightGridChild(state, 'qml/lcvphoto#HueSaturationLightness', 280)
        } else if ( fragment === 'brightness-and-contrast' ){
            highlight = highlightGridChild(state, 'qml/lcvphoto#BrightnessAndContrast', 280)
        } else if ( fragment === 'imageview' ){
            highlight = highlightGridChild(state, 'qml/lcvcore#ImageView', 90, 5)
        } 
    }

}
