import QtQuick 2.9
import editor 1.0
import editqml 1.0
import workspace 1.0

WorkspaceControl{

    property Component highlightFactory : Highlighter{}

    function createHighlight(state){
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
        if ( fragment === 'project-pane' ){
            var projectPane = lk.layers.workspace.panes.focusPane('projectFileSystem')
            if ( !projectPane )
                return

            highlight = createHighlight(state)
            var coords = projectPane.mapGlobalPosition()
            highlight.boxX = coords.x
            highlight.boxY = coords.y
            highlight.box.width = projectPane.width
            highlight.box.height = projectPane.height
            highlight.box.border.width = 5
            highlight.box.border.color = highlight.box.color
            highlight.box.color = 'transparent'

        } else if ( fragment === 'editor-pane' ){
            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var coords = editorPane.mapGlobalPosition()
            highlight = createHighlight(state)
            highlight.boxX = coords.x
            highlight.boxY = coords.y
            highlight.box.width = editorPane.width
            highlight.box.height = editorPane.height
            highlight.box.border.width = 5
            highlight.box.border.color = highlight.box.color
            highlight.box.color = 'transparent'

        } else if ( fragment === 'documentation-pane' ){
            var docPane = lk.layers.workspace.panes.focusPane('documentation')
            if ( !docPane )
                return

            var coords = docPane.mapGlobalPosition()
            highlight = createHighlight(state)
            highlight.boxX = coords.x
            highlight.boxY = coords.y
            highlight.box.width = docPane.width
            highlight.box.height = docPane.height
            highlight.box.border.width = 5
            highlight.box.border.color = highlight.box.color
            highlight.box.color = 'transparent'

        } else if ( fragment === 'view-pane' ){
            var viewerPane = lk.layers.workspace.panes.focusPane('viewer')
            if ( !viewerPane )
                return

            var coords = viewerPane.mapGlobalPosition()
            highlight = createHighlight(state)
            highlight.boxX = coords.x
            highlight.boxY = coords.y
            highlight.box.width = viewerPane.width
            highlight.box.height = viewerPane.height
            highlight.box.border.width = 5
            highlight.box.border.color = highlight.box.color
            highlight.box.color = 'transparent'

        } else if ( fragment === 'mode-button' ){
            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var coords = editorPane.mapGlobalPosition()
            highlight = createHighlight(state)
            highlight.boxX = coords.x + editorPane.width - 145
            highlight.boxY = coords.y - 5
            highlight.box.width = 40
            highlight.box.height = 40

        } else if ( fragment === 'imports-section' ){
            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor
            var codeHandler = editor.documentHandler.codeHandler
            var editingFragments = codeHandler.editingFragments()

            for ( var i = 0; i < editingFragments.length; ++i ){
                var importEdit = editingFragments[i]
                if ( importEdit.type() === 'qml/import' ){
                    var coords = importEdit.visualParent.mapToItem(editor, 0, 0)
                    var editorCoords = editorPane.mapGlobalPosition()
                    highlight = createHighlight(state)
                    highlight.boxX = coords.x + editorCoords.x
                    highlight.boxY = coords.y + editorCoords.y + 30
                    highlight.box.width = importEdit.visualParent.parent.width
                    highlight.box.height = importEdit.visualParent.parent.height
                }
            }


        } else if ( fragment === 'items-section' ){
            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor
            var codeHandler = editor.documentHandler.codeHandler

            var editingFragments = codeHandler.editingFragments()

            for ( var i = 0; i < editingFragments.length; ++i ){
                var itemEdit = editingFragments[i]
                if ( itemEdit.type() === 'qml/QtQuick#Item' ){
                    var objectContainer = itemEdit.visualParent.parent.parent.parent

                    var coords = objectContainer.mapToItem(editor, 0, 0)
                    var editorCoords = editorPane.mapGlobalPosition()
                    highlight = createHighlight(state)
                    highlight.boxX = coords.x + editorCoords.x
                    highlight.boxY = coords.y + editorCoords.y + 30
                    highlight.box.width = objectContainer.width + 5
                    highlight.box.height = objectContainer.height + 5
                }
            }

        } else if ( fragment === 'root-item' ){
            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor
            var codeHandler = editor.documentHandler.codeHandler

            var editingFragments = codeHandler.editingFragments()

            for ( var i = 0; i < editingFragments.length; ++i ){
                var itemEdit = editingFragments[i]
                if ( itemEdit.type() === 'qml/QtQuick#Item' ){
                    var objectContainer = itemEdit.visualParent.parent.parent.parent

                    var coords = objectContainer.mapToItem(editor, 0, 0)
                    var editorCoords = editorPane.mapGlobalPosition()
                    highlight = createHighlight(state)
                    highlight.boxX = coords.x + editorCoords.x + 20
                    highlight.boxY = coords.y + editorCoords.y + 28
                    highlight.box.width = 100
                    highlight.box.height = 35
                }
            }
        } else if ( fragment === 'item-options' ){
            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor
            var codeHandler = editor.documentHandler.codeHandler

            var editingFragments = codeHandler.editingFragments()

            for ( var i = 0; i < editingFragments.length; ++i ){
                var itemEdit = editingFragments[i]
                if ( itemEdit.type() === 'qml/QtQuick#Item' ){
                    var objectContainer = itemEdit.visualParent.parent.parent.parent

                    var coords = objectContainer.mapToItem(editor, 0, 0)
                    var editorCoords = editorPane.mapGlobalPosition()
                    highlight = createHighlight(state)
                    highlight.boxX = coords.x + editorCoords.x + objectContainer.width - 120
                    highlight.boxY = coords.y + editorCoords.y + 28
                    highlight.box.width = 130
                    highlight.box.height = 35
                }
            }
        } else if ( fragment === 'add-option' ){
            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor
            var codeHandler = editor.documentHandler.codeHandler

            var editingFragments = codeHandler.editingFragments()

            for ( var i = 0; i < editingFragments.length; ++i ){
                var itemEdit = editingFragments[i]
                if ( itemEdit.type() === 'qml/QtQuick#Item' ){
                    var objectContainer = itemEdit.visualParent.parent.parent.parent

                    var coords = objectContainer.mapToItem(editor, 0, 0)
                    var editorCoords = editorPane.mapGlobalPosition()
                    highlight = createHighlight(state)
                    highlight.boxX = coords.x + editorCoords.x + objectContainer.width - 38
                    highlight.boxY = coords.y + editorCoords.y + 28
                    highlight.box.width = 35
                    highlight.box.height = 35
                }
            }

        } else if ( fragment === 'add-option-object' ){
            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor

            var container = lk.layers.workspace.panes.container

            for ( var i = 0; i < container.children.length; ++i ){
                if ( container.children[i].objectName === 'editorBox' ){
                    if ( container.children[i].child.objectName === 'addQmlBox' ){
                        var addBox = container.children[i].child

                        var coords = addBox.mapToItem(container, 0, 0)

                        highlight = createHighlight(state)
                        highlight.boxX = coords.x + addBox.width - 140
                        highlight.boxY = coords.y + 35
                        highlight.box.width = 75
                        highlight.box.height = 30
                    }
                }
            }

        } else if  ( fragment === 'add-option-search' ){
            var editorPane = lk.layers.workspace.panes.focusPane('editor')
            if ( !editorPane )
                return

            var editor = editorPane.editor

            var container = lk.layers.workspace.panes.container

            for ( var i = 0; i < container.children.length; ++i ){
                if ( container.children[i].objectName === 'editorBox' ){
                    if ( container.children[i].child.objectName === 'addQmlBox' ){
                        var addBox = container.children[i].child

                        var coords = addBox.mapToItem(container, 0, 0)

                        highlight = createHighlight(state)
                        highlight.boxX = coords.x
                        highlight.boxY = coords.y + 90
                        highlight.box.width = addBox.width
                        highlight.box.height = 30
                    }
                }
            }

        } else if ( fragment === 'video-path' ){
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
                        var decoderEdit = childFragments[j]
                        if ( decoderEdit.type() === 'qml/lcvcore#VideoDecoderView' ){


                            var decoderChildren = decoderEdit.getChildFragments()

                            for ( var k = 0; k < decoderChildren.length; ++k ){
                                var fileProperty = decoderChildren[k]
                                if ( fileProperty.identifier() === 'file' ){

                                    var palettes = fileProperty.paletteList()
                                    for ( var pi = 0; pi < palettes.length; ++pi ){
                                        var palette = palettes[pi]
                                        if ( palette.name === 'PathPalette' ){
                                            var coords = palette.item.mapToItem(editor, 0, 0)
                                            var editorCoords = editorPane.mapGlobalPosition()

                                            highlight = createHighlight(state)
                                            highlight.boxX = coords.x + editorCoords.x + palette.item.width - 35
                                            highlight.boxY = coords.y + editorCoords.y + 30
                                            highlight.box.width = 35
                                            highlight.box.height = 35
                                        }
                                    }
                                }

                            }

                        }
                    }
                }
            }
        } else if ( fragment === 'grayscale-input' ){
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
                        var decoderEdit = childFragments[j]
                        if ( decoderEdit.type() === 'qml/lcvcore#GrayscaleView' ){

                            var decoderChildren = decoderEdit.getChildFragments()

                            for ( var k = 0; k < decoderChildren.length; ++k ){
                                var inputProperty = decoderChildren[k]
                                if ( inputProperty.identifier() === 'input' ){

                                    var palettes = inputProperty.paletteList()

                                    for ( var pi = 0; pi < palettes.length; ++pi ){
                                        var palette = palettes[pi]
                                        if ( palette.name === 'ConnectionPalette' ){
                                            var coords = palette.item.mapToItem(editor, 0, 0)
                                            var editorCoords = editorPane.mapGlobalPosition()

                                            highlight = createHighlight(state)
                                            highlight.boxX = coords.x + editorCoords.x
                                            highlight.boxY = coords.y + editorCoords.y + 30
                                            highlight.box.width = palette.item.width - 35
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
