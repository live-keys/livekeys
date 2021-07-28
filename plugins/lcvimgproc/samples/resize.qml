import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0

Column{
    
    // This sample shows how to resize an image
    // matrix
    
    id : root
    
    property string imagePath : project.dir() + '/../../../samples/_images/buildings_0246.jpg'
    
    Row{
        
        width : result.width
        height : result.height
        
        ImageRead{
            id : sc
            file : root.imagePath
        }
        
        Resize{
            
            id : scrs
            input : sc.result
            size: "400x320"        
        }
        
        ImageView {
            id: result
            image: scrs.result
        }
    }
}
