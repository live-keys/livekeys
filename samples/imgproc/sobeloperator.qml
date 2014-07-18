import 'lcvcore' 1.0
import 'lcvimgproc' 1.0

Column{
    
    // Sobel operator sample in x and y directions
    
    id : root
    
    property string imagePath : codeDocument.path + '/../_images/caltech_buildings_DSCN0246.JPG'
    
    ImRead{
        id : src
        file : root.imagePath
        Component.onCompleted : {
            width = width / 4
            height = height / 4
        }
    }
    
    Resize{
        id : scrs
        input : src.output
        fx : 0.25
        fy : 0.25
    }
    
    ChannelSelect{
        id : cs
        input : scrs.output
        channel : 2
    }
    
    Row{
    
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
    }
}
