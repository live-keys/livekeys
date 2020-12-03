import QtQuick 2.3
import editqml 1.0
import lcvcore 1.0 as Cv
import lcvimgproc 1.0 as Img

Grid{
    spacing: 2
    columns: 1

    Cv.ImRead{
        id: src
        file: project.path('../../../../samples/_images/buildings_0246.jpg')
    }

    PerspectiveInput{
        id: perspective
        property Watcher watcher: Watcher{
            position: scanner.capture()
        }
        input: src.output.cloneMat()
    }

    Cv.ImageView{
        image: perspective.result
    }
}
