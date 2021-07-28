import QtQuick 2.0
import base 1.0
import lcvphoto 1.0

Act{
    id: root
    property var input: null
    property real gamma: 1.0
    property real saturation: 1.0
    property real bias: 0.85

    property var tonemapDrago: TonemapDragoInternal {
        gamma: root.gamma
        saturation: root.saturation
        bias: root.bias
    }

    run: function(input){
        return tonemapDrago.process(input)
    }
    onComplete: exec()

    args: ["$input"]
}
