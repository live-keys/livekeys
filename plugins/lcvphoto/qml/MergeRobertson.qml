import QtQuick 2.0
import base 1.0
import lcvphoto 1.0

Act{
    id: root
    property var input: null
    property var times: []
    property var response: null
    property var mergeRobertson: MergeRobertsonInternal {}

    run: function(input, times, response){
        return mergeRobertson.process(input, times, response)
    }
    onComplete: exec()

    args: ["$input", "$times", "$response"]
}
