import QtQuick 2.3
import lcvcore 1.0 as LcvCore
import base 1.0
import lcvimgproc 1.0 as ImgProc
import fs 1.0 as Fs

Act{
    id: drawDetectedFaces

    property var mat: null
    property var faces: null

    run: function(mat, faces){
        if (!mat || !faces) return null
        var writable = LcvCore.MatOp.createWritableFromMat(mat)
        for (var i=0; i < faces.length; ++i){
            var face = faces[i]

            ImgProc.Draw.rectangle(
                writable,
                Qt.point(face.x, face.y),
                Qt.point(face.x+face.width, face.y + face.height),
                "lightblue")
        }
        return writable
    }
    args: ["$mat", "$faces"]

}
