import QtQuick 2.3
import lcvcore 1.0
import lcvphoto 1.0

Grid{
    
    columns: 2
    
    ImRead{
        id: src
        file: project.dir() + '/../../../samples/_images/object_101_piano_query.jpg'
        visible: false
    }
    
    Levels{
        id: levels
        lightness: [10, 1.5, 220]
        channels: {
            0: [0, 1.0, 170]
        }
        input: src.output.cloneMat()
        onComplete: exec()
    }
    
    ImageView{
        image: levels.result
    }
    
}
