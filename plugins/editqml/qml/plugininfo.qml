import QtQuick 2.3
import editqml 1.0
import base 1.0

Container{

    PluginInfoExtractor{
        id: pie
        onPluginInfoReady: {
            console.log(data)
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

    Opening{
        onRun: {
            if ( script.argvTail.length === 1 ){
                timer.start()
                pie.importUri = script.argvTail[0]
            } else {
                vlog.e("Script requires one argument: plugininfo <name>");
                Qt.quit(1)
            }
        }
    }

}
