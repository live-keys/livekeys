import lcvcore 1.0
import lcvimgproc 1.0
import lcvfeatures2d 1.0

Grid{
    columns : 2
    
    // Train images
    
    property string imagePath   : codeDocument.path + '/../_images/'
    property string trainImage  : imagePath + 'object_101_piano_train1.jpg'
    property string trainImage2 : imagePath + 'caltech_buildings_DSCN0246_small.JPG'
    property string queryImage  : imagePath + 'object_101_piano_query.jpg'
    
    ImRead{
        id: trainImageLoader
        
        visible : false
        
        property var images
        property var keypoints
        
        Component.onCompleted : {
            var imageArray    = new Array()
            var keypointArray = new Array()
            
            file = trainImage
            imageArray.push(output.createOwnedObject())
            keypointArray.push(trainFeatureDetect.keypoints.createOwnedObject())
            
            file = trainImage2
            imageArray.push(output.createOwnedObject())
            keypointArray.push(trainFeatureDetect.keypoints.createOwnedObject())
            
            images    = imageArray
            keypoints = keypointArray
        }
    }
    
    FastFeatureDetector{
        id : trainFeatureDetect
        input : trainImageLoader.output
    }
    
    BriefDescriptorExtractor{
        keypoints : trainFeatureDetect.keypoints
        onDescriptorsChanged : {
            descriptorMatcher.add(descriptors)
            descriptorMatcher.train();
        }
    }
        
    // Query Image
    
    
    ImRead{
        id : queryImageLoader
        file : queryImage
    }
    
    FastFeatureDetector{
        id : queryFeatureDetect
        input : queryImageLoader.output
    }
    
    BriefDescriptorExtractor{
        id : queryFeatureExtract
        keypoints : queryFeatureDetect.keypoints
    }
    
    // Matching
    
    BruteForceMatcher{
        id : descriptorMatcher
        queryDescriptors : queryFeatureExtract.descriptors
        knn: 2
    }
    
    DescriptorMatchFilter{
        id: descriptorMatchFilter
        matches1to2: descriptorMatcher.matches
        minDistanceCoeff: 2.5
        nndrRatio: 0.8
    }
    
    MatchesToLocalKeypoint{
        id: matchesToLocalKeypoint
        matches1to2 : descriptorMatchFilter.matches1to2Out
        trainKeypointVectors : trainImageLoader.keypoints
        queryKeypointVector : queryFeatureDetect.keypoints
    }
    
    KeypointHomography{
        queryImage : queryImageLoader.output
        keypointsToScene: matchesToLocalKeypoint.output
        objectCorners : [
            [Qt.point(0,0), Qt.point(100,0), Qt.point(100,100), Qt.point(0,100)]
        ]
    }
    
    DrawMatches{
        keypoints1 : queryFeatureDetect.keypoints
        keypoints2 : trainImageLoader.keypoints[0]
        matches1to2 : descriptorMatchFilter.matches1to2Out
        matchIndex : 0
    }
    
}
