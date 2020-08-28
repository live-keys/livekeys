import QtQuick 2.9
import editor 1.0
import editqml 1.0

WorkspaceControl{
    run: function(workspace){
        var editor = lk.layers.workspace.panes.focusPane('editor')
        var codeHandler = editor.documentHandler.codeHandler
        lk.layers.workspace.extensions.editqml.rootPosition = codeHandler.findRootPosition()

        lk.layers.workspace.extensions.editqml.shapeRootObject(editor, editor.documentHandler.codeHandler, function(){
            lk.layers.workspace.extensions.editqml.paletteControls.shapeAtPositionWithInstructions(
                editor, 
                81, 
                JSON.parse('
                
                {
                    "type": "Grid",
                    "children": [
                        {
                            "type": "ImageFile"
                        },
                        {
                            "type": "TransformImage",
                            "palettes": ["TransformPalette"],
                            "properties": [
                                {
                                    "name": "input"
                                }
                            ]
                        },
                        {
                            "type": "ImageView",
                            "properties": [
                                {
                                    "name": "image"
                                }
                            ]
                        }
                    ]
                }
                
                '))
        })
    }
}
