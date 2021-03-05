import QtQuick 2.3
import base 1.0
import lcvcore 1.0 as Cv

Act{
    property int type: Cv.Mat.CV8U
    property size size: "300x300"
    property int channels: 3
    property color fill: 'black'
    property bool runOnComplete: true
    property QtObject reference: null
    onReferenceChanged: {
        if ( reference ){
            type = reference.depth()
            channels = reference.channels()
            size = reference.dimensions()
        }
    }

    run: function(size, type, channels, fill){
        return Cv.MatOp.createFill(size, type, channels, fill)
    }
    args: ["$size", "$type", "$channels", "$fill"]

    onComplete: {
        if ( runOnComplete )
            exec()
    }

}
