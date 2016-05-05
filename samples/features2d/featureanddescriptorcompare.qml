import lcvcore 1.0
import lcvcontrols 1.0
import lcvfeatures2d 1.0
 
Row{
    
    property string imagePath  : codeDocument.path + '/../_images/buildings_0246.jpg'
    property string imagePath2 : codeDocument.path + '/../_images/buildings_0246.jpg'
     
    Column{

        ImRead{ 
            id: iminput
            file: imagePath
            visible: false
        }
        MatDraw{
            id : keypointHighlight
            input: detectorSelector.selectedDetector.output
        }
        
        Rectangle{
            width : parent.width 
            height : 200
            color : LiveCVStyle.darkElementBackgroundColor
            
            DrawHistogram{
                id : descriptorHistogram
                width : parent.width
                height : 200
                maxValue : 5000
                colors : ["#aa007700", "#66000077"]
                render : DrawHistogram.BinaryConverted
            }
        }

        ImRead{
            id: iminput2
            file: imagePath2
            visible: false
        }
        MatDraw{
            id : keypointHighlight2
            input: detectorSelector2.selectedDetector.output
        }
        
    }
    
    Column{
        spacing : 1
        
        FeatureDetectorSelection{
            id: detectorSelector
            detectorInput : iminput.output
            TextButton{
                anchors.right : parent.right
                anchors.verticalCenter : parent.verticalCenter
                width : 20
                height : 20
                fontPixelSize : 20
                onClicked : featureConfigPanel.visible = !featureConfigPanel.visible
                text : featureConfigPanel.visible ? "-" : "+"
            }
        }
        ConfigurationPanel{
            id : featureConfigPanel
            visible : false
            configurationFields : detectorSelector.selectedPanelModel
        }
        
        DescriptorExtractorSelection{
            id : extractorSelector
            detector : detectorSelector.selectedDetector
            TextButton{
                anchors.right : parent.right
                anchors.verticalCenter : parent.verticalCenter
                width : 20
                height : 20
                fontPixelSize : 20
                onClicked : descriptorConfigPanel.visible = !descriptorConfigPanel.visible
                text : descriptorConfigPanel.visible ? "-" : "+"
            }
        }
        
        ConfigurationPanel{
            id : descriptorConfigPanel
            visible : false
            configurationFields : extractorSelector.selectedPanelModel
        }
    
        KeypointListView{
            height : 400
            width : 500
            keypointHighlighter: keypointHighlight
            detector : detectorSelector.selectedDetector
            extractor: extractorSelector.selectedExtractor
            onKeypointSelected : descriptorHistogram.setValuesFromIntListAt(descriptorValues[index], 0)
        }
        
        FeatureDetectorSelection{
            id: detectorSelector2
            detectorInput : iminput2.output
            TextButton{
                anchors.right : parent.right
                anchors.verticalCenter : parent.verticalCenter
                width : 20
                height : 20
                fontPixelSize : 20
                onClicked : featureConfigPanel2.visible = !featureConfigPanel2.visible
                text : featureConfigPanel2.visible ? "-" : "+"
            }
        }
        
        ConfigurationPanel{
            id : featureConfigPanel2
            visible : false
            configurationFields : detectorSelector2.selectedPanelModel
        }
        
        DescriptorExtractorSelection{
            id : extractorSelector2
            detector : detectorSelector2.selectedDetector
            TextButton{
                anchors.right : parent.right
                anchors.verticalCenter : parent.verticalCenter
                width : 20
                height : 20
                fontPixelSize : 20
                onClicked : descriptorConfigPanel2.visible = !descriptorConfigPanel2.visible
                text : descriptorConfigPanel2.visible ? "-" : "+"
            }
        }
        
        ConfigurationPanel{
            id : descriptorConfigPanel2
            visible : false
            configurationFields : extractorSelector2.selectedPanelModel
        }
    
        KeypointListView{
            width : 500
            height : 400
            keypointHighlighter: keypointHighlight2
            detector : detectorSelector2.selectedDetector
            extractor: extractorSelector2.selectedExtractor
            onKeypointSelected : descriptorHistogram.setValuesFromIntListAt(descriptorValues[index], 1)
        }
    }
}
