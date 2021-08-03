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

        var editFragments = codeHandler.editFragments()

        for ( var i = 0; i < editFragments.length; ++i ){
            var itemEdit = editFragments[i]
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
                                        palette.value = project.path('../../../../samples/_videos/amherst-11_2754_3754.avi')
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
