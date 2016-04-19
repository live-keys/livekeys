import lcvcore 1.0
import lcvcontrols 1.0
import lcvfeatures2d 1.0
 
Row{
    
    property string imagePath : 
        codeDocument.path + '/../_images/caltech_buildings_DSCN0246_small.JPG'
     
    Column{
        FeatureDetectorSelection{
            id: detectorSelector
            detectorInput : iminput.output
        }
        ConfigurationPanel{
            configurationFields : detectorSelector.selectedPanelModel
        }

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
            color : "#222333"
            
        DrawHistogram{
            id : descriptorHistogram
            width : parent.width
            height : 200
            maxValue : 5000
            colors : ["#aa007700"]
            render : DrawHistogram.BinaryConverted
        }
        }
    }
    
    Column{
        DescriptorExtractorSelection{
            id : extractorSelector
            detector : detectorSelector.selectedDetector
        }
        
        ConfigurationPanel{
            configurationFields : extractorSelector.selectedPanelModel
        }
    
        KeypointListView{
            keypointHighlighter: keypointHighlight
            detector : detectorSelector.selectedDetector
            extractor: extractorSelector.selectedExtractor
            onKeypointSelected : descriptorHistogram.setValuesFromIntList(descriptorValues[index])
        }
    }
}
