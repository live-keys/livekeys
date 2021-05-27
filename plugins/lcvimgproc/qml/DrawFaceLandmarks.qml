import QtQuick 2.3
import lcvcore 1.0 as LcvCore
import base 1.0
import lcvimgproc 1.0 as ImgProc
import fs 1.0 as Fs

Act{
    id: drawDetectedFaces

    property var mat: null
    property var landmarks: null

    run: function(mat, landmarks){
        if (!mat || !landmarks) return null
        var writable = LcvCore.MatOp.createWritableFromMat(mat)
        for (var i=0; i < landmarks.length; ++i){
            var lmks = landmarks[i]
            for (var j = 0; j < lmks.length; ++j){
                ImgProc.Draw.circle(writable, lmks[j], 5, "red", 3)
            }
        }
        return writable
    }
    args: ["$mat", "$landmarks"]

}
