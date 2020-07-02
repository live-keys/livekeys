import QtQuick 2.3
import lcvcore 1.0

Grid{
    
    BlankImage{
        id: blankImage
        size: "800x600"
        fill: 'white'
        channels: 3
    }
    
    Draw{
        id: draw
        input: blankImage.result
    }
    
    ColorHistogram{
        input: draw.result
    }
    
}