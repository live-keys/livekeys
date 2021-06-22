import QtQuick 2.3
import base 1.0
import lcvimgproc 1.0 as ImgProc

Act{
    property QtObject input: null
    property var points: null
    run: function(input, points){
        if (!input) return null
        var size = input.dimensions()
        var list = [Qt.point(0,0),
                    Qt.point(size.width,0),
                    Qt.point(size.width, size.height),
                    Qt.point(0, size.height)]
        var transform = ImgProc.Geometry.getPerspectiveTransform(list, points)

        return ImgProc.Geometry.warpPerspective(
                    input,
                    transform,
                    size,
                    ImgProc.Geometry.INTER_LINEAR,
                    ImgProc.Geometry.BORDER_CONSTANT)
    }
    args: ["$input", "$points"]
}
