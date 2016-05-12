import lcvcore 1.0
import lcvimgproc 1.0
import lcvcontrols 1.0
import lcvfeatures2d 1.0

Rectangle{
    anchors.fill : parent
    color : "transparent"
    
    property string imagePath  : codeDocument.path + '/../_images/object_101_piano_train1.jpg'
    property string imagePath2 : codeDocument.path + '/../_images/object_101_piano_query.jpg'
    
    ImRead{
        id : imageSource
        file : imagePath
        visible : false
    }    
    
    Component.onCompleted : {
        console.log('Compilation completed\n________________________________________________________________________________________');
    }  
     
    
    Timer {
        interval: 1000; running: true; //repeat: true
        onTriggered: {
            console.log('Time Triggered\n_____________________________________________________________________________________________');
            //query.file = query.file === imagePath ? imagePath2 : imagePath
        } 
    } 
    
    /*ImRead{
        id : query
        file : "/home/dinu/Work/livecv/samples/_images/object_101_piano_train1.jpg"
        visible : false 
    }*/
    
    CamCapture{
        device : '1'
        id : query
        visible : false
    }
    Resize{ 
        visible : false  
        input : query.output
        id : queryResized
        matSize : Qt.size(query.width/2, query.height/2)
    }
     
    FeatureObjectMatch{
        id : fom
        imageSource : queryResized
        querySource : queryResized
        
        minMatchDistanceCoeff : 2.5
        matchNndrRatio : 0.8
        
        //trainFeatureDetector : OrbFeatureDetector{}
    }
    
}
