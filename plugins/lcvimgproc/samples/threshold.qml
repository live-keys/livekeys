import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0

Grid{
    columns: 2

    // This sample shows the usage of the CvtColor and Threshold
    // elements

    ImRead{
       id : src
       file : project.dir() + '/../../../samples/_images/buildings_0246.jpg'
    }

    CvtColor{
        id : grayscale
        input : src.output
        code : CvtColor.CV_BGR2GRAY
        dstCn : 1
    }

    Threshold{
        input: grayscale.output
        thresh : 95
        maxVal : 255
        thresholdType : Threshold.BINARY
    }
}
