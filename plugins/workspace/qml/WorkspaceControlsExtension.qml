import QtQuick 2.3
import editor 1.0
import workspace.nodeeditor 1.0

WorkspaceExtension{
    objectName: "workspace"

    property ObjectGraphControls objectGraphControls: ObjectGraphControls{}

    commands: {
        'minimize' : [lk.layers.window.handle.minimize, "Minimize"],
        'toggleFullScreen': [lk.layers.window.handle.toggleFullScreen, "Toggle Fullscreen"],
        "node_delete_active": [objectGraphControls.removeActiveItem, "Deletes the activated item in the node editor."],
        "nodeEditMode" : [objectGraphControls.nodeEditMode, "Switch to node editing mode."]
    }

    // quick node editing

    keyBindings: {
        "backspace": {command: "workspace.node_delete_active", whenPane: "editor", whenItem:"objectGraph" },
        "delete": {command: "workspace.node_delete_active", whenPane: "editor", whenItem:"objectGraph" },
        "alt+n": {command: "workspace.nodeEditMode" }
    }

    menuInterceptors: [
        {
            whenPane: 'editor',
            whenItem: 'editorType',
            intercept: function(pane, item){
                return [
                    {
                        name : "Cut",
                        action : item.textEdit.cut,
                        shortcut: 'ctrl+x',
                        enabled: item.textEdit.selectionStart !== item.textEdit.selectionEnd
                    }, {
                        name : "Copy",
                        action : item.textEdit.copy,
                        shortcut: 'ctrl+c',
                        enabled: item.textEdit.selectionStart !== item.textEdit.selectionEnd
                    }, {
                        name : "Paste",
                        action : item.textEdit.paste,
                        enabled : item.textEdit.canPaste,
                        shortcut: 'ctrl+v'
                    }
                ]
            }
        },
        {
            whenPane: 'log',
            whenItem: 'logList',
            intercept: function(pane, item){
                return [
                    {
                        name : "Copy",
                        action : pane.copySelection,
                        shortcut: 'ctrl+c',
                        enabled: pane.selection.length
                    }, {
                        name : "Open Location",
                        action : pane.openSelectionLocation,
                        shortcut: 'ctrl+l',
                        enabled: pane.selection.length
                    }, {
                        name : "Open Location Externally",
                        action : pane.openSelectionLocationExternally,
                        enabled : pane.selection.length
                    }, {
                        name : "Clear Selection",
                        action : pane.clearSelection,
                        enabled : pane.selection.length
                    }
                ]
            }
        },
        {
            whenPane: 'projectFileSystem',
            intercept: function(pane, item){
                if ( item.entry().isFile ){
                    return [
                        {
                            name : "Edit File",
                            action : item.openFile
                        }, {
                            name : "Monitor file",
                            action : item.monitorFile
                        }, {
                            name : "Set As Active",
                            action : item.setActive
                        }, {
                            name : "Add As Runnable",
                            action : item.addRunnable
                        }, {
                            name : "Rename",
                            action : function(){
                                item.editMode = true
                                item.focusText()
                            }
                        }, {
                            name : "Delete",
                            action : function(){
                                pane.removeEntry(item.entry(), false)
                            }
                        }
                    ]
                } else if ( item.path() === project.rootPath ){
                    return [
                        {
                            name : "Show in Explorer",
                            action : item.openExternally
                        }, {
                            name : "Close Project",
                            action : function(){
                                lk.layers.workspace.commands.execute('window.workspace.project.close')
                            }
                        }, {
                            name : "New Document",
                            action : function(){
                                var fe = project.fileModel.addTemporaryFile()
                                lk.layers.workspace.wizards.openFile(fe.path, ProjectDocument.Edit)
                            }
                        }, {
                            name : "New Runnable",
                            action : function(){
                                var fe = project.fileModel.addTemporaryFile()
                                lk.layers.workspace.wizards.openFile(fe.path, ProjectDocument.Edit)
                                project.openRunnable(fe.path, [fe.path])
                            }
                        }, {
                            name : "Add File",
                            action : function(){
                                lk.layers.workspace.wizards.addFile(item.entry().path, {}, function(f){
                                    lk.layers.workspace.wizards.openFile(f.path, ProjectDocument.Edit)
                                })
                            }
                        }, {
                            name : "Add Directory",
                            action : function(){
                                lk.layers.workspace.wizards.addDirectory(item.entry().path, {}, function(d){})
                            }
                        }
                    ]
                } else {
                    return [
                        {
                            name : "Show in Explorer",
                            action : item.openExternally
                        },
                        {
                            name : "Rename",
                            action : function(){
                                item.editMode = true
                                item.focusText()
                            }
                        },
                        {
                            name : "Delete",
                            action : function(){
                                pane.removeEntry(item.entry(), true)
                            }
                        }, {
                            name : "Add File",
                            action : function(){
                                lk.layers.workspace.wizards.addFile(item.entry().path, function(f){
                                    lk.layers.workspace.wizards.openFile(f.path, ProjectDocument.Edit)
                                })
                            }
                        }, {
                            name : "Add Directory",
                            action : function(){
                                lk.layers.workspace.wizards.addDirectory(item.entry().path, function(d){})
                            }
                        }
                    ]
                }
            }
        }
    ]

}
