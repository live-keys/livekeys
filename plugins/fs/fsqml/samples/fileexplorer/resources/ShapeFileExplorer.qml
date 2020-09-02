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
                    "properties": [
                        {
                            "name": "columns",
                            "palettes": ["IntPalette"]
                        }
                    ],
                    "children": [
                        {
                            "type": "VisualFileSelector",
                            "palettes": ["VisualFileSelectorPalette"]
                        },
                        {
                            "type": "ExtensionPass",
                            "properties": [
                                {
                                    "name": "file",
                                    "palettes": ["ConnectionPalette"]
                                },
                                {
                                    "name": "extensions",
                                    "palettes": ["StringListPalette"]
                                }
                            ]
                        },
                        {
                            "type": "ImageRead",
                            "properties": [
                                {
                                    "name": "file",
                                    "palettes": ["ConnectionPalette"]
                                }
                            ]
                        },
                        {
                            "type": "NavigableImageView",
                            "properties": [
                                {
                                    "name": "image",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "Mat",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                },
                                {
                                    "name": "maxWidth",
                                    "palettes": ["DoublePalette"]
                                },
                                {
                                    "name": "maxHeight",
                                    "palettes": ["DoublePalette"]
                                }
                            ]
                        },
                        {
                            "type": "Sepia",
                            "properties": [
                                {
                                    "name": "image",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "Mat",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                },
                            ]
                        },
                        {
                            "type": "Temperature",
                            "properties": [
                                {
                                    "name": "image",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "Mat",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                },
                                {
                                    "name": "value",
                                    "palettes": ["DoublePalette"]
                                }
                            ]
                        },
                        {
                            "type": "Temperature",
                            "properties": [
                                {
                                    "name": "image",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "Mat",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                },
                                {
                                    "name": "value",
                                    "palettes": ["DoublePalette"]
                                }
                            ]
                        },
                        {
                            "type": "NavigableImageView",
                            "properties": [
                                {
                                    "name": "image",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "Mat",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                },
                                {
                                    "name": "maxWidth",
                                    "palettes": ["DoublePalette"]
                                },
                                {
                                    "name": "maxHeight",
                                    "palettes": ["DoublePalette"]
                                }
                            ]
                        },
                        {
                            "type": "NavigableImageView",
                            "properties": [
                                {
                                    "name": "image",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "Mat",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                },
                                {
                                    "name": "maxWidth",
                                    "palettes": ["DoublePalette"]
                                },
                                {
                                    "name": "maxHeight",
                                    "palettes": ["DoublePalette"]
                                }
                            ]
                        },
                        {
                            "type": "NavigableImageView",
                            "properties": [
                                {
                                    "name": "image",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "Mat",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                },
                                {
                                    "name": "maxWidth",
                                    "palettes": ["DoublePalette"]
                                },
                                {
                                    "name": "maxHeight",
                                    "palettes": ["DoublePalette"]
                                }
                            ]
                        }
                    ]
                }
                
                )
        })
    }
}
