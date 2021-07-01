import QtQuick 2.3
import lcvcore 1.0
import base 1.0
import lcvimgproc 1.0 as Img

Act{
    property Mat input: null
    property double rho: 0.0
    property double theta: 0.0
    property int threshold: 0
    property double srn: 0.0
    property double stn: 0.0

    run: Img.FeatureDetection.houghLines
    args: ["$input", "$rho", "$theta", "$threshold", "$srn", "$stn"]
}
