import QtQuick 2.0
import base 1.0
import lcvphoto 1.0

Act{
    id: root
    property var input: null
    property real gamma: 1.0
    property real saturation: 1.0
    property real scale: 0.7

    property var tonemapMantiuk: TonemapMantiukInternal {
        gamma: root.gamma
        saturation: root.saturation
        scale: root.scale
    }

    run: function(input){
        return tonemapMantiuk.process(input)
    }
    onComplete: exec()

    args: ["$input"]
}
