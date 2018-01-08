import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0

Row{
    
    // Scan values and set custom pixels
    
    property string imagePath : project.dir() + '/../_images/buildings_0246.jpg'
            
    ImRead{
        id : sc      
        visible: false
        file : parent.imagePath
    }
    PanAndZoom{
        input: sc.output
    }
}
