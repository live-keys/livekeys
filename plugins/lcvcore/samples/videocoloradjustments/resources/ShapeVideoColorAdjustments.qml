import QtQuick 2.9
import editor 1.0
import editqml 1.0

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
                            "type": "qml/lcvcore#VideoDecoderView",
                            "properties": [
                                {
                                    "name": "file",
                                    "palettes": ["FilePathPalette"]
                                }
                            ]
                        },
                        {
                            "type": "qml/lcvphoto#Levels",
                            "palettes": ["LevelsPalette"],
                            "properties": [
                                {
                                    "name": "input",
                                    "isAnObject": true,
                                    "instructions" : {
                                        "type": "qml/lcvcore#Mat",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                }
                            ]
                        },
                        {
                            "type": "qml/lcvphoto#HueSaturationLightness",
                            "palettes": ["HueSaturationLightnessPalette"],
                            "properties": [
                                {
                                    "name": "input",
                                    "isAnObject": true,
                                    "instructions" : {
                                        "type": "qml/lcvcore#Mat",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                }
                            ]
                        },
                        {
                            "type": "qml/lcvphoto#BrightnessAndContrast",
                            "palettes": ["BrightnessAndContrastPalette"],
                            "properties": [
                                {
                                    "name": "input",
                                    "isAnObject": true,
                                    "instructions" : {
                                        "type": "qml/lcvcore#Mat",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                }
                            ]
                        },
                        {
                            "type": "qml/lcvcore#ImageView",
                            "properties": [
                                {
                                    "name": "image",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "qml/lcvcore#Mat",
                                        "palettes": ["ImagePalette"]
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
