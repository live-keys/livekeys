import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0
import lcvfeatures2d 1.0

Column{

    // Keypoint homography
    
    // This sample shows the basic components required to detect a trained object
    // within an image
    
    property string trainImage : project.dir() + '/../../../samples/_images/clock-train-small.jpg'
    property string queryImage : project.dir() + '/../../../samples/_images/clock-query-room-small.jpg'
    
    // Train images
    
    ImRead{
        id: trainImageLoader 
        visible : false
        file : trainImage
    }
    
    FastFeatureDetector{
        id : trainFeatureDetect
        input : trainImageLoader.output
        visible : false
    }
    
    BriskDescriptorExtractor{
        keypoints : trainFeatureDetect.keypoints
        Component.onCompleted : {
            descriptorMatcher.add(descriptors)
            descriptorMatcher.train();
        }
    }
        
    // Query Image
    
    ImRead{
        id : queryImageLoader
        visible : false
        file : queryImage
    }
    
    FastFeatureDetector{
        id : queryFeatureDetect
        input : queryImageLoader.output
        visible : false
    }
    
    BriskDescriptorExtractor{
        id : queryFeatureExtract
        keypoints : queryFeatureDetect.keypoints
    }
    
    // Matching
     
    FlannBasedMatcher{
        id : descriptorMatcher
        queryDescriptors : queryFeatureExtract.descriptors
        knn : 2
        params : {'indexParams' : 'Lsh'}
    }
    
    DescriptorMatchFilter{
        id: descriptorMatchFilter
        matches1to2: descriptorMatcher.matches
        minDistanceCoeff: 12.5
        nndrRatio: 0.8
    }
       
    DrawMatches{
        keypoints1 : queryFeatureDetect.keypoints
        keypoints2 : trainFeatureDetect.keypoints
        matches1to2 : descriptorMatchFilter.matches1to2Out
        matchIndex : 0
    }
    
    
    MatchesToLocalKeypoint{
        id: matchesToLocalKeypoint
        matches1to2 : descriptorMatchFilter.matches1to2Out
        trainKeypointVectors : [trainFeatureDetect.keypoints]
        queryKeypointVector : queryFeatureDetect.keypoints
    }
    
    KeypointHomography{
        queryImage : queryImageLoader.output
        keypointsToScene: matchesToLocalKeypoint.output
        objectCorners : [
            [Qt.point(0,0), 
             Qt.point(trainFeatureDetect.width,0), 
             Qt.point(trainFeatureDetect.width, trainFeatureDetect.height), 
             Qt.point(0,trainFeatureDetect.height)]
        ]
    }
    
}

    
