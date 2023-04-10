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
                    "type": "qml/QtQuick#Item",
                    "children": [
                        {
                            "type": "qml/lcvcore#VideoSurfaceView",
                            "properties": [
                                {
                                    "name": "timeline",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "qml/timeline#Timeline",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                }
                            ]
                        },
                        {
                            "type": "qml/QtQuick#Rectangle",
                            "properties": [
                                {
                                    "name": "x",
                                    "palettes": ["ConnectionPalette"]
                                }
                                
                            ]
                        },
                        {
                            "type": "qml/timeline#KeyframeValue",
                            "properties": [
                                {
                                    "name": "track",
                                    "palettes": ["TextPalette"]
                                }
                            ]
                        },
                        {
                            "type": "qml/timeline#Timeline",
                            "palettes": ["TimelinePalette"],
                            "properties": [
                                {
                                    "name": "file",
                                    "palettes": ["FilePathPalette"]
                                }
                            ]
                        }
                    ]
                }
                
                )
        })
    }
}
