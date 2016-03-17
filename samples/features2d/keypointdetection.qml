import QtQuick.Controls 1.2
import lcvcore 1.0
import lcvimgproc 1.0
import lcvfeatures2d 1.0

Row{

    // Fast feature detector sample

    property string imagePath :
        codeDocument.path + '/../_images/object_101_piano_train1.jpg'

    ImRead{
        id : inputImage
        file : imagePath
        visible : false
    }

    SimpleBlobDetector{
        id : fastFeatureDetector
        input : inputImage.output
        Component.onCompleted : {
            initialize({
                'minDistanceBetweenBlobs': 50.0,
                'minArea' : 20.0,
                'maxArea': 500.0
            });
        }
    }
    
    BriefDescriptorExtractor{
        id : descriptorExtractor
        keypoints: fastFeatureDetector.keypoints
    }
    
    MatDraw{
        id : keypointHighlight
        input: fastFeatureDetector.output
    }
    
    Column{
        id: container
        
        Rectangle{
            width: 400
            height: 30
            color: "#223344"
            Text{
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                text: 'Total Keypoints: ' + keypointView.model.length
                
                color: '#fff'
                font.pixelSize: 14
                font.family: 'Arial'
                font.bold: true
            }
        }
        
        property FeatureDetector detector : fastFeatureDetector
        property DescriptorExtractor extractor : descriptorExtractor
        property variant descriptorValues : descriptorExtractor ? descriptorExtractor.descriptors.data().values() : []
        
        ScrollView{
            width: 400
            height: 600
            
        ListView{
            id : keypointView
            model : container.extractor ? 
                            container.extractor.keypoints.keyPointData() : container.detector ? 
                                container.extractor.keypoints.keyPointData() : []
            
            width: 400
            height: 600
            clip: true
            currentIndex : -1 
            
            delegate: Rectangle{
                id: keypointContainer
                
                property point pt : modelData.pt ? modelData.pt : '0x0'
                
                width: 400
                height: ListView.isCurrentItem ? 60 + descriptorData.height : 40
                
                color: ListView.isCurrentItem  ? "#223333" : keypointMouse.containsMouse ? "#223328" : "#222228"
                Text{
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.top: parent.top
                    anchors.topMargin: 7
                    text: modelData.pt ? 
                        'P(' + parseFloat(modelData.pt.x).toFixed(2) + ', ' + parseFloat(modelData.pt.y).toFixed(2) + ')' : ''
                    font.pixelSize: 12
                    font.family: 'Arial'
                    color: "#fff"
                }
                Text{
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.top: parent.top
                    anchors.topMargin: 26
                    font.pixelSize: 11
                    font.family: 'Arial'
                    color: '#fff'
                    text: 
                        'Size(' + parseFloat(modelData.size).toFixed(2) + '), ' + 
                        'Angle(' + parseFloat(modelData.angle).toFixed(2) + '), ' + 
                        'Response(' + parseFloat(modelData.response).toFixed(2) + '), ' + 
                        'Octave(' + modelData.octave + '), ' + 
                        'Class(' + modelData.classId + ')'
                }
                TextEdit{
                    id : descriptorData
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.top: parent.top
                    anchors.topMargin: 46
                    width : 310
                    font.pixelSize: 11
                    font.family: 'Courier'
                    color: '#ccc'
                    text: ''
                    wrapMode: TextEdit.WrapAnywhere
                    visible : !ListView.isCurrentItem
                }
                
                MouseArea{
                    id: keypointMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered : {
                        if ( modelData.pt )
                            keypointHighlight.circle(modelData.pt, 10, "#33cc33", 2, 8, 0);
                    }
                    onExited : {
                        keypointHighlight.cleanUp(); 
                        if ( keypointView.currentItem.pt )
                            keypointHighlight.circle(keypointView.currentItem.pt, 10, "#99cc88", 2, 8, 0);
                    }
                    onClicked : {
                        keypointView.currentIndex = index
                        if ( container.extractor !== null ){
                            var descriptorValuesText = '';
                            for ( var i = 0; i < container.descriptorValues[index].length; ++i ){
                                descriptorValuesText += container.descriptorValues[index][i] + ' ';
                            }
                            descriptorData.text = descriptorValuesText
                        }
                    }
                }
            }
        }
    }
    }
    
}
