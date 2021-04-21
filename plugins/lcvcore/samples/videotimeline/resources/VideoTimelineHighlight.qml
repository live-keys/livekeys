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

    function createTimelineHighlight(state, xOffset, yOffset){
        var highlight = null
        var editorPane = lk.layers.workspace.panes.focusPane('editor')
        if ( !editorPane )
            return

        var editor = editorPane.editor
        var codeHandler = editor.documentHandler.codeHandler

        var editingFragments = codeHandler.editingFragments()

        for ( var i = 0; i < editingFragments.length; ++i ){
            var itemEdit = editingFragments[i]
            if ( itemEdit.type() === 'qml/QtQuick#Item' ){
                var childFragments = itemEdit.getChildFragments()
                for ( var j = 0; j < childFragments.length; ++j ){
                    var tlEdit = childFragments[j]
                    if ( tlEdit.type() === 'qml/timeline#Timeline' ){
                        //var childdren = tlEdit.getChildFragments()
                        var palettes = tlEdit.paletteList()
                        for ( var pi = 0; pi < palettes.length; ++pi ){

                            var palette = palettes[pi]
                            if ( palette.name === 'TimelinePalette' ){
                                var coords = palette.item.mapToItem(editor, 0, 0)
                                var editorCoords = editorPane.mapGlobalPosition()
                                editor.makePositionVisible(coords.y + editorCoords.y + yOffset - 80)                            
                                coords = palette.item.mapToItem(editor, 0, 0)
                                highlight = createHighlight(state)

                                highlight.boxX = coords.x + editorCoords.x + xOffset
                                highlight.boxY = coords.y + editorCoords.y + yOffset
                                return highlight
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
        if ( fragment === 'timeline-bound' ){
            highlight = createTimelineHighlight(state, -20, 10)
            highlight.box.width = 565
            highlight.box.height = 240
            highlight.box.border.width = 5
            highlight.box.border.color = highlight.box.color
            highlight.box.color = 'transparent'

        } else if ( fragment === 'tracks' ){
            highlight = createTimelineHighlight(state, 13, 62)
            highlight.box.width = 100
            highlight.box.height = 60
        } else if ( fragment === 'timeline-item' ){
            highlight = createTimelineHighlight(state, 18, -12)
            highlight.box.width = 130
            highlight.box.height = 35
        } else if ( fragment === 'video-track-1' ){
            highlight = createTimelineHighlight(state, 13, 68)
            highlight.box.width = 100
            highlight.box.height = 20
        } else if ( fragment === 'play-button' ){
            highlight = createTimelineHighlight(state, 40, 31)
            highlight.box.width = 35
            highlight.box.height = 35
        } else if ( fragment === 'options-button' ){
            highlight = createTimelineHighlight(state, -2, 67)
            highlight.box.width = 20
            highlight.box.height = 20
        } else if ( fragment === 'keyframe-track' ){
            highlight = createTimelineHighlight(state, 13, 94)
            highlight.box.width = 65
            highlight.box.height = 20
        } else if ( fragment === 'keyframes' ){
            highlight = createTimelineHighlight(state, 210, 90)
            highlight.box.width = 62
            highlight.box.height = 25
        } else if ( fragment === 'keyframevalue'){
            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor
            var codeHandler = editor.documentHandler.codeHandler

            var editingFragments = codeHandler.editingFragments()

            for ( var i = 0; i < editingFragments.length; ++i ){
                var itemEdit = editingFragments[i]
                if ( itemEdit.type() === 'qml/QtQuick#Item' ){
                    var childFragments = itemEdit.getChildFragments()
                    for ( var j = 0; j < childFragments.length; ++j ){
                        var kfv = childFragments[j]
                        if ( kfv.type() === 'qml/timeline#KeyframeValue' ){
                            var objectContainer = kfv.visualParent.parent.parent.parent

                            var coords = objectContainer.mapToItem(editor, 0, 0)
                            var editorCoords = editorPane.mapGlobalPosition()
                            editor.makePositionVisible(coords.y + editorCoords.y + 32 - 80)                            
                            coords = objectContainer.mapToItem(editor, 0, 0)
                            highlight = createHighlight(state)
                            highlight.boxX = coords.x + editorCoords.x + 25
                            highlight.boxY = coords.y + editorCoords.y + 32
                            highlight.box.width = 210
                            highlight.box.height = 35
                        }
                    }
                }
            }
        } else if ( fragment === 'videosurfaceview'){
            var viewerPane = lk.layers.workspace.panes.focusPane('viewer')
            if ( !viewerPane )
                return

            var coords = viewerPane.mapGlobalPosition()
            highlight = createHighlight(state)
            highlight.boxX = coords.x
            highlight.boxY = coords.y + 30
            highlight.box.width = 600
            highlight.box.height = 400
            highlight.box.border.width = 5
            highlight.box.border.color = highlight.box.color
            highlight.box.color = 'transparent'
        } else if ( fragment === 'blue-rectangle' ){
            var viewerPane = lk.layers.workspace.panes.focusPane('viewer')
            if ( !viewerPane )
                return

            var coords = viewerPane.mapGlobalPosition()
            highlight = createHighlight(state)
            highlight.boxX = coords.x
            highlight.boxY = coords.y + viewerPane.height - 100
            highlight.box.width = 100
            highlight.box.height = 100
            highlight.box.border.width = 5
            highlight.box.border.color = highlight.box.color
            highlight.box.color = 'transparent'
        } else if ( fragment === 'timeline-options-button' ){
            highlight = createTimelineHighlight(state, -3, 31)
            highlight.box.width = 35
            highlight.box.height = 35
        } else if ( fragment === 'file-property' ){
            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor
            var codeHandler = editor.documentHandler.codeHandler

            var editingFragments = codeHandler.editingFragments()

            for ( var i = 0; i < editingFragments.length; ++i ){
                var itemEdit = editingFragments[i]
                if ( itemEdit.type() === 'qml/QtQuick#Item' ){
                    var childFragments = itemEdit.getChildFragments()
                    for ( var j = 0; j < childFragments.length; ++j ){
                        var timeline = childFragments[j]
                        if ( timeline.type() === 'qml/timeline#Timeline' ){
                            var childdren = timeline.getChildFragments()
                            for ( var k = 0; k < childdren.length; ++k ){
                                var property = childdren[k]
                                if ( property.identifier() === 'file' ){
                                    var palettes = property.paletteList()
                                    for ( var pi = 0; pi < palettes.length; ++pi ){

                                        var palette = palettes[pi]
                                        if ( palette.name === 'FilePathPalette' ){
                                            var coords = palette.item.mapToItem(editor, 0, 0)
                                            var editorCoords = editorPane.mapGlobalPosition()
                                            editor.makePositionVisible(coords.y + editorCoords.y + 25 - 80)                            
                                            coords = palette.item.mapToItem(editor, 0, 0)
                                            highlight = createHighlight(state)

                                            highlight.boxX = coords.x + editorCoords.x - 15
                                            highlight.boxY = coords.y + editorCoords.y + 25
                                            highlight.box.width = 250
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
    }
}
