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
        }
    ]

}
