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
       file : project.dir() + '/../../../samples/_images/object_101_piano_query.jpg'
    }
    
    LevelsSliders{
        id: levelsSliders
        border.width: 1
        border.color: '#061a29'
        input: src.output
    }
    
    Levels{
        input: src.output
        lightness: levelsSliders.lightness
        channels: levelsSliders.levelByChannel
    }

}

