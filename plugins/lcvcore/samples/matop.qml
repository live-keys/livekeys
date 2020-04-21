import QtQuick 2.3
import lcvcore 1.0 as Cv

Grid{
    
    spacing: 2
    
    Cv.MatView{
         mat: {
             var m = Cv.MatOp.create(Qt.size(100, 100), Cv.Mat.CV8U, 3)
             Cv.MatOp.fill(m, "#003333")
             return m
         }
    }
    
    Cv.MatView{
         mat: Cv.MatOp.createFill(Qt.size(100, 100), Cv.Mat.CV8U, 3, "#660000")
    }
    
    Cv.MatView{
         mat: Cv.MatOp.createFromArray([
             [0,   100, 0],
             [150, 0,   250],
             [0,   200, 0]
         ])
         linearFilter: false
         width: 100
         height: 100
    }
}