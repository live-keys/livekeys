import QtQuick 2.3
import lcvcore 1.0
import base 1.0
import lcvimgproc 1.0 as Img

Act{
    property Mat input: null // bug

    property int shape: 2
    property size size: "11x11"
    property point anchor: Qt.point(3, 5)

    run: Img.FilteringOperations.getStructuringElement
    args: ["$shape", "$size", "$anchor"]
}
