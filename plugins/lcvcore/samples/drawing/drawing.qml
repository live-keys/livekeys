import QtQuick 2.3
import lcvcore 1.0
import live 1.0

Grid{
    
    BlankImage{
        id: blankImage
        size: "800x600"
        fill: 'white'
        channels: 3
    }
    
    DrawSurface{
        id: drawSurface
        input: blankImage.result
    }
        
    ColorHistogramView{
        input: drawSurface.result
    }

}
