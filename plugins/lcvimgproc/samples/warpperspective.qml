import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0

Grid{    
    columns: 1
    

    ImageRead{
        id: src
        file: project.path('../../../samples/_images/buildings_0246.jpg')
    }
            
    Perspective{
        id: warpPerspective
        input: src.result
        points: [Qt.point(0,100), Qt.point(100, 150), Qt.point(100, 0), Qt.point(0,0)]
    }
    
    ImageView {
        image: warpPerspective.result
        
    }
}
