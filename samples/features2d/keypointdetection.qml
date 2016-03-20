import lcvcontrols 1.0
import lcvcore 1.0
import lcvfeatures2d 1.0

Column{
    
    ImRead{
        id: iminput
        file: codeDocument.path + '/../_images/caltech_buildings_DSCN0246_small.JPG'
        visible: false
    }
    
    MatDraw{
        id : keypointHighlight
        input: detectorSelector.selectedDetector.output
    }
    
    KeypointListView{
        visible: false
        keypointHighlighter: keypointHighlight
        detector : detectorSelector.selectedDetector
        //extractor: descriptorExtractor
    }
    
    FeatureDetectorSelection{
        id: detectorSelector
        detectorInput : iminput.output
    }
    
    ConfigurationPanel{
        configurationFields : detectorSelector.selectedPanelModel
    }
    
    MouseArea{
        width: 100
        height: 100
        onClicked: console.log(detectorSelector.configuration[detectorSelector.selectedName]["threshold"])
    }
}