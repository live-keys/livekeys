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
                    "palettes": ["NodePalette"],
                    "children": [
                        {
                            "type": "FileReader",
                            "properties": [
                                {
                                    "name": "file",
                                    "palettes": ["PathPalette"]
                                }
                            ]
                        },
                        {
                            "type": "TextSearch",
                            "properties": [
                                {
                                    "name": "text",
                                    "palettes": ["ConnectionPalette"]
                                },
                                {
                                    "name": "value",
                                    "palettes": ["TextPalette"]
                                }
                            ]
                        },
                        {
                            "type": "IndexSelector",
                            "properties": [
                                {
                                    "name": "list",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "QJSValue",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                },
                                {
                                    "name": "index",
                                    "palettes": ["IntPalette"]
                                }
                            ]
                        },
                        {
                            "type": "ConvertToInt",
                            "properties": [
                                {
                                    "name": "input",
                                    "palettes": ["ConnectionPalette"]
                                }
                            ]
                        },
                        {
                            "type": "TextLineAtPosition",
                            "properties": [
                                {
                                    "name": "position",
                                    "palettes": ["ConnectionPalette"]
                                },
                                {
                                    "name": "text",
                                    "palettes": ["ConnectionPalette"]
                                }
                            ]
                        },
                        {
                            "type": "TextClip",
                            "properties": [
                                {
                                    "name": "text",
                                    "palettes": ["ConnectionPalette"]
                                },
                                {
                                    "name": "positions",
                                    "palettes": ["ConnectionPalette"]
                                }
                            ]
                        },
                        {
                            "type": "Text",
                            "properties": [
                                {
                                    "name": "text",
                                    "palettes": ["ConnectionPalette"]
                                }
                            ]
                        }
                    ]
                }
                
                )
        })
    }
}
