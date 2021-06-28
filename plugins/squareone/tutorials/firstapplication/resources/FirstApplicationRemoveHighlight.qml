import QtQuick 2.9
import editor 1.0
import editqml 1.0
import workspace 1.0

WorkspaceControl{

    run: function(workspace, state){
        if ( state.currentHighlight ){
            for ( var i = 0; i < state.currentHighlight.length; ++i ){
                state.currentHighlight[i].destroy()
            }
            state.currentHighlight = null
        }
    }

}
