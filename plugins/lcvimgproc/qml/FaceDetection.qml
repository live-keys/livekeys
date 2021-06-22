import QtQuick 2.3
import lcvcore 1.0
import base 1.0
import lcvimgproc 1.0
import fs 1.0 as Fs

Act{
    id: faceDetection
    property var clasifier : CascadeClassifier{
        id: cc
        file: faceDetection.file
    }

    property double scaleFactor: 1.1
    property int minNeighbors: 3
    property int flags: 0
    property string file: 'haarcascade_frontalface_default.xml'
    property Mat input: null
    run: function(input){
        return cc.detectFaces(input)
    }
    args: ["$input"]

}
