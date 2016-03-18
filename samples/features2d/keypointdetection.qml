import lcvcontrols 1.0

Column{
    
    FeatureDetectorSelection{
        id: detectorSelector
    }
    
    ConfigurationPanel{
//        configurationData : detectorSelector.configuration["FastFeatureDetector"]
        configurationFields : detectorSelector.selectedPanelModel
    }
    
    MouseArea{
        width: 100
        height: 100
        onClicked: console.log(detectorSelector.configuration[detector.selectedName]["threshold"])
    }
}