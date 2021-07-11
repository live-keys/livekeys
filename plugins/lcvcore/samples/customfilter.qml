import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0

Row{
    
    // This sample shows how to iterate or 
    // access values from an image directly
    // from the qml file and create a custom filter.
    
    ImageRead{
       id : src
       file : project.dir() + '/../../../samples/_images/buildings_0246.jpg'
    }
    
    ChannelSelect{
        id: cs
        input : src.result
        
        
        onResultChanged : {
            if (!result) return

            var dim   = result.dimensions()
            var buffer = result.buffer() 
            var uview = new Uint8Array(buffer) 
            
            for ( var i = 0; i < dim.height; ++i ){
                for ( var j = 0; j < dim.width; ++j ){
                    var val = uview[i * dim.width + j]
                    if ( val < 100 )
                        uview[i * dim.width + j] = 0
                }
            }
            
        }
    }
    
    ImageView {
        id: imageView
        image: cs.result
    }
    
}
