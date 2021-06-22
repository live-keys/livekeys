import QtQuick 2.3
import lcvcore 1.0
import base 1.0
import lcvimgproc 1.0 as Img

Act{
    property Mat input1: null
    property Mat input2: null
    property Mat mask: null

    run: function(input1, input2, mask){
        return Img.Transformations.blend(input1, input2, mask)
    }
    args: ["$input1", "$input2", "$mask"]
}
