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
                        if ( blankImageEdit.type() === 'qml/lcvcore#Draw' ){
                            var objectContainer = blankImageEdit.visualParent.parent.parent.parent

                            var coords = objectContainer.mapToItem(editor, 0, 0)
                            var editorCoords = editorPane.mapGlobalPosition()
                            highlight = createHighlight(state)
                            highlight.boxX = coords.x + editorCoords.x + 20
                            highlight.boxY = coords.y + editorCoords.y + 33
                            highlight.box.width = 110
                            highlight.box.height = 35

                        }
                    }
                }
            }
        } else if ( fragment === 'brush-icon' ){
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
                        if ( blankImageEdit.type() === 'qml/lcvcore#Draw' ){
                            var objectContainer = blankImageEdit.visualParent.parent.parent.parent

                            var coords = objectContainer.mapToItem(editor, 0, 0)
                            var editorCoords = editorPane.mapGlobalPosition()
                            highlight = createHighlight(state)
                            highlight.boxX = coords.x + editorCoords.x + 7
                            highlight.boxY = coords.y + editorCoords.y + 115
                            highlight.box.width = 35
                            highlight.box.height = 35

                        }
                    }
                }
            }
        } else if ( fragment === 'color-icon' ){
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
                        if ( blankImageEdit.type() === 'qml/lcvcore#Draw' ){
                            var objectContainer = blankImageEdit.visualParent.parent.parent.parent

                            var coords = objectContainer.mapToItem(editor, 0, 0)
                            var editorCoords = editorPane.mapGlobalPosition()
                            highlight = createHighlight(state)
                            highlight.boxX = coords.x + editorCoords.x + 7
                            highlight.boxY = coords.y + editorCoords.y + 140
                            highlight.box.width = 35
                            highlight.box.height = 35

                        }
                    }
                }
            }
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
            highlight.boxY = coords.y - 15
            highlight.box.width = 330
            highlight.box.height = 235
            highlight.box.border.width = 5
            highlight.box.border.color = highlight.box.color
            highlight.box.color = 'transparent'
        }

    }

}
