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
    
    function highlightTransformButton(state, xOffset, yOffset){
        var highlight = null
        var editorPane = lk.layers.workspace.panes.focusPane('editor')
        if ( !editorPane )
            return

        var editor = editorPane.editor
        var codeHandler = editor.documentHandler.codeHandler

        var editingFragments = codeHandler.editingFragments()
        for ( var i = 0; i < editingFragments.length; ++i ){
            var itemEdit = editingFragments[i]
            if ( itemEdit.type() === 'qml/QtQuick#Grid' ){
                var childFragments = itemEdit.getChildFragments()
                for ( var j = 0; j < childFragments.length; ++j ){
                    var transformImage = childFragments[j]
                    if ( transformImage.type() === 'qml/lcvimgproc#TransformImage' ){
                        var palettes = transformImage.paletteList()
                        for ( var pi = 0; pi < palettes.length; ++pi ){
                            var palette = palettes[pi]
                            if ( palette.name === 'TransformPalette' ){
                                var coords = palette.item.mapToItem(editor, 0, 0)
                                var editorCoords = editorPane.mapGlobalPosition()
                                editor.makePositionVisible(coords.y + editorCoords.y + yOffset - 80)                            
                                coords = palette.item.mapToItem(editor, 0, 0)

                                highlight = createHighlight(state)
                                highlight.boxX = coords.x + editorCoords.x + xOffset
                                highlight.boxY = coords.y + editorCoords.y + yOffset
                                highlight.box.width = 30
                                highlight.box.height = 30
                                
                            }
                            
                        }
                    }
                }
            }
        }
        return highlight
    }

    run: function(workspace, state, fragment){
        var highlight = null
        if ( fragment === 'crop-button' ){
            highlight = highlightTransformButton(state, -2,60)
        } else if ( fragment === 'resize-button' ){
            highlight = highlightTransformButton(state, 24,60)
        } else if ( fragment === 'rotate-button' ){
            highlight = highlightTransformButton(state, -2,86)
        } else if ( fragment === 'warp-button' ){
            highlight = highlightTransformButton(state, 24,86)
        } else if ( fragment === 'transformation' ){
            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor
            var codeHandler = editor.documentHandler.codeHandler

            var editingFragments = codeHandler.editingFragments()
            for ( var i = 0; i < editingFragments.length; ++i ){
                var itemEdit = editingFragments[i]
                if ( itemEdit.type() === 'qml/QtQuick#Grid' ){
                    var childFragments = itemEdit.getChildFragments()
                    for ( var j = 0; j < childFragments.length; ++j ){
                        var transformImage = childFragments[j]
                        if ( transformImage.type() === 'qml/lcvimgproc#TransformImage' ){
                            var transformChildFragments = transformImage.getChildFragments()
                            for ( var k = 0; k < transformChildFragments.length; ++k ){
                                var crop = transformChildFragments[k]
                                if ( crop.type() === 'qml/lcvcore#Crop' ){
                                    var objectContainer = crop.visualParent.parent.parent.parent

                                    var coords = objectContainer.mapToItem(editor, 0, 0)
                                    var editorCoords = editorPane.mapGlobalPosition()
                                    editor.makePositionVisible(coords.y + editorCoords.y + 33 - 80)                            
                                    coords = objectContainer.mapToItem(editor, 0, 0)
                                    
                                    highlight = createHighlight(state)
                                    highlight.boxX = coords.x + editorCoords.x + 20
                                    highlight.boxY = coords.y + editorCoords.y + 33
                                    highlight.box.width = 70
                                    highlight.box.height = 35
                                }
                            }
                        }
                    }
                }
            }
        }
    }

}
