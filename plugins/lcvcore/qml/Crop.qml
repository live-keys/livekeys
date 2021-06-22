import QtQuick 2.3
import base 1.0
import lcvcore 1.0 as Cv

Act{
    property QtObject input: null
    property rect region: "0,0,0x0"

    run: function(input, region){
        return Cv.MatOp.crop(input, region)
    }
    args: ["$input", "$region"]
}
