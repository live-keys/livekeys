import lcvcore 1.0
import lcvimgproc 1.0

Column{
    
    // This sample shows how to resize an image
    // matrix
    
    id : root
    
    property string imagePath : codeDocument.path + '/../_images/buildings_0246.jpg'
    
    Row{
        
        width : sc.width
        height : sc.height
        
        ImRead{
            id : sc
            file : root.imagePath
        }
        
        Resize{
            id : scrs
            input : sc.output
            fx : 0.25
            fy : 0.25
            
        }
    }
}
