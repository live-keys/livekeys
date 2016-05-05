import lcvcore 1.0
import lcvimgproc 1.0
import lcvfeatures2d 1.0

Row{
    
    // Train images
    
    // TODO
     
    property string imagePath   : codeDocument.path + '/../_images/'
    
    property string trainImage  : imagePath + 'object_101_piano_train1.jpg'
    property string trainImage2 : imagePath + 'buildings_0246.jpg'
    property string queryImage  : imagePath + 'object_101_piano_query.jpg'
    
    Column{
        id: imageFeatureColumn
        
        property int maxWidth: 300
     
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
            
            width : imageFeatureColumn.maxWidth
            height : trainImageDetector.height
            ImRead{
                id : trainImageLoader
                file : modelData
                visible : false
            }
            
            FastFeatureDetector{
                id : trainImageDetector
                input : trainImageLoader.output
                width: parent.width
                height: (parent.width / trainImageLoader.width) * implicitHeight
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
        params : {
            'normType' : BruteForceMatcher.NORM_HAMMING
        }
        knn : 2
    }
    
    DescriptorMatchFilter{
        id: descriptorMatchFilter
        matches1to2: descriptorMatcher.matches
        minDistanceCoeff: 3.5
        nndrRatio: 0.8
    }
    
    DrawMatches{
        keypoints1 : queryFeatureDetect.keypoints
        keypoints2 : trainImages.selectedItem.trainKeypoints
        matches1to2 : descriptorMatchFilter.matches1to2Out
        matchIndex : trainImages.selectedIndex
    }
     
}
