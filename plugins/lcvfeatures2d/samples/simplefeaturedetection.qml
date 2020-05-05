import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0
import lcvfeatures2d 1.0

Row{

    // A simple feature detection example
    // You can substitute with 'BriskFeatureDetector', 'OrbFeatureDetector', etc
    // Check the documentation for the parameters data for each detector

    property string imagePath :
        project.dir() + '/../../../samples/_images/object_101_piano_train1.jpg'

    ImRead{
        id : inputImage
        file : imagePath
    }

    FastFeatureDetector{
        id : fastFeatureDetector
        input : inputImage.output
        params : {
            'threshold' : 30,
            'nonmaxSuppresion' : true
        }
    }
    
}
