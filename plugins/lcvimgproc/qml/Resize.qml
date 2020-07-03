import QtQuick 2.3
import base 1.0
import lcvcore 1.0
import lcvimgproc 1.0 as Img

Act{
    property Mat input: null
    property size size: "0x0"
    property int interpolation: Img.Geometry.INTER_LINEAR
    run: function(input, size, interpolation){
        return Img.Geometry.resize(input, size, interpolation)
    }
    args: ["$input" , "$size", "$interpolation"]
}
