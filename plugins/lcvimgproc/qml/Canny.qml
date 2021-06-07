import QtQuick 2.3
import lcvcore 1.0
import base 1.0
import lcvimgproc 1.0 as Img

Act{
    property Mat input: null
    property double threshold1: 0
    property double threshold2: 1

    run: Img.FilteringOperations.canny
    args: ["$input", "$threshold1", "$threshold2"]
}
