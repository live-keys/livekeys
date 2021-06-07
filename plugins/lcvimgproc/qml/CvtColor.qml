import QtQuick 2.3
import lcvcore 1.0
import base 1.0
import lcvimgproc 1.0 as Img

Act{
    property Mat input: null
    property int code: 0
    property int dstCn: 0
    run: Img.ColorSpace.cvtColor
    args: ["$input", "$code", "$dstCn"]
}
