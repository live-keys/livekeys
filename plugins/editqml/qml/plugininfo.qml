import QtQuick 2.3
import editqml 1.0
import base 1.0 as B

B.Container{

    PluginInfoExtractor{
        id: pie
        onPluginInfoReady: {
            printResult(data)
            Qt.quit(0)
        }
    }

    Timer{
        id: timer
        interval: 10000
        repeat: false
        onTriggered: {
            vlog.e("Script timed out.")
            Qt.quit(2)
        }
    }

    B.Act{
        onComplete: run()
        run: function(){
            if ( B.Script.argvTail.length === 1 ){
                timer.start()
                pie.importUri = B.Script.argvTail[0]
            } else {
                vlog.e("Script requires one argument: plugininfo <name>");
                Qt.quit(1)
            }
        }
    }

}
