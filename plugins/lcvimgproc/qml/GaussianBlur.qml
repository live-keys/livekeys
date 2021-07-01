import QtQuick 2.3
import lcvcore 1.0
import base 1.0
import lcvimgproc 1.0 as Img

Act{
    property Mat input: null
    property size size: "0x0"
    property double sigmaX: 0.0
    property double sigmaY: 0.0
    property int borderType: Img.FilteringOperations.BORDER_DEFAULT

    run: Img.FilteringOperations.gaussianBlur
    args: ["$input", "$size", "$sigmaX", "$sigmaY", "$borderType"]
}
