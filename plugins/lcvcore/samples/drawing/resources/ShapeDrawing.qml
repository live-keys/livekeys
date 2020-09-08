import QtQuick 2.9
import editor 1.0
import editqml 1.0

WorkspaceControl{
    run: function(workspace){
        var editor = lk.layers.workspace.panes.focusPane('editor')
        var codeHandler = editor.documentHandler.codeHandler
        var rootPosition = lk.layers.workspace.extensions.editqml.rootPosition = codeHandler.findRootPosition()

        lk.layers.workspace.extensions.editqml.shapeRootObject(editor, editor.documentHandler.codeHandler, function(){
            lk.layers.workspace.extensions.editqml.paletteControls.shapeAtPositionWithInstructions(
                editor, 
                rootPosition, 
                {
                    "type": "Grid",
                    "children": [
                        {
                            "type": "BlankImage",
                            "properties": [
                                {
                                    "name": "size",
                                    "palettes": ["SizePalette"]
                                },
                                {
                                    "name": "fill",
                                    "palettes": ["ColorPalette"]
                                }
                            ]
                        },
                        {
                            "type": "Draw",
                            "palettes": ["DrawPalette"],
                            "properties": [
                                {
                                    "name": "input",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "Mat",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                }
                            ]
                        },
                        {
                            "type": "ColorHistogramView",
                            "properties": [
                                {
                                    "name": "input",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "QtObject",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                }
                            ]
                        }
                    ]
                }
                
                )
        })
    }
}
