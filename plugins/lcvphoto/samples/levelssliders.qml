import QtQuick 2.3
import lcvcore 1.0
import lcvphoto 1.0
import live 1.0

Grid{
    
    columns: 2
    spacing: 10
    
    // Using Levels
    
    ImRead{
       id : src
       file : project.dir() + '/../../../samples/_images/coast_6.jpg'
    }
    
    LevelsSliders{
        id: levelsSliders
        border.width: 1
        border.color: '#061a29'
        input: src.output
    }
    
    Levels{
        id: levels
        input: src.output.cloneMat()
        lightness: levelsSliders.lightness
        channels: levelsSliders.levelByChannel
    }
    
    ImageView{
        image: levels.result
    }

}

