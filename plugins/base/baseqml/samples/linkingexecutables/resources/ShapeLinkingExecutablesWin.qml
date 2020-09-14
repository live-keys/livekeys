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
                    "type": "Item",
                    "children": [
                        {
                            "type": "Exec",
                            "properties": [
                                {
                                    "name": "path",
                                    "palettes": ["PathPalette"]
                                },
                                {
                                    "name": "args",
                                    "palettes": ["StringListPalette"]
                                }
                            ]
                        },
                        {
                            "type": "Exec",
                            "properties": [
                                {
                                    "name": "input",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "Stream",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                },
                                {
                                    "name": "path",
                                    "palettes": ["PathPalette"]
                                },
                                {
                                    "name": "args",
                                    "palettes": ["StringListPalette"]
                                }
                            ]
                        },
                        {
                            "type": "StreamLog",
                            "properties": [
                                {
                                    "name": "stream",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "Stream",
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
