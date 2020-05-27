import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0
import lcvfeatures2d 1.0

Row{

    // Configuring the SimpleBlobDetector in order to extract specific
    // blobs within an image

    spacing : 15

    property string imagePath :
        project.dir() + '/../../../samples/_images/object_101_piano_train1.jpg'

    ImRead{
        id : inputImage
        file : imagePath
    }

    SimpleBlobDetector{
        id : fastFeatureDetector
        input : inputImage.output
        params : {
            'minDistanceBetweenBlobs': 50.0,
            'minArea' : 20.0,
            'maxArea': 500.0
        }
    }
    
}
