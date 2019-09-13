import QtQuick 2.5
import editor 1.0
import live 1.0

Item{
    anchors.fill: parent
    
    // This sample shows how to create an editor that's
    // able to edit it's own code
    
    Editor{
        anchors.centerIn: parent
        document: project.openFile(project.path('editor.qml'))
        height: parent.height
        width: 400
        windowControls: lk.windowControls()
    }
}
