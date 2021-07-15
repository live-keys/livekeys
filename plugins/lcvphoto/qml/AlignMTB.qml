import QtQuick 2.0
import base 1.0
import lcvphoto 1.0

Act{
    id: root
    property var input: null
    property int maxBits: 6
    property int excludeRange: 4
    property bool cut: true

    property var alignMtb: CvAlignMTB {
        maxBits: root.maxBits
        excludeRange: root.excludeRange
        cut: root.cut
    }

    run: function(input){
        return alignMtb.process(input)
    }
    onComplete: exec()

    args: ["$input"]
}
