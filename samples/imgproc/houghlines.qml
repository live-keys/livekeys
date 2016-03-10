import lcvcore 1.0
import lcvimgproc 1.0

Row{
    
    // This sample shows the usage of Hough Line 
    // transform to detect lines within an image
    
    id : root
    
    property string imagePath : codeDocument.path + '/../_images/caltech_buildings_DSCN0246_small.JPG'
    
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
