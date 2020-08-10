import QtQuick 2.9
import editor 1.0
import editqml 1.0

WorkspaceControl{

    run: function(workspace){
        lk.layers.workspace.commands.execute("editqml.shape_all")
    }
}
