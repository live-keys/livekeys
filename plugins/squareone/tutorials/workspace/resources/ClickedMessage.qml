import QtQuick 2.9
import editor 1.0
import editqml 1.0

WorkspaceControl{

    run: function(workspace){
        lk.layers.window.dialogs.message('This message box was opened by the link in the documentation.',
        {
            button2Name : 'Got it',
            button2Function : function(mbox){
                mbox.close()
            }
        })
    }
}
