import QtQuick 2.3
import base 1.0
import lcvcore 1.0 as Cv

Act{
    property Cv.Mat input: null
    property int top: 0
    property int bottom: 0
    property int left: 0
    property int right: 0
    property int borderType: Cv.MatOp.BORDER_DEFAULT
    property color color: "white"

    run: Cv.MatOp.copyMakeBorder
    args: ["$input", "$top", "$bottom", "$left", "$right", "$borderType", "$color"]
}
