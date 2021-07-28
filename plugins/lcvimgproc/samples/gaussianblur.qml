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
    
    GaussianBlur{
        id: gaussianBlur
        input : src.result
        size : "21x21"
        sigmaX : 5
        sigmaY : 5
    }
    
    ImageView {
        image: gaussianBlur.result
    }
}
