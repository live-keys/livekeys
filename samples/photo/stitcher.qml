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
        spacing: 5
        
        Cv.ImRead{
            id: imageLeft
            file: project.path('../_images/stitch-left.jpg')
            onOutputChanged: initInputList()
        }
        Cv.ImRead{
            id: imageMid
            file: project.path('../_images/stitch-mid.jpg')
            onOutputChanged: initInputList()

        }
        Cv.ImRead{
            id: imageRight
            file: project.path('../_images/stitch-right.jpg')
            onOutputChanged: initInputList()
        }
    }

    function initObjectList(){
        if ( imageLeft.output !== 0 &&  imageMid.output !== 0 && imageRight.output !== 0) {
            stitcher.input = Cv.MatOp.createMatList([imageLeft.output, imageMid.output, imageRight.output])
        }
    }
    
    Stitcher{
        id: stitcher
        input: null
    }
    
}
