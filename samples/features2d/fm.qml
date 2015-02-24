import 'lcvcore' 1.0
import "lcvimgproc" 1.0
import "lcvfeatures2d" 1.0

Grid{
    columns : 2
    
    // Train images
    
    property string imagePath : codeDocument.path + '/../_images/caltech_buildings_DSCN0246_small.JPG'
    
    ImRead{
        id: trainImageLoader
        
        property variant images
        property variant keypoints
        
        Component.onCompleted : {
            var imageArray    = new Array()
            var keypointArray = new Array()
            
            file = imagePath
            imageArray.push(output.createOwnedObject())
            keypointArray.push(trainFeatureDetect.output.createOwnedObject())
            
            images    = imageArray
            keypoints = keypointArray
        }
    }
    
    FastFeatureDetector{
        id : trainFeatureDetect
        input : trainImageLoader.output
    }
    
    BriefDescriptorExtractor{
        input : trainFeatureDetect.output
        onDescriptorsChanged : {
            descriptorMatcher.add(descriptors)
            descriptorMatcher.train();
        }
    }
    
    // Query Image
    
    
    ImRead{
        id : queryImageLoader
        file : imagePath
    }
    
    FastFeatureDetector{
        id : queryFeatureDetect
        input : queryImageLoader.output
    }
    
    BriefDescriptorExtractor{
        id : queryFeatureExtract
        input : queryFeatureDetect.output
    }
    
    // Matching
    
    BruteForceMatcher{
        id : descriptorMatcher
        queryDescriptors : queryFeatureExtract.descriptors
    }
    
    DrawMatches{
        img1 : trainImageLoader.images[0]
        keypoints1 : trainImageLoader.keypoints[0]
        matches1to2 : descriptorMatcher.matches
    }
    
    
}