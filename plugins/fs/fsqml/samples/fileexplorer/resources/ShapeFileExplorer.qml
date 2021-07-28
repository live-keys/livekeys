import QtQuick 2.9
import editor 1.0
import editqml 1.0
import workspace 1.0

WorkspaceControl{
    run: function(workspace){
        var editor = lk.layers.workspace.panes.focusPane('editor')
        var codeHandler = editor.documentHandler.codeHandler
        var rootPosition = lk.layers.workspace.extensions.editqml.rootPosition = codeHandler.findRootPosition()
        lk.layers.workspace.extensions.editqml.shapeImports(editor, codeHandler)
        lk.layers.workspace.extensions.editqml.shapeRootObject(editor, editor.documentHandler.codeHandler, function(){
            lk.layers.workspace.extensions.editqml.paletteControls.shapeAtPositionWithInstructions(
                editor, 
                rootPosition, 
                {
                    "type": "qml/QtQuick#Grid",
                    "children": [
                        {
                            "type": "qml/fs#VisualFileSelector",
                            "palettes": ["VisualFileSelectorPalette"]
                        },
                        {
                            "type": "qml/fs#ExtensionPass",
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
                            "type": "qml/lcvcore#ImageRead",
                            "properties": [
                                {
                                    "name": "file",
                                    "palettes": ["ConnectionPalette"]
                                }
                            ]
                        },
                        {
                            "type": "qml/lcvcore#NavigableImageView",
                            "properties": [
                                {
                                    "name": "image",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "qml/lcvcore#Mat",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                }
                            ]
                        },
                        {
                            "type": "qml/lcvphoto#Sepia",
                            "properties": [
                                {
                                    "name": "image",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "qml/lcvcore#Mat",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                },
                            ]
                        },
                        {
                            "type": "qml/lcvphoto#Temperature",
                            "properties": [
                                {
                                    "name": "image",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "qml/lcvcore#Mat",
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
                            "type": "qml/lcvphoto#Temperature",
                            "properties": [
                                {
                                    "name": "image",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "qml/lcvcore#Mat",
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
                            "type": "qml/lcvcore#NavigableImageView",
                            "properties": [
                                {
                                    "name": "image",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "qml/lcvcore#Mat",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                }
                            ]
                        },
                        {
                            "type": "qml/lcvcore#NavigableImageView",
                            "properties": [
                                {
                                    "name": "image",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "qml/lcvcore#Mat",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                }
                            ]
                        },
                        {
                            "type": "qml/lcvcore#NavigableImageView",
                            "properties": [
                                {
                                    "name": "image",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "qml/lcvcore#Mat",
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
