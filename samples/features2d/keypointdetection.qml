import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import lcvcore 1.0
import lcvimgproc 1.0
import lcvfeatures2d 1.0
import lcvcontrols 1.0

Row{

    // Fast feature detector sample

    property string imagePath :
        codeDocument.path + '/../_images/object_101_piano_train1.jpg'

    ImRead{
        id : inputImage
        file : imagePath
        visible : false
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
        visible: false
    }
    
    BriefDescriptorExtractor{
        id : descriptorExtractor
        keypoints: fastFeatureDetector.keypoints
    }
    
    MatDraw{
        id : keypointHighlight
        input: fastFeatureDetector.output
    }
    
    KeypointListView{
        keypointHighlighter: keypointHighlight
        detector : fastFeatureDetector
        extractor: descriptorExtractor
    }
    
}
