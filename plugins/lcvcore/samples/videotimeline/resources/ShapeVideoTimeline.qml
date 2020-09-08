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
                            "type": "VideoSurfaceView",
                            "properties": [
                                {
                                    "name": "timeline",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "Timeline",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                }
                            ]
                        },
                        {
                            "type": "Rectangle",
                            "properties": [
                                {
                                    "name": "x",
                                    "palettes": ["ConnectionPalette"]
                                }
                                
                            ]
                        },
                        {
                            "type": "KeyframeValue",
                            "properties": [
                                {
                                    "name": "track",
                                    "palettes": ["TextPalette"]
                                }
                            ]
                        },
                        {
                            "type": "Timeline",
                            "palettes": ["TimelinePalette"],
                            "properties": [
                                {
                                    "name": "file",
                                    "palettes": ["PathPalette"]
                                }
                            ]
                        }
                    ]
                }
                
                )
        })
    }
}
