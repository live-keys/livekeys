import QtQuick 2.3
import lcvcore 1.0
import base 1.0
import lcvimgproc 1.0 as Img

Act{
    property Mat input: null
    property size size: "0x0"
    property var anchor: Qt.point(0, 0)

    run: Img.FilteringOperations.blur
    args: ["$input", "$size", "$anchor"]
}
