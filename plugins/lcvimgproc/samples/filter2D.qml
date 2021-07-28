import QtQuick 2.3
import lcvcore 1.0 as Cv
import lcvimgproc 1.0

Grid{
    
    // Usage of the filter2D
    
    columns: 1
    
    Cv.ImageRead{
       id : src
       file : project.dir() + '/../../../samples/_images/buildings_0246.jpg'
    }
    
    Cv.MatView{
        id: kernel
        width : 200
        height : 200
        linearFilter : false
        mat : {
            return Cv.MatOp.createFromArray([
                [-0.1, 0.0, 0.4], 
                [-0.1, 0.0, 0.1], 
                [-0,1, 0.0, 0.1]], Cv.Mat.CV32F)
        }
        visible: false
    }
    
    Filter2D{
        id: f2d
        input : src.result
        kernel : kernel.mat
    }
    
    Cv.ImageView {
        image: f2d.result
    }
}
