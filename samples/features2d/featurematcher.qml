import 'lcvcore' 1.0
import "lcvimgproc" 1.0
import "lcvfeatures2d" 1.0

Grid{
    columns : 2
    
    // Train images
    
    property string imagePath : codeDocument.path + '/../_images/'
    property string trainImage : imagePath + 'object_101_piano_train1.jpg'
    property string queryImage : imagePath + 'object_101_piano_query.jpg'
    
    ImRead{
        id: trainImageLoader
        
        visible : false
        
        property variant images
        property variant keypoints
        
        Component.onCompleted : {
            var imageArray    = new Array()
            var keypointArray = new Array()
            
            file = trainImage
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
        input : trainImageLoader.output
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
        input : queryImageLoader.output
        keypoints : queryFeatureDetect.keypoints
    }
    
    // Matching
    
    BruteForceMatcher{
        id : descriptorMatcher
        queryDescriptors : queryFeatureExtract.descriptors
    }
    
    DrawMatches{
        keypoints1 : queryFeatureDetect.keypoints
        keypoints2 : trainImageLoader.keypoints[0]
        matches1to2 : descriptorMatcher.matches
    }
    
}
