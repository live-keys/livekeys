import QtQuick 2.0
import base 1.0
import lcvphoto 1.0 as Photo

Act{
    id: root
    property var input: null
    property int mode: 0
    property bool tryUseGpu: true

    property var stitcher: Photo.CvStitcher {
        mode: root.mode
        tryUseGpu: root.tryUseGpu
    }

    run: function(input){
        return stitcher.stitch(input)
    }
    onComplete: exec()

    args: ["$input"]
}
