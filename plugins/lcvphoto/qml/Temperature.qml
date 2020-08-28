import QtQuick 2.3
import base 1.0
import lcvcore 1.0 as Cv
import lcvimgproc 1.0 as Img

Act{

    property Cv.Mat image: null
    property double value: 0

    run: function(image, value){
        if ( !image )
            return null

        var channels = Cv.MatOp.split(image)
        if ( !channels.length )
            return null

        var normalized = value * 32 - 16

        var redSpread  = [0, 64 + normalized, 128 + normalized, 255]
        var blueSpread = [0, 64 - normalized, 128 - normalized, 255]

        var redLut = Cv.MatOp.spreadByLinearInterpolation([0, 64, 128, 255], redSpread)
        channels[2] = Cv.MatOp.lut(channels[2], redLut)

        var blueLut = Cv.MatOp.spreadByLinearInterpolation([0, 64, 128, 255], blueSpread)
        channels[0] = Cv.MatOp.lut(channels[0], blueLut)

        var result = Cv.MatOp.merge(channels)
        return result
    }

    onComplete: exec()

    args: ["$image", "$value"]
}

