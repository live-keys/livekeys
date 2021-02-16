import QtQuick 2.9
import editor 1.0
import editqml 1.0

WorkspaceControl{
    run: function(workspace){
        var editors = lk.layers.workspace.panes.findPanesByType('editor')
        vlog.i(editors)
        for ( var i = 0; i < editors.length; ++i ){
            var editor = editors[i]
            if ( editor.document.file.name === 'multiimage.qml' ){

                var codeHandler = editor.documentHandler.codeHandler
                var rootPosition = lk.layers.workspace.extensions.editqml.rootPosition = codeHandler.findRootPosition()
                lk.layers.workspace.extensions.editqml.shapeImports(editor, codeHandler)


            } else if ( editor.document.file.name === 'Operations.qml' ){

                var codeHandler = editor.documentHandler.codeHandler
                var rootPosition = lk.layers.workspace.extensions.editqml.rootPosition = codeHandler.findRootPosition()

                lk.layers.workspace.extensions.editqml.shapeImports(editor, codeHandler)
                lk.layers.workspace.extensions.editqml.shapeRootObject(editor, editor.documentHandler.codeHandler, function(){
                    lk.layers.workspace.extensions.editqml.paletteControls.shapeAtPositionWithInstructions(
                        editor,
                        rootPosition,

                        {
                            "type": "qml/QtQuick#Item",
                            "children": [
                                {
                                    "type": "qml/lcvcore#ImageFile",
                                    "properties": [
                                        {
                                            "name": "source",
                                            "palettes": ["ConnectionPalette"]
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
                                            "instructions" : {
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



    }
}
