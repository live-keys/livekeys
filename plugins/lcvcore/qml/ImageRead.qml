import QtQuick 2.3
import base 1.0
import lcvcore 1.0 as Cv

Act{
    id: root
    property string file: ''
    property int isColor: Cv.MatIO.CV_LOAD_IMAGE_COLOR

    run: Cv.MatIO.read
    onComplete: exec()
    args: ["$file", "$isColor"]
}
