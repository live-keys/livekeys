import QtQuick 2.3
import lcvcore 1.0
import base 1.0
import lcvimgproc 1.0 as Img

Act{
    property Mat input: null
    property Mat kernel: null
    property var anchor: Qt.point(0, 0)
    property int iterations: 0

    run: Img.FilteringOperations.dilate
    args: ["$input", "$kernel", "$anchor", "$iterations"]
}
