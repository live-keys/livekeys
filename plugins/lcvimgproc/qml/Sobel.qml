import QtQuick 2.3
import lcvcore 1.0
import base 1.0
import lcvimgproc 1.0 as Img

Act{
    property Mat input: null
    property int ddepth: 0
    property int xOrder: 0.0
    property int yOrder: 0.0

    run: Img.FilteringOperations.sobel
    args: ["$input", "$ddepth", "$xOrder", "$yOrder"]
}
