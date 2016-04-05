import QtQuick 2.3
import QtQuick.Controls 1.2
import lcvcore 1.0
import lcvcontrols 1.0
import lcvfeatures2d 1.0

Rectangle{
    id : root
    
    anchors.left: parent.left
    anchors.right: parent.right
    color: "transparent"
    
    height : 200
    
    property var imageSource : null
    property FeatureDetector featureDetector : FastFeatureDetector{}
    property DescriptorExtractor descriptorExtractor : BriefDescriptorExtractor{}
    
    signal descriptorsAdded(Mat descriptors)
    
    MatList{
        id : objectList
        property variant keypoints : new Array()
    }
    
    ScrollView{
        width : parent.width
        height: parent.height
        anchors.left: parent.left
        anchors.leftMargin : 50
        style : LiveCVScrollStyle{}
        
    Row{
    
    height: root.height
    //width : trainImages.width
    
    Repeater{
        id : trainImages
        property int selectedIndex : 0
        height : root.height
                        
        model : objectList
        delegate : MatView{
            height : parent.height
            width : (parent.height / mat.dataSize().height) * mat.dataSize().width
            mat : modelData
        }
    }
    
    }
}
    
    SelectionWindow{
        id : selectionWindow
        onRegionSelected : {
            root.featureDetector.input = region
            objectList.appendMat(root.featureDetector.output.createOwnedObject())
            
            var keypoints = root.featureDetector.keypoints.createOwnedObject()
            objectList.keypoints.push(keypoints)
            root.descriptorExtractor.keypoints = keypoints
            root.descriptorsAdded(root.descriptorExtractor.descriptors)
        }
    }


    Rectangle{
        anchors.left: parent.left
        width : 50
        height : parent.height
        MouseArea{
            anchors.fill : parent
            onClicked : {
                selectionWindow.show()
                selectionWindow.mat = root.imageSource.output
            }
        }
    }
    
}

