import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0

Grid{
    columns: 2

    // This sample shows the usage of the CvtColor and Threshold
    // elements

    ImageRead{
       id : src
       file : project.dir() + '/../../../samples/_images/buildings_0246.jpg'
    }

    Threshold{
        id: threshold
        input: src.result
        threshold : 95
        maxValue : 255
    }
    
    ImageView {
        image: threshold.result
    }

}
