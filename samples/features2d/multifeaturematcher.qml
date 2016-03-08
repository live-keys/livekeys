import 'lcvcore' 1.0
import "lcvimgproc" 1.0
import "lcvfeatures2d" 1.0


Column{
    
    // Train images
    
    property string imagePath   : codeDocument.path + '/../_images/'
    property string trainImage  : imagePath + 'object_101_piano_train1.jpg'
    property string trainImage2 : imagePath + 'caltech_buildings_DSCN0246_small.JPG'
    property string queryImage  : imagePath + 'object_101_piano_query.jpg'
    
    Row{
    
    Repeater{
        id : trainImages

        property variant selectedItem : null
        property int     selectedIndex : 0
                        
        model : [trainImage, trainImage2]
        
        delegate : Rectangle{
            id: delegateRoot
            
            property variant trainImage : trainImageLoader.output
            property variant trainKeypoints : trainImageDetector.keypoints
            property int     trainIndex : index
            
            width : trainImageLoader.width
            height : trainImageLoader.height
            ImRead{
                id : trainImageLoader
                file : modelData
                visible : false
            }
            
            FastFeatureDetector{
                id : trainImageDetector
                input : trainImageLoader.output
            }
            
            BriefDescriptorExtractor{
                keypoints : trainImageDetector.keypoints
                onDescriptorsChanged : {
                    descriptorMatcher.add(descriptors)
                    descriptorMatcher.train();
                }
            }
            
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    trainImages.selectedIndex = delegateRoot.trainIndex
                    trainImages.selectedItem  = trainImages.itemAt(trainImages.selectedIndex)
                }
            }
        }
        
        Component.onCompleted : {
            if ( count > 0 ){
                selectedIndex = 0
                selectedItem = itemAt(selectedIndex)
            }
        }
        
    }
    
}
    
    // Query Image
    
    
    ImRead{
        id : queryImageLoader
        file : queryImage
        visible : false
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
    }
    
    DrawMatches{
        keypoints1 : queryFeatureDetect.keypoints
        keypoints2 : trainImages.selectedItem.trainKeypoints
        matches1to2 : descriptorMatcher.matches
        matchIndex : trainImages.selectedIndex
    }
    
}
