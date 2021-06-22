import QtQuick 2.3
import lcvcore 1.0
import base 1.0
import lcvimgproc 1.0 as Img

Act{
    property int shape: Img.FilteringOperations.MORPH_ELLIPSE
    property size size: "11x11"
    property point anchor: Qt.point(3, 5)

    run: Img.FilteringOperations.getStructuringElement
    args: ["$shape", "$size", "$anchor"]
    onComplete: exec()
}
