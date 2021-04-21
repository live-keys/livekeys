import QtQuick 2.3
import base 1.0
import lcvimgproc 1.0 as ImgProc

Act{
    property QtObject input: null
    property QtObject background: null
    property var points: null
    run: function(input, background, points){
        return ImgProc.Geometry.perspectiveProjection(input, background, points)
    }
    args: ["$input", "$background", "$points"]
}
