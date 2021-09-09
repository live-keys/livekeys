import QtQuick 2.3
import base 1.0
import lcvcore 1.0 as Cv

Act{
    id: root
    property string file: ''
    property Cv.Mat image: null
    property var options: ({})

    run: Cv.MatIO.write
    args: ["$file", "$image", "$options"]
}
