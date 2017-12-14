import QtQuick 2.5
import editor 1.0
import live 1.0

Item{
    anchors.fill: parent          // This sample shows how to create an editor focusing     // only on a fragment from the actual document. Notice     // the lineStartIndex and lineEndIndex parameters. You     // will notice the parameters are updated internally according     // to the document changes.
    
    FragmentEditor{
        anchors.left: parent.left
        document: DocumentFragment{             document: project.openFile(project.path('fragmenteditor.qml'))             lineStartIndex: 25             lineEndIndex: 31
        }
        height: 300
        width: 400
        windowControls: livecv.windowControls()
    }
    
    Rectangle{         anchors.bottom: parent.bottom
        width: 200         height: 100         color: '#889988' 
    }
}
