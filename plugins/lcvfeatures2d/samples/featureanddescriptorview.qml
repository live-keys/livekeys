import QtQuick 2.3
import live 1.0
import lcvcore 1.0
import lcvimgproc 1.0
import lcvfeatures2d 1.0
 
Row{
    
    // Descriptor histogram sample
    
    // Select/configure feature detectors/extractors from the panels.
    // The generated feature list can then be used to view descriptors
    // in the generated histogram
    
    property string imagePath : project.dir() + '/../../../samples/_images/clock-train-small.jpg'
     
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
            color : "#020d14"
            
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
            width : 500
            keypointHighlighter: keypointHighlight
            detector : detectorSelector.selectedDetector
            extractor: extractorSelector.selectedExtractor
            onKeypointSelected : descriptorHistogram.setValuesFromIntList(descriptorValues[index])
        }
    }
}
