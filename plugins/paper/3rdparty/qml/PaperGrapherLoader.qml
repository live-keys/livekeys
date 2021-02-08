import QtQuick 2.3
import paper 1.0
import 'papergraph.js' as PaperGraph

Item{
    id: root

    property PaperCanvas paperCanvas: null

    property string paperGraphPath:':'
    property string paperGraphLibPath: paperGraphPath + '/lib'

    property var paperGrapher: null
    signal paperGrapherReady(var pg)

    property QtObject style : PaperGrapherConfig{}

    function __loadPaperGrapher(){
        var pg = new PaperGraph.Pg(paperCanvas.paper)
        pg.document.setup()
        pg.layer.setup()
        pg.undo.setup()

        root.paperGrapher = pg
        root.paperGrapherReady(pg)
    }

    Component.onCompleted: {
        if ( !paperCanvas ){
            throw new Error("PaperGrapher: Paper Canvas not defined.")
        }

        if ( paperCanvas.paper ){
            __loadPaperGrapher()
        } else {
            paperCanvas.paperReady.connect(__loadPaperGrapher)
        }

    }

}
