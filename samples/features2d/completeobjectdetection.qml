import lcvcore 1.0
import lcvimgproc 1.0
import lcvcontrols 1.0
import lcvfeatures2d 1.0

Rectangle{
    anchors.fill : parent
    color : "transparent"
            
    property string imagePath  : codeDocument.path + '/../_images/clock-train-small.jpg'
    property string imagePath2 : codeDocument.path + '/../_images/clock-query-room-small.jpg'
    
    ImRead{
        id : imageSource
        file : imagePath
        visible : false
    }
    
    ImRead{
        id : query
        file : codeDocument.path + '/../_images/clock-query-grass-small.jpg'
        visible : false 
    }
     
    FeatureObjectMatch{
        id : fom
        imageSource : imageSource
        querySource : query
    trainFeatureDetector : FastFeatureDetector{
        params : {
            'threshold' : 5
        }
    }
        
        minMatchDistanceCoeff : 5.5
        matchNndrRatio : 0.8
        
        //trainFeatureDetector : OrbFeatureDetector{}
    }
    
}
