import QtQuick 2.3
import lcvcore 1.0
import lcvphoto 1.0

Column{
    
    // This samples shows the usage of the stitcher component.
    // Images are loaded into a MatList, and once the list is loaded
    // it's redirected to the Stitcher.
    
    // This sample might not work in OpenCV versions lower than 3.3
    
    spacing: 5
    
    Row{
        spacing: 5
        
        ImRead{
            id: imageLeft
            file: project.path('../_images/stitch-left.jpg')
        }
        ImRead{
            id: imageMid
            file: project.path('../_images/stitch-mid.jpg')
        }
        ImRead{
            id: imageRight
            file: project.path('../_images/stitch-right.jpg')
        }
    }
    
    MatList{
        id: inputList
        Component.onCompleted: fromArray([imageLeft.output, imageMid.output, imageRight.output])
        onEntriesAdded: if ( size() === 3 ) stitcher.input = inputList
    }
    
    Stitcher{
        id: stitcher
    }
    
}
