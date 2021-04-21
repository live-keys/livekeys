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

    function highlightItemChild(state, type, width, xOffset = 0){
        var highlight = null
        var editorPane = lk.layers.workspace.panes.focusPane('editor')
        if ( !editorPane )
            return

        var editor = editorPane.editor
        var codeHandler = editor.documentHandler.codeHandler

        var editingFragments = codeHandler.editingFragments()

        for ( var i = 0; i < editingFragments.length; ++i ){
            var gridEdit = editingFragments[i]
            if ( gridEdit.type() === 'qml/QtQuick#Item' ){
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
                        highlight.boxX = coords.x + editorCoords.x + 30 + xOffset
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
        } else if ( fragment === 'filereader' ){
            highlight = highlightItemChild(state, "qml/fs#FileReader", 150)
        } else if ( fragment === 'textsearch' ){
            highlight = highlightItemChild(state, "qml/editor#TextSearch", 160)
        } else if ( fragment === 'search-value' ){
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
                    for ( var j = 0; j < children.length; ++j){
                        var textSearch = children[j]
                        if (textSearch.type() === 'qml/editor#TextSearch'){
                            var childdren = textSearch.getChildFragments()
                            for (var k = 0; k < childdren.length; ++k){

                                var value = childdren[k]
                                if (value.identifier() === 'value'){
                                    var palettes = value.paletteList()
                                    for ( var pi = 0; pi < palettes.length; ++pi ){

                                        var palette = palettes[pi]
                                        if ( palette.name === 'TextPalette' ){
                                            var coords = palette.item.mapToItem(editor, 0, 0)
                                            var editorCoords = editorPane.mapGlobalPosition()
                                            editor.makePositionVisible(coords.y + editorCoords.y + 25 - 80)                            
                                            coords = palette.item.mapToItem(editor, 0, 0)
                                            highlight = createHighlight(state)

                                            highlight.boxX = coords.x + editorCoords.x - 10
                                            highlight.boxY = coords.y + editorCoords.y + 25
                                            highlight.box.width = 365
                                            highlight.box.height = 35
                                        }
                                    }
                                }
                            }
                        }
                    }

                }
            }
        } else if ( fragment === 'read-text' ){
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
                    for ( var j = 0; j < children.length; ++j){
                        var textSearch = children[j]
                        if (textSearch.type() === 'qml/editor#TextSearch'){
                            var childdren = textSearch.getChildFragments()
                            for (var k = 0; k < childdren.length; ++k){

                                var value = childdren[k]
                                if (value.identifier() === 'text'){
                                    var palettes = value.paletteList()
                                    for ( var pi = 0; pi < palettes.length; ++pi ){

                                        var palette = palettes[pi]
                                        if ( palette.name === 'ConnectionPalette' ){
                                            var coords = palette.item.mapToItem(editor, 0, 0)
                                            var editorCoords = editorPane.mapGlobalPosition()
                                            editor.makePositionVisible(coords.y + editorCoords.y + 25 - 80)                            
                                            coords = palette.item.mapToItem(editor, 0, 0)
                                            highlight = createHighlight(state)

                                            highlight.boxX = coords.x + editorCoords.x - 10
                                            highlight.boxY = coords.y + editorCoords.y + 25
                                            highlight.box.width = 345
                                            highlight.box.height = 35
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else if ( fragment === 'indexselector' ){
            highlight = highlightItemChild(state, "qml/base#IndexSelector", 200)
        } else if ( fragment === 'converttoint' ){
            highlight = highlightItemChild(state, "qml/base#ConvertToInt", 200)
        } else if ( fragment === 'textlineatposition' ){
            highlight = highlightItemChild(state, "qml/base#TextLineAtPosition", 260)
        } else if ( fragment === 'textclip' ){
            highlight = highlightItemChild(state, "qml/base#TextClip", 120)
        } else if ( fragment === 'read-text-clip' ){
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
                    for ( var j = 0; j < children.length; ++j){
                        var textSearch = children[j]
                        if (textSearch.type() === 'qml/base#TextClip'){
                            var childdren = textSearch.getChildFragments()
                            for (var k = 0; k < childdren.length; ++k){

                                var value = childdren[k]
                                if (value.identifier() === 'text'){
                                    var palettes = value.paletteList()
                                    for ( var pi = 0; pi < palettes.length; ++pi ){

                                        var palette = palettes[pi]
                                        if ( palette.name === 'ConnectionPalette' ){
                                            var coords = palette.item.mapToItem(editor, 0, 0)
                                            var editorCoords = editorPane.mapGlobalPosition()
                                            editor.makePositionVisible(coords.y + editorCoords.y + 25 - 80)                            
                                            coords = palette.item.mapToItem(editor, 0, 0)
                                            highlight = createHighlight(state)

                                            highlight.boxX = coords.x + editorCoords.x - 10
                                            highlight.boxY = coords.y + editorCoords.y + 25
                                            highlight.box.width = 345
                                            highlight.box.height = 35
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else if ( fragment === 'positions' ){
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
                    for ( var j = 0; j < children.length; ++j){
                        var textSearch = children[j]
                        if (textSearch.type() === 'qml/base#TextClip'){
                            var childdren = textSearch.getChildFragments()
                            for (var k = 0; k < childdren.length; ++k){

                                var value = childdren[k]
                                if (value.identifier() === 'positions'){
                                    var palettes = value.paletteList()
                                    for ( var pi = 0; pi < palettes.length; ++pi ){

                                        var palette = palettes[pi]
                                        if ( palette.name === 'ConnectionPalette' ){
                                            var coords = palette.item.mapToItem(editor, 0, 0)
                                            var editorCoords = editorPane.mapGlobalPosition()
                                            editor.makePositionVisible(coords.y + editorCoords.y + 25 - 80)                            
                                            coords = palette.item.mapToItem(editor, 0, 0)
                                            highlight = createHighlight(state)

                                            highlight.boxX = coords.x + editorCoords.x - 10
                                            highlight.boxY = coords.y + editorCoords.y + 25
                                            highlight.box.width = 345
                                            highlight.box.height = 35
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else if ( fragment === 'text-item' ){
            highlight = highlightItemChild(state, "qml/QtQuick#Text", 80)
        } else if ( fragment === 'result' ){
            var viewerPane = lk.layers.workspace.panes.focusPane('viewer')
            if ( !viewerPane )
                return

            var viewerCoords = viewerPane.mapGlobalPosition()

            highlight = createHighlight(state)

            highlight.boxX = viewerCoords.x - 10
            highlight.boxY = viewerCoords.y - 10
            highlight.box.width = 460
            highlight.box.height = 35
            highlight.box.border.width = 5
            highlight.box.border.color = highlight.box.color
            highlight.box.color = 'transparent'
        }
    }

}
