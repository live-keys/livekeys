import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0

Grid{
    
    // Sobel operator sample in x and y directions
    
    columns : 1
    
    id : root
    
    property string imagePath : project.dir() + '/../../../samples/_images/buildings_0246.jpg'
    
    ImageRead{
        id : src
        file : root.imagePath
    }
    
    ChannelSelect{
        id : cs
        input : src.result
        channel : 2
    }
    
    Sobel{ // Sobel in y direction
        id: s1
        input : cs.result
        ddepth : Mat.CV16S
        xOrder : 1
        yOrder : 0
    }
    
    Sobel{ // Sobel in x direction
        id: s2
        input : cs.result
        ddepth : Mat.CV16S
        xOrder : 0
        yOrder : 1
    }
    
    ImageView {
        image: s1.result
    }
    
    ImageView {
        image: s2.result
    }
}
