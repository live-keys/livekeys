import lcvcore 1.0
import lcvimgproc 1.0

Row{
    
    // This sample shows the usage of Hough Line 
    // transform to detect lines within an image
    
    id : root
    
    property string imagePath : codeDocument.path + '/../_images/buildings_0246.jpg'
    
    Rectangle{
        
        width : sc.width
        height : sc.height
        
        ImRead{
            id : sc
            file : root.imagePath
        }
        
        HoughLinesP{
            input : cny.output
            width : sc.width
            height : sc.height
            rho : 0.5
            theta : Math.PI / 180
            threshold : 50
            lineThickness : 2
            lineColor : "#00ff00"
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
