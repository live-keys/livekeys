import QtQuick 2.3
import lcvcore 1.0 as Cv
import lcvphoto 1.0
import base 1.0

Column{
    
    // This samples shows the usage of the stitcher component.
    // Images are loaded into a ObjectList, and once the list is loaded
    // it's redirected to the Stitcher.
    
    // This sample might not work in OpenCV versions lower than 3.3
    
    spacing: 5
    
    Row{
        Repeater{
            id: repeater
            model: ['stitch-left.jpg', 'stitch-mid.jpg', 'stitch-right.jpg'].map(
                function(e){ return project.path('../../../samples/_images/' + e);}).map(
                function(e){ return Cv.MatIO.read(e); }
            )
            
            Cv.MatView{
                mat : modelData
            }
        }
    }
    
    Stitcher{
        id: stitcher
        input: Cv.MatOp.createMatList(repeater.model)
    }

    Cv.ImageView {
        image: stitcher.result
    }
    
}
