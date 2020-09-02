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
                            "type": "ImageFile",
                            "properties": [
                                {
                                    "name": "source",
                                    "palettes": ["PathPalette"]
                                }
                            ]
                        },
                        {
                            "type": "Levels",
                            "palettes": ["LevelsPalette"],
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
                            "type": "HueSaturationLightness",
                            "palettes": ["HueSaturationLightnessPalette"],
                            "properties": [
                                {
                                    "name": "input",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "Mat",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                },
                                {
                                    "name": "hue",
                                    "palettes": ["IntPalette"]
                                },
                                {
                                    "name": "saturation",
                                    "palettes": ["IntPalette"]
                                },
                                {
                                    "name": "lightness",
                                    "palettes": ["IntPalette"]
                                }
                            ]
                        },
                        {
                            "type": "BrightnessAndContrast",
                            "palettes": ["BrightnessAndContrastPalette"],
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
                            "type": "ImageView",
                            "properties": [
                                {
                                    "name": "image",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "Mat",
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
