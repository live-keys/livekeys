import QtQuick 2.3
import workspace 1.0 as Workspace
import live 1.0
import editor 1.0
import lcvcore 1.0 as Cv
import paper 1.0 as Paper

CodePalette{
    id: palette
    type : "qml/lcvcore#PaperSurface"

    property QtObject theme: lk.layers.workspace.themes.current
    property QtObject paperSurface: null
    property Cv.Mat background: paperSurface ? paperSurface.background : null
    onBackgroundChanged: {
        if ( background ){
            paperEditor.background = backgroundView
        }
    }

    property Component backgroundView: Cv.ImageView{
        image: palette.background
    }

    item: Item{
        width: 600
        height: 400

        Paper.PaperEditor{
            id: paperEditor
            anchors.fill: parent
            paperWidth: paperSurface ? paperSurface.width : 0
            paperHeight: paperSurface ? paperSurface.height : 0
            Component.onCompleted : {
                paperEditor.paperCanvas.onPainted.connect(function(){
                    itemCapture.capture()
                })
            }
        }

        Cv.ItemCapture{
            id: itemCapture
            captureSource : paperEditor.paperCanvas
            onReady : { capture() }
            onResultChanged: {
                if ( paperSurface )
                    paperSurface.output = result
            }
        }
    }

    onInit: {
        paperSurface = value
    }

}


