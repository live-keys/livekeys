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
            
                var editormulti = editor

                var chmulti = editormulti.documentHandler.codeHandler
                var rpmulti = lk.layers.workspace.extensions.editqml.rootPosition = chmulti.findRootPosition()

                lk.layers.workspace.extensions.editqml.shapeRootObject(editormulti, chmulti, function(){
                    lk.layers.workspace.extensions.editqml.paletteControls.shapeAtPositionWithInstructions(
                        editormulti,
                        rpmulti,
                        {
                            "shapeImports": true,
                            "type": "qml/QtQuick#ListView",
                            "properties": [
                                {
                                    "name": "model",
                                    "palettes": ["ConnectionPalette"]
                                },
                                {
                                    "name": "delegate",
                                    "isAnObject": true
                                }
                            ]
                        }

                    )
                    
                })
            } else if ( editor.document.file.name === 'Operations.qml' ){

                var editorop = editor

                var chop = editorop.documentHandler.codeHandler
                var rpop = lk.layers.workspace.extensions.editqml.rootPosition = chop.findRootPosition()

                lk.layers.workspace.extensions.editqml.shapeRootObject(editorop, chop, function(){
                    lk.layers.workspace.extensions.editqml.paletteControls.shapeAtPositionWithInstructions(
                        editorop,
                        rpop,

                        {
                            "shapeImports": true,
                            "type": "qml/QtQuick#Item",
                            "properties": [
                                {
                                    "name": "width",
                                    "palettes": ["DoublePalette"]
                                },
                                {
                                    "name": "height",
                                    "palettes": ["DoublePalette"]
                                }
                            ],
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
