import QtQuick 2.3
import editqml 1.0
import lcvcore 1.0 as Cv
import lcvimgproc 1.0 as Img

Grid{
    spacing: 2
    columns: 1

    Cv.BlankImage{
        id: blankImage
        size: "600x400"
        fill: 'white'
        channels: 3
    }
    
    Cv.DrawSurface{
        id: drawSurface
        input: blankImage.result
    }
    
    PerspectiveInput{
        id: perspective
        property Watcher watcher: Watcher{}
        input: drawSurface.result
    }


    Cv.ImageView{
        image: perspective.result
    }
}
