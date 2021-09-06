import QtQuick 2.0
import base 1.0
import lcvcore 1.0 as Cv
import lcvimgproc 1.0 as Img

Act{

    property Cv.Mat image: null
    property Cv.Mat kernel: Cv.MatOp.createFromArray(
        [
            [0.272, 0.534, 0.131],
            [0.349, 0.686, 0.168],
            [0.393, 0.769, 0.189]
        ],
        Cv.Mat.CV32F
    )

    returns: "qml/object"
    run: Img.Geometry.transform
    args: ["$image", "$kernel"]
}
