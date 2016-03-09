import lcvcore 1.0
import lcvimgproc 1.0
import lcvfeatures2d 1.0

Row{

    // Fast feature detector sample

    property string imagePath :
        codeDocument.path + '/../_images/object_101_piano_train1.jpg'

    ImRead{
        id : inputImage
        file : imagePath
    }

    SimpleBlobDetector{
        id : fastFeatureDetector
        input : inputImage.output
        Component.onCompleted : {
            initialize({
                'minDistanceBetweenBlobs': 50.0,
                'minArea' : 20.0,
                'maxArea': 500.0
            });
        }
    }
    
}
