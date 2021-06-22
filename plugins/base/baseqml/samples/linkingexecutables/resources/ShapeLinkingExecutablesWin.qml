import QtQuick 2.9
import editor 1.0
import editqml 1.0
import base 1.0 as B

WorkspaceControl{
    run: function(workspace){
        var editor = lk.layers.workspace.panes.focusPane('editor')    
        
        if ( B.Script.environment.os.platform === 'win32' ){
            var winPath = project.path('linkingexecutables_win.qml')
            var document = project.openTextFile(winPath)
            editor.document = document
            project.setActive(winPath)
        }
         
        var codeHandler = editor.documentHandler.codeHandler
        var rootPosition = lk.layers.workspace.extensions.editqml.rootPosition = codeHandler.findRootPosition()
        lk.layers.workspace.extensions.editqml.shapeImports(editor, codeHandler)
        lk.layers.workspace.extensions.editqml.shapeRootObject(editor, editor.documentHandler.codeHandler, function(){
            lk.layers.workspace.extensions.editqml.paletteControls.shapeAtPositionWithInstructions(
                editor,
                rootPosition,
                {
                    "type": "qml/QtQuick#Item",
                    "palettes": ["NodePalette"],
                    "children": [
                        {
                            "type": "qml/base#Exec",
                            "properties": [
                                {
                                    "name": "path",
                                    "palettes": ["FilePathPalette"]
                                },
                                {
                                    "name": "args",
                                    "palettes": ["StringListPalette"]
                                }
                            ]
                        },
                        {
                            "type": "qml/base#Exec",
                            "properties": [
                                {
                                    "name": "input",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "qml/base#Stream",
                                        "palettes": ["ConnectionPalette"]
                                    }
                                },
                                {
                                    "name": "path",
                                    "palettes": ["FilePathPalette"]
                                },
                                {
                                    "name": "args",
                                    "palettes": ["StringListPalette"]
                                }
                            ]
                        },
                        {
                            "type": "qml/base#StreamLog",
                            "properties": [
                                {
                                    "name": "stream",
                                    "isAnObject": true,
                                    "instructions": {
                                        "type": "qml/base#Stream",
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
