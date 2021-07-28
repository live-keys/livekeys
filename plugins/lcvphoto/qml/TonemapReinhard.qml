import QtQuick 2.0
import base 1.0
import lcvphoto 1.0

Act{
    id: root
    property var input: null
    property real gamma: 1.0
    property real intensity: 0.0
    property real lightAdapt: 1.0
    property real colorAdapt: 0.0

    property var tonemapReinhard: CvTonemapReinhard {
        gamma: root.gamma
        intensity: root.intensity
        lightAdapt: root.lightAdapt
        colorAdapt: root.colorAdapt

    }

    run: function(input){
        return tonemapReinhard.process(input)
    }
    onComplete: exec()

    args: ["$input"]
}
