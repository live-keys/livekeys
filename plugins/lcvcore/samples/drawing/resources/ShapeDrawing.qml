import QtQuick 2.9
import editor 1.0
import editqml 1.0
import workspace 1.0

WorkspaceControl{
    run: function(workspace){
        var editor = lk.layers.workspace.panes.focusPane('editor')
        var codeHandler = editor.code.language
        var rootDeclaration = codeHandler.rootDeclaration()
        lk.layers.workspace.extensions.editqml.shapeImports(editor, codeHandler)
        lk.layers.workspace.extensions.editqml.shapeRootObject(editor, editor.documentHandler.codeHandler, function(){
            lk.layers.workspace.extensions.editqml.paletteControls.shapeAtPositionWithInstructions(
                editor, 
                rootDeclaration.position(),
                {
                    "type": "qml/QtQuick#Grid",
                    "children": [
                        {
                            "type": "qml/lcvcore#BlankImage",
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
                            "type": "qml/lcvcore#DrawSurface",
                            "palettes": ["DrawPalette"],
                            "properties": [
                                {
                                    "name": "input",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "qml/lcvcore#Mat",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                }
                            ]
                        },
                        {
                            "type": "qml/lcvcore#ColorHistogramView",
                            "properties": [
                                {
                                    "name": "input",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "u/QtObject",
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
