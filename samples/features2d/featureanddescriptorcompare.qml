import QtQuick 2.3
import live 1.0
import lcvcore 1.0
import lcvimgproc 1.0
import lcvfeatures2d 1.0
 
Row{
    
    // Descriptor histogram comparison sample
    
    // Select/configure feature detectors/extractors from the panels
    // to compare and view descriptors in the generated histogram
    
    property string imagePath  : project.dir() + '/../_images/clock-train-small.jpg'
    property string imagePath2 : project.dir() + '/../_images/clock-query-room-small.jpg'
     
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
            height : 100
            color : LiveCVStyle.darkElementBackgroundColor
            
            DrawHistogram{
                id : descriptorHistogram
                width : parent.width
                height : 100
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
            stateId : 'trainFeatureDetectorSelection'
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
