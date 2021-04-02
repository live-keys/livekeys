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

    function findDrawPalette(){
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
                    var drawSurfaceEdit = childFragments[j]
                    if ( drawSurfaceEdit.type() === 'qml/lcvcore#DrawSurface' ){
                        var palettes = drawSurfaceEdit.paletteList()
                        for ( var pi = 0; pi < palettes.length; ++pi ){
                            var palette = palettes[pi]
                            if (palette.name === 'DrawPalette'){
                                return palette
                            }
                        }
                    }
                }
            }
        }

        return null
    }

    run: function(workspace, state, fragment){
        var highlight = null
        if ( fragment === 'first-component' ){
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
                        var blankImageEdit = childFragments[j]
                        if ( blankImageEdit.type() === 'qml/lcvcore#BlankImage' ){
                            var objectContainer = blankImageEdit.visualParent.parent.parent.parent

                            var coords = objectContainer.mapToItem(editor, 0, 0)
                            var editorCoords = editorPane.mapGlobalPosition()
                            editor.makePositionVisible(coords.y + editorCoords.y + 33 - 80)                            
                            coords = objectContainer.mapToItem(editor, 0, 0)
                            highlight = createHighlight(state)
                            highlight.boxX = coords.x + editorCoords.x + 20
                            highlight.boxY = coords.y + editorCoords.y + 33
                            highlight.box.width = 200
                            highlight.box.height = 35

                        }
                    }
                }
            }
        } else if ( fragment === 'draw-component' ){
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
                        var blankImageEdit = childFragments[j]
                        if ( blankImageEdit.type() === 'qml/lcvcore#DrawSurface' ){
                            var objectContainer = blankImageEdit.visualParent.parent.parent.parent

                            var coords = objectContainer.mapToItem(editor, 0, 0)
                            var editorCoords = editorPane.mapGlobalPosition()
                            editor.makePositionVisible(coords.y + editorCoords.y + 33 - 80)                            
                            coords = objectContainer.mapToItem(editor, 0, 0)
                            highlight = createHighlight(state)
                            highlight.boxX = coords.x + editorCoords.x + 20
                            highlight.boxY = coords.y + editorCoords.y + 33
                            highlight.box.width = 210
                            highlight.box.height = 35

                        }
                    }
                }
            }
        } else if ( fragment === 'brush-icon' ){
            var drawPalette = findDrawPalette()
            if (!drawPalette) return

            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor
            var coords = drawPalette.item.mapToItem(editor, 0, 0)
            var editorCoords = editorPane.mapGlobalPosition()
            var xOffset = -3
            var yOffset = 65
            editor.makePositionVisible(coords.y + editorCoords.y + yOffset - 80)
            coords = drawPalette.item.mapToItem(editor, 0, 0)
            highlight = createHighlight(state)

            highlight.boxX = coords.x + editorCoords.x + xOffset
            highlight.boxY = coords.y + editorCoords.y + yOffset
            highlight.box.width = 30
            highlight.box.height = 30
            return highlight
        } else if ( fragment === 'brush-size' ){
            var drawPalette = findDrawPalette()
            if (!drawPalette) return

            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor
            var coords = drawPalette.item.mapToItem(editor, 0, 0)
            var editorCoords = editorPane.mapGlobalPosition()
            var xOffset = 110
            var yOffset = 30
            editor.makePositionVisible(coords.y + editorCoords.y + yOffset - 80)
            coords = drawPalette.item.mapToItem(editor, 0, 0)
            highlight = createHighlight(state)

            highlight.boxX = coords.x + editorCoords.x + xOffset
            highlight.boxY = coords.y + editorCoords.y + yOffset
            highlight.box.width = 30
            highlight.box.height = 30
            return highlight
        } else if ( fragment === 'gradient-icon' ){
            var drawPalette = findDrawPalette()
            if (!drawPalette) return

            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor
            var coords = drawPalette.item.mapToItem(editor, 0, 0)
            var editorCoords = editorPane.mapGlobalPosition()
            var xOffset = 24
            var yOffset = 65
            editor.makePositionVisible(coords.y + editorCoords.y + yOffset - 80)
            coords = drawPalette.item.mapToItem(editor, 0, 0)
            highlight = createHighlight(state)

            highlight.boxX = coords.x + editorCoords.x + xOffset
            highlight.boxY = coords.y + editorCoords.y + yOffset
            highlight.box.width = 30
            highlight.box.height = 30
            return highlight
        } else if ( fragment === 'color-pickers' ){
            var drawPalette = findDrawPalette()
            if (!drawPalette) return

            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor
            var coords = drawPalette.item.mapToItem(editor, 0, 0)
            var editorCoords = editorPane.mapGlobalPosition()
            var xOffset = -10
            var yOffset = 180
            editor.makePositionVisible(coords.y + editorCoords.y + yOffset - 80)
            coords = drawPalette.item.mapToItem(editor, 0, 0)
            highlight = createHighlight(state)

            highlight.boxX = coords.x + editorCoords.x + xOffset
            highlight.boxY = coords.y + editorCoords.y + yOffset
            highlight.box.width = 70
            highlight.box.height = 70
            highlight.box.border.width = 5
            highlight.box.border.color = highlight.box.color
            highlight.box.color = 'transparent'
            return highlight

        } else if ( fragment === 'histogram-component' ){
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
                        var blankImageEdit = childFragments[j]
                        if ( blankImageEdit.type() === 'qml/lcvcore#ColorHistogramView' ){
                            var objectContainer = blankImageEdit.visualParent.parent.parent.parent

                            var coords = objectContainer.mapToItem(editor, 0, 0)
                            var editorCoords = editorPane.mapGlobalPosition()
                            editor.makePositionVisible(coords.y + editorCoords.y + 33 - 80)                            
                            coords = objectContainer.mapToItem(editor, 0, 0)
                            highlight = createHighlight(state)
                            highlight.boxX = coords.x + editorCoords.x + 25
                            highlight.boxY = coords.y + editorCoords.y + 33
                            highlight.box.width = 150
                            highlight.box.height = 35

                        }
                    }
                }
            }
        } else if ( fragment === 'histogram-view' ){
            var viewerPane = lk.layers.workspace.panes.focusPane('viewer')
            if ( !viewerPane )
                return

            var coords = viewerPane.mapGlobalPosition()
            highlight = createHighlight(state)
            highlight.boxX = coords.x - 15
            highlight.boxY = coords.y + 15
            highlight.box.width = 330
            highlight.box.height = 235
            highlight.box.border.width = 5
            highlight.box.border.color = highlight.box.color
            highlight.box.color = 'transparent'
        }

    }

}
