import QtQuick 2.3
import lcvcore 1.0
import base 1.0
import lcvimgproc 1.0 as Img

Act{
    property Mat src1: null
    property Mat src2: null
    property Mat mask: null

    run: function(src1, src2, mask){
        return Img.Transformations.blend(src1, src2, mask)
    }
    args: ["$src1", "$src2", "$mask"]
}
