import QtQuick 2.3
import lcvcore 1.0 as Cv
import lcvimgproc 1.0 as Img


// This example shows how to use the drawing functions 
// made available by the MatDraw element.

Rectangle{
    
    Cv.MatView{
        mat: {
            var m = Cv.MatOp.createWritableFill( Qt.size(400, 400), Cv.Mat.CV8U, 4, "#aa3311" )
            Img.Draw.line( m, Qt.point(10, 10) , Qt.point(100, 200), "#fff", 1, 8, 0 )
            Img.Draw.fillPoly(m, [
                [Qt.point(10, 10), Qt.point(20, 20), Qt.point(300, 20)],
                [Qt.point(20, 20), Qt.point(100, 100), Qt.point(10, 300)]
            ], "#fff" )
            Img.Draw.circle( m, Qt.point(150, 150), 50, "#3333cc", -1)
            Img.Draw.rectangle( m, Qt.point(110, 220), Qt.point(200, 300), "#33aa33", -1)
            return m.toMat()
        }
    }
    
}
