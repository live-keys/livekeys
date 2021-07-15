import QtQuick 2.0
import base 1.0
import lcvphoto 1.0

Act{
    id: root
    property var input: null
    property var times: []
    property var response: null
    property var mergeDebevec: CvMergeDebevec {}

    run: function(input, times, response){
        return mergeDebevec.process(input, times, response)
    }
    onComplete: exec()

    args: ["$input", "$times", "$response"]
}
