import QtQuick 2.3
import base 1.0
import lcvcore 1.0 as Cv

Act{
    property QtObject input: null
    property var points: null
    run: function(input, points){
        return Cv.MatOp.perspective(input, points)
    }
    args: ["$input", "$points"]
}
