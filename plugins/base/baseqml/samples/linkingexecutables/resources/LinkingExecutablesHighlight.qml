import QtQuick 2.9
import editor 1.0
import editqml 1.0
import workspace 1.0

WorkspaceControl{

    property Component highlightFactory : Highlighter{}

    function createHighlight(state){
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

    run: function(workspace, state, fragment){
        var highlight = null
        
        if ( fragment === 'node-palette' ){
            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor
            var codeHandler = editor.documentHandler.codeHandler

            var editingFragments = codeHandler.editingFragments()

            for ( var i = 0; i < editingFragments.length; ++i ){
                var itemEdit = editingFragments[i]
                if ( itemEdit.type() === 'qml/QtQuick#Item' ){
                    var palettes = itemEdit.paletteList()
                    for ( var pi = 0; pi < palettes.length; ++pi ){

                        var palette = palettes[pi]
                        if ( palette.name === 'NodePalette' ){
                            var coords = palette.item.mapToItem(editor, 0, 0)
                            var editorCoords = editorPane.mapGlobalPosition()
                            editor.makePositionVisible(coords.y + editorCoords.y + 30 - 80)                            
                            coords = palette.item.mapToItem(editor, 0, 0)
                            highlight = createHighlight(state)

                            highlight.boxX = coords.x + editorCoords.x
                            highlight.boxY = coords.y + editorCoords.y + 30
                            highlight.box.width = 600
                            highlight.box.height = 300
                            highlight.box.border.width = 5
                            highlight.box.border.color = highlight.box.color
                            highlight.box.color = 'transparent'
                        }
                    }
                }
            }
        } else if ( fragment === 'first-exec' ) {
            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor
            var codeHandler = editor.documentHandler.codeHandler

            var editingFragments = codeHandler.editingFragments()

            for ( var i = 0; i < editingFragments.length; ++i ){
                var itemEdit = editingFragments[i]
                if ( itemEdit.type() === 'qml/QtQuick#Item' ){
                    var children = itemEdit.getChildFragments()
                    var exec1 = children[0]
                    var objectContainer = exec1.visualParent.parent.parent.parent

                    var coords = objectContainer.mapToItem(editor, 0, 0)
                    var editorCoords = editorPane.mapGlobalPosition()
                    editor.makePositionVisible(coords.y + editorCoords.y + 33 - 80)                            
                    coords = objectContainer.mapToItem(editor, 0, 0)
                    highlight = createHighlight(state)
                    highlight.boxX = coords.x + editorCoords.x + 20
                    highlight.boxY = coords.y + editorCoords.y + 33
                    highlight.box.width = 100
                    highlight.box.height = 35
                    

                }
            }
        } else if ( fragment === 'second-exec' ) {
            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor
            var codeHandler = editor.documentHandler.codeHandler

            var editingFragments = codeHandler.editingFragments()

            for ( var i = 0; i < editingFragments.length; ++i ){
                var itemEdit = editingFragments[i]
                if ( itemEdit.type() === 'qml/QtQuick#Item' ){
                    var children = itemEdit.getChildFragments()
                    var exec1 = children[1]
                    var objectContainer = exec1.visualParent.parent.parent.parent

                    var coords = objectContainer.mapToItem(editor, 0, 0)
                    var editorCoords = editorPane.mapGlobalPosition()
                    editor.makePositionVisible(coords.y + editorCoords.y + 33 - 80)                            
                    coords = objectContainer.mapToItem(editor, 0, 0)
                    highlight = createHighlight(state)
                    highlight.boxX = coords.x + editorCoords.x + 20
                    highlight.boxY = coords.y + editorCoords.y + 33
                    highlight.box.width = 120
                    highlight.box.height = 35
                    

                }
            }
        } else if ( fragment === 'path' ) {
            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor
            var codeHandler = editor.documentHandler.codeHandler

            var editingFragments = codeHandler.editingFragments()

            for ( var i = 0; i < editingFragments.length; ++i ){
                var itemEdit = editingFragments[i]
                if ( itemEdit.type() === 'qml/QtQuick#Item' ){
                    var children = itemEdit.getChildFragments()
                    var exec1 = children[0]
                    var childdren = exec1.getChildFragments()
                    for (var k = 0; k < childdren.length; ++k){

                        var value = childdren[k]
                        if (value.identifier() === 'path'){
                            var palettes = value.paletteList()
                            for ( var pi = 0; pi < palettes.length; ++pi ){

                                var palette = palettes[pi]
                                if ( palette.name === 'FilePathPalette' ){
                                    var coords = palette.item.mapToItem(editor, 0, 0)
                                    var editorCoords = editorPane.mapGlobalPosition()
                                    editor.makePositionVisible(coords.y + editorCoords.y + 25 - 80)                            
                                    coords = palette.item.mapToItem(editor, 0, 0)
                                    highlight = createHighlight(state)

                                    highlight.boxX = coords.x + editorCoords.x - 10
                                    highlight.boxY = coords.y + editorCoords.y + 25
                                    highlight.box.width = 250
                                    highlight.box.height = 35
                                }
                            }
                        }
                    }
                    

                }
            }
        } else if ( fragment === 'arg-list' ) {
            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor
            var codeHandler = editor.documentHandler.codeHandler

            var editingFragments = codeHandler.editingFragments()

            for ( var i = 0; i < editingFragments.length; ++i ){
                var itemEdit = editingFragments[i]
                if ( itemEdit.type() === 'qml/QtQuick#Item' ){
                    var children = itemEdit.getChildFragments()
                    var exec1 = children[0]
                    var childdren = exec1.getChildFragments()
                    for (var k = 0; k < childdren.length; ++k){

                        var value = childdren[k]
                        if (value.identifier() === 'args'){
                            var palettes = value.paletteList()
                            for ( var pi = 0; pi < palettes.length; ++pi ){

                                var palette = palettes[pi]
                                if ( palette.name === 'StringListPalette' ){
                                    var coords = palette.item.mapToItem(editor, 0, 0)
                                    var editorCoords = editorPane.mapGlobalPosition()
                                    editor.makePositionVisible(coords.y + editorCoords.y + 25 - 80)
                                    coords = palette.item.mapToItem(editor, 0, 0)
                                    highlight = createHighlight(state)

                                    highlight.boxX = coords.x + editorCoords.x - 10
                                    highlight.boxY = coords.y + editorCoords.y + 25
                                    highlight.box.width = palette.item.width + 20
                                    highlight.box.height = palette.item.height + 10
                                    highlight.box.border.width = 5
                                    highlight.box.border.color = highlight.box.color
                                    highlight.box.color = 'transparent'
                                }
                            }
                        }
                    }


                }
            }
        } else if ( fragment === 'connection' ) {
            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor
            var codeHandler = editor.documentHandler.codeHandler

            var editingFragments = codeHandler.editingFragments()

            for ( var i = 0; i < editingFragments.length; ++i ){
                var itemEdit = editingFragments[i]
                if ( itemEdit.type() === 'qml/QtQuick#Item' ){
                    var palettes = itemEdit.paletteList()
                    for ( var pi = 0; pi < palettes.length; ++pi ){

                        var palette = palettes[pi]
                        if ( palette.name === 'NodePalette' ){
                            var coords = palette.item.mapToItem(editor, 0, 0)
                            var editorCoords = editorPane.mapGlobalPosition()
                            editor.makePositionVisible(coords.y + editorCoords.y + 70 - 80)
                            coords = palette.item.mapToItem(editor, 0, 0)
                            highlight = createHighlight(state)

                            highlight.boxX = coords.x + editorCoords.x + 185
                            highlight.boxY = coords.y + editorCoords.y + 70
                            highlight.box.width = 40
                            highlight.box.height = 60
                            highlight.box.border.width = 5
                            highlight.box.border.color = highlight.box.color
                            highlight.box.color = 'transparent'
                        }
                    }
                }
            }
        }  else if ( fragment === 'streamlog' ) {
            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor
            var codeHandler = editor.documentHandler.codeHandler

            var editingFragments = codeHandler.editingFragments()

            for ( var i = 0; i < editingFragments.length; ++i ){
                var itemEdit = editingFragments[i]
                if ( itemEdit.type() === 'qml/QtQuick#Item' ){
                    var children = itemEdit.getChildFragments()
                    var exec1 = children[2]
                    var objectContainer = exec1.visualParent.parent.parent.parent

                    var coords = objectContainer.mapToItem(editor, 0, 0)
                    var editorCoords = editorPane.mapGlobalPosition()
                    editor.makePositionVisible(coords.y + editorCoords.y + 33 - 80)
                    coords = objectContainer.mapToItem(editor, 0, 0)
                    highlight = createHighlight(state)
                    highlight.boxX = coords.x + editorCoords.x + 20
                    highlight.boxY = coords.y + editorCoords.y + 33
                    highlight.box.width = 180
                    highlight.box.height = 35


                }
            }
        } else if ( fragment === 'log-view' ){
            highlight = createHighlight(state)
            highlight.boxX = 220 + 35 * 3 - 15
            highlight.boxY = -10
        }
    }

}
