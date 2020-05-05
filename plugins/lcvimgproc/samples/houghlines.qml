import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0

Row{
    
    // This sample shows the usage of Hough Line 
    // transform to detect lines within an image
    
    id : root
    
    property string imagePath : project.dir() + '/../../../samples/_images/buildings_0246.jpg'
    
    Rectangle{
        
        width : sc.width
        height : sc.height
        
        ImRead{
            id : sc
            file : root.imagePath
        }
        
        
        HoughLines{
            input : cny.output
            width : sc.width
            height : sc.height
            rho : 0.5
            theta : Math.PI / 180
            threshold : 50
        }
    }
        
    Canny{
        id : cny
        input : sc.output
        width : sc.width
        height : sc.height
        threshold1 : 70
        threshold2 : 210
        apertureSize : 3
    }
}
