import QtQuick 2.9
import editor 1.0
import editqml 1.0
import base 1.0 as B
import workspace 1.0

WorkspaceControl{

    run: function(workspace){
        var fe = lk.layers.workspace.panes.focusPane('editor')

        if ( B.Script.environment.os.platform === 'win32' ){
            var winPath = project.path('linkingexecutables_win.qml')
            var document = project.openFile(winPath)

            fe.document = document
            project.setActive(winPath)
        }

        lk.layers.workspace.commands.execute("editqml.shape_all")
    }
}
