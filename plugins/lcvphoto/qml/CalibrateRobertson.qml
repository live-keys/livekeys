import QtQuick 2.0
import base 1.0
import lcvphoto 1.0

Act{
    id: root
    property var input: null
    property var times: []
    property int maxIter: 30
    property real threshold: 0.01
    property var calibrateRobertson: CalibrateRobertsonInternal {
        maxIter: root.maxIter
        threshold: root.threshold
    }

    run: function(input, times){
        return calibrateRobertson.process(input, times)
    }
    onComplete: exec()

    args: ["$input", "$times"]
}
