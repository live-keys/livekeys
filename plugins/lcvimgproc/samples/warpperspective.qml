import QtQuick 2.3
import lcvcore 1.0 as Cv
import lcvimgproc 1.0

Grid{    
    spacing: 2
    columns: 1
    
    // This samples shows the usage of perspective transform
    // between 2 sets of 4 points

    Cv.ImRead{
        id: src
        file: project.path('../../../samples/_images/buildings_0246.jpg')
    }
    
    GetPerspectiveTransform{
        id: persTransform
        src.items: [
            Qt.point(0, 0), Qt.point(800, 0),
            Qt.point(512, 384), Qt.point(0, 384)
        ]
        dst.items: [
            Qt.point(0, 0), Qt.point(512, 0),
            Qt.point(600, 300), Qt.point(0, 384)
        ]
    }
    
    // Empty matrix to apply the transformation on
    
    Cv.MatView{
        id: bg
        visible: false
        mat : Cv.MatOp.createFill("600x400", Cv.Mat.CV8U, 3, "#000")
    }
    
    WarpPerspective{
        id: warpPerspective
        input: src.output
        output: bg.mat
        m: persTransform.output
    }
}
