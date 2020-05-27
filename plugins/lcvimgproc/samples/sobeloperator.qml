import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0

Grid{
    
    // Sobel operator sample in x and y directions
    
    columns : 2
    
    id : root
    
    property string imagePath : project.dir() + '/../../../samples/_images/buildings_0246.jpg'
    
    ImRead{
        id : src
        file : root.imagePath
    }
    
    ChannelSelect{
        id : cs
        input : src.output
        channel : 2
        visible : false
    }
    
    Sobel{ // Sobel in y direction
        input : cs.output
        ddepth : Mat.CV16S
        width : src.width
        height : src.height
        xorder : 1
        yorder : 0
        ksize  : 3
        scale  : 1
        delta : 0
    }
    
    Sobel{ // Sobel in x direction
        input : cs.output
        ddepth : Mat.CV16S
        width : src.width
        height : src.height
        xorder : 0
        yorder : 1
        ksize  : 3
        scale  : 1
        delta : 0
    }
}
