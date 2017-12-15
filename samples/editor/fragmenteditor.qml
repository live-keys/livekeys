import QtQuick 2.5
import editor 1.0
import live 1.0

Item{
    anchors.fill: parent          // This sample shows how to create an editor focusing     // only on a fragment from the actual document. Notice     // the lineStartIndex and lineEndIndex parameters. You     // will notice the parameters are updated internally according     // to the document changes.
         StaticLoader{         id: staticLineIndex         source: Item{ property int start: 39; property int end: 46; }         Component.onCompleted: {             staticLoad('statcLineIndex')             fragmentEditor.document = fragmentSelection
        }
    }          DocumentFragment{         id: fragmentSelection
        document: project.openFile(project.path('fragmenteditor.qml'))
        lineStartIndex: staticLineIndex.item.start         onLineStartIndexChanged: staticLineIndex.item.start = lineStartIndex
        lineEndIndex: staticLineIndex.item.end         onLineEndIndexChanged: staticLineIndex.item.end = lineEndIndex
    }     
    FragmentEditor{         id: fragmentEditor
        anchors.left: parent.left
        height: 300
        width: 400
        windowControls: livecv.windowControls()
    }
    
    Rectangle{         anchors.bottom: parent.bottom         anchors.bottomMargin: 10
        width: 200         height: 100         color: '#889988' 
    }
}
