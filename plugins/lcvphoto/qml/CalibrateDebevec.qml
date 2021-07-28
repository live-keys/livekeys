import QtQuick 2.0
import base 1.0
import lcvphoto 1.0

Act{
    id: root
    property var input: null
    property var times: []
    property int samples: 70
    property real lambda: 10.0
    property bool random: true
    property var calibrateDebevec: CalibrateDebevecInternal {
        samples: root.samples
        lambda: root.lambda
        random: root.random
    }

    run: function(input, times){
        return calibrateDebevec.process(input, times)
    }
    onComplete: exec()

    args: ["$input", "$times"]
}
