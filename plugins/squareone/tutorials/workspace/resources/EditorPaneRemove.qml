import QtQuick 2.9
import editor 1.0
import editqml 1.0
import workspace 1.0

WorkspaceControl{

    run: function(workspace){
        var editorPane = lk.layers.workspace.panes.focusPane('editor')
        lk.layers.workspace.panes.removePane(editorPane)
    }
}
