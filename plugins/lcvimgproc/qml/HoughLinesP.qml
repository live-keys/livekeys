import QtQuick 2.3
import lcvcore 1.0
import base 1.0
import lcvimgproc 1.0 as Img

Act{
    property Mat input: null
    property double rho: 1
    property double theta: 1
    property int threshold: 1
    property double minLineLength: 0.0
    property double maxLineGap: 0.0

    run: Img.FeatureDetection.houghLinesP
    args: ["$input", "$rho", "$theta", "$threshold", "$minLineLength", "$maxLineGap"]
}
