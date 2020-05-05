import QtQuick 2.3
import lcvcore 1.0 as Cv
import lcvimgproc 1.0 as Img

Grid{
    
    // This sample shows how to use a mask to create
    // an alpha channel for an image.
    
    columns : 2
    
    Cv.ImRead{
       id : src
       file : project.dir() + '/../../../samples/_images/buildings_0246.jpg'
    }
    
    Cv.MatView{
        id : mask
        mat : {
            var maskMat = Cv.MatOp.createWritableFill(
                src.output.dimensions(), Cv.Mat.CV8U, 1, "#000"
            )
            Img.Draw.circle(
                maskMat, 
                Qt.point( maskMat.dimensions().width / 2, maskMat.dimensions().height / 2),
                maskMat.dimensions().height / 4, 
                "#fff", -1);
            return maskMat.toMat()
        }
    }
    
    Cv.AlphaMerge{
        input : src.output
        mask : mask.mat
    }
    
}
