import QtQuick 2.5
import editor 1.0
import live 1.0

Item{
    anchors.fill: parent
    
    // This sample shows how to create an editor focusing
    // only on a fragment from the actual document. Notice 
    // the lineStartIndex and lineEndIndex parameters. You
    // will notice the parameters are updated internally according
    // to the document changes.
    
    StaticLoader{
        id: staticLineIndex
        source: Item{ property int start: 39; property int end: 46; }
        Component.onCompleted: {
            staticLoad('staticLineIndex')
            fragmentEditor.document = project.openFile(project.path('fragmenteditor.qml'))
        }
    }

    Editor{
        id: fragmentEditor
        anchors.left: parent.left
        fragmentStart: 34
        fragmentEnd: 40
        height: 500
        width: 600
        textEdit.viewport: Qt.rect(0,0,600, 500)
        windowControls: lk.windowControls()
    }
    
    Rectangle{
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        width: 200
        height: 100
        color: '#889988'
    }
}
