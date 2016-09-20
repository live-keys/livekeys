import lcvcore 1.0
import lcvimgproc 1.0
import lcvfeatures2d 1.0

Column{
    
    // Brute force matcher
    
    // Usage of BruteForceMatcher on Brief descriptors to find matches
    // between two images
    
    property string trainImage : codeDocument.path + '/../_images/clock-train-small.jpg'
    property string queryImage : codeDocument.path + '/../_images/clock-query-room-small.jpg'
    
    // Train images
    
    Row{
    
        ImRead{
            id: trainImageLoader
            visible : false
            file : trainImage
        }

        FastFeatureDetector{
            id : trainFeatureDetect
            input : trainImageLoader.output
        }

        BriefDescriptorExtractor{
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
        }

        BriefDescriptorExtractor{
            id : queryFeatureExtract
            keypoints : queryFeatureDetect.keypoints
        }
    
    }
    
    // Matching
     
    BruteForceMatcher{
        id : descriptorMatcher
        queryDescriptors : queryFeatureExtract.descriptors
        params : {
            'normType' : BruteForceMatcher.NORM_HAMMING
        }
        knn : -1
    }
    
    DescriptorMatchFilter{
        id: descriptorMatchFilter
        matches1to2: descriptorMatcher.matches
        minDistanceCoeff: 1.5
        nndrRatio: 0.9
    }
       
    DrawMatches{
        keypoints1 : queryFeatureDetect.keypoints
        keypoints2 : trainFeatureDetect.keypoints
        matches1to2 : descriptorMatchFilter.matches1to2Out
        matchIndex : 0
    }
    
}
