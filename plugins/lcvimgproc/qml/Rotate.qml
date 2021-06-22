import QtQuick 2.3
import base 1.0
import lcvcore 1.0
import lcvimgproc 1.0 as Img

Act{
    property Mat input: null
    property double degrees: 0
    run: function(input, degrees){
        return Img.Geometry.rotate(input, degrees)
    }
    args: ["$input" , "$degrees"]
}
