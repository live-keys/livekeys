import lcvcore 1.0
import lcvimgproc 1.0
import lcvfeatures2d 1.0

Row{

    // A simple feature detection example
    // You can substitute with 'BriskFeatureDetector', 'OrbFeatureDetector', etc
    // Check the documentation for the initialization data for each detector

    property string imagePath :
        codeDocument.path + '/../_images/object_101_piano_train1.jpg'

    ImRead{
        id : inputImage
        file : imagePath
    }

    FastFeatureDetector{
        id : fastFeatureDetector
        input : inputImage.output
        Component.onCompleted : {
            initialize({
                'threshold' : 20,
                'nonmaxSuppresion' : true
            })
        }
    }
    
}
