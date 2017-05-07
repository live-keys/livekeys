import QtQuick 2.3
import lcvcore 1.0
import lcvimgproc 1.0
import lcvfeatures2d 1.0

Rectangle{
    
    // This sample shows the FeatureObjectMatch component, which provides a clipping
    // window to add new objects, together with a homography are to display found objects
    // To start, click on the add (+) button on the left of the scene, and select an 
    // area of the object to add. The detected object will be highlighted on the query
    // image. You can change sources by changing the file property from the imageSource
    // component. Or you can change the source or query images with a webcam and start
    // from there. 
    
    anchors.fill : parent
    color : "transparent"
            
    property string trainImage  : project.dir() + '/../_images/cards-train-j.jpg'
    property string trainImage2 : project.dir() + '/../_images/cards-train-q.jpg'
    property string trainImage3 : project.dir() + '/../_images/cards-train-k.jpg'
    
    property string queryImage : project.dir() + '/../_images/cards-query.jpg'
    
    ImRead{
        id : imageSource
        file : trainImage
        visible : false
    }
    
    ImRead{
        id : query
        file : queryImage
        visible : false
    }
     
    FeatureObjectMatch{
        id : fom
        imageSource : imageSource
        querySource : query
        
        minMatchDistanceCoeff : 15.5
        matchNndrRatio : 0.8
    }
    
}
