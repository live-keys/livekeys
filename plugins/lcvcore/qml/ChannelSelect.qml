import QtQuick 2.3
import base 1.0
import lcvcore 1.0 as Cv

Act{
    property Cv.Mat input: null
    property int channel: 0

    run: Cv.MatOp.selectChannel
    args: ["$input", "$channel"]
}
