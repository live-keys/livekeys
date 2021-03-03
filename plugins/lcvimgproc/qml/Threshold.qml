import QtQuick 2.3
import lcvcore 1.0
import base 1.0
import lcvimgproc 1.0 as Img

Act{
    property Mat input: null
    property real threshold : 95
    property real maxValue : 255
    property int type: Img.Transformations.BINARY

    run: function(input, threshold, maxValue, type){
        return Img.Transformations.threshold(input, threshold, maxValue, type)
    }
    args: ["$input", "$threshold", "$maxValue", "$type"]
}
