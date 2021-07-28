import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0

Row{
    
    // This sample shows the usage of the Blur
    // element
    
    ImageRead{
       id : src
       file : project.dir() + '/../../../samples/_images/buildings_0246.jpg'
    }
    
    Blur{
        id: blur
        input : src.result
        anchor : Qt.point(3, 3)
        size : "5x5"
    }
    
    ImageView {
        image: blur.result
    }
}
