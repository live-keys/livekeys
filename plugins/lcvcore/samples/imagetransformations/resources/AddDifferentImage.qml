import QtQuick 2.9
import editor 1.0
import editqml 1.0
import workspace 1.0

WorkspaceControl{

    run: function(workspace){
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
                    var imageFile = childFragments[j]
                    if ( imageFile.type() === 'qml/lcvcore#ImageFile' ){


                        var ifChildren = imageFile.getChildFragments()

                        for ( var k = 0; k < ifChildren.length; ++k ){
                            var sourceProperty = ifChildren[k]
                            if ( sourceProperty.identifier() === 'source' ){

                                var palettes = sourceProperty.paletteList()
                                for ( var pi = 0; pi < palettes.length; ++pi ){
                                    var palette = palettes[pi]
                                    if ( palette.name === 'FilePathPalette' ){
                                        palette.value = project.path('../../../../samples/_images/buildings_0246.jpg')
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
