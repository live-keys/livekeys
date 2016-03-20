import QtQuick 2.3
import lcvcore 1.0
import lcvfeatures2d 1.0

Rectangle{
    id: root

    property var configuration : ({
        "FastFeatureDetector" : {
            'threshold' : 10,
            'nonmaxSuppresion': true
        },
        "BriskFeatureDetector" : {
            'thresh' : 30,
            'octaves' : 3,
            'patternScale' : 1.0
        }
    })

    signal configurationUpdate()

    function updateConfiguration(detectorKey, propertyKey, value){
        root.configuration[detectorKey][propertyKey] = value
        if ( selectedDetector )
            selectedDetector.initialize(root.configuration[detectorKey])
        root.configurationUpdate()
    }

    property list<QtObject> model: [
        QtObject{
            property string name: "FastFeatureDetector"
            property FeatureDetector detector: FastFeatureDetector{}
            property list<Component> configurationFields : [
                Component{
                    ConfigurationField{
                        label : "Threshold"
                        editor : InputBox{
                            text: root.configuration["FastFeatureDetector"]["threshold"]
                            onTextChanged: root.updateConfiguration("FastFeatureDetector", "threshold", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "Non Max Suppression"
                        editor : InputBox{
                            text: root.configuration["FastFeatureDetector"]["nonmaxSuppresion"]
                            onTextChanged: root.updateConfiguration("FastFeatureDetector", "nonmaxSuppresion", text)
                        }
                    }
                }
            ]
        },
        QtObject{
            property string name: "BriskFeatureDetector"
            property FeatureDetector detector: BriskFeatureDetector{}
            property list<Component> configurationFields : [
                Component{
                    ConfigurationField{
                        label : "Threshold"
                        editor : InputBox{
                            text: root.configuration["BriskFeatureDetector"]["thresh"];
                            onTextChanged: root.updateConfiguration("BriskFeatureDetector", "thresh", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "Octaves"
                        editor : InputBox{
                            text: root.configuration["BriskFeatureDetector"]["octaves"];
                            onTextChanged: root.updateConfiguration("BriskFeatureDetector", "octaves", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "Pattern Scale"
                        editor : InputBox{
                            text: root.configuration["BriskFeatureDetector"]["patternScale"];
                            onTextChanged: root.updateConfiguration("BriskFeatureDetector", "patternScale", text)
                        }
                    }
                }
            ]
        }
    ]

    property Mat   detectorInput : null

    property alias selectedName: chosenItemText.text
    property alias selectedDetector: chosenItem.detector
    onSelectedDetectorChanged: {
        selectedDetector.input = detectorInput
    }
    Component.onCompleted: selectedDetector.input = detectorInput

    property alias selectedPanelModel: chosenItem.panelModel
    property alias selectedIndex: listView.currentIndex

    property color backgroundColor: "#444"
    property color highlightColor: "#44444a"
    property color textColor: "#ccc"

    property int dropBoxHeight: 80

    signal comboClicked

    z: 100
    
    width: 300
    height : 30

    Rectangle {
        id: chosenItem
        width: parent.width
        height: root.height
        color: chosenItemMouse.containsMouse ? root.highlightColor : root.backgroundColor

        property FeatureDetector detector: root.model ? root.model[0].detector : null
        property var panelModel: root.model ? root.model[0].configurationFields : []

        Text{
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            id: chosenItemText
            text: root.model ? root.model[0].name : ''
            font.family: "Arial, Helvetica, sans-serif"
            font.pixelSize: 13
            color: "#ddd"
        }

        MouseArea{
            id: chosenItemMouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                root.state = root.state === "dropDown" ? "" : "dropDown"
            }
            cursorShape: Qt.PointingHandCursor
        }
    }

    Rectangle {
        id: dropDown
        width: root.width
        height: 0
        clip: true
        anchors.top: chosenItem.bottom
        color: root.backgroundColor

        ListView {
            id: listView
            height: root.dropBoxHeight
            model: root.model
            currentIndex: 0
            delegate: Rectangle {
                width: root.width
                height: root.height
                color: delegateArea.containsMouse ? root.highlightColor : "transparent"

                Text{
                    text: modelData.name
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 10
                    color: "#ddd"
                    font.family: "Arial, Helvetica, sans-serif"
                    font.pixelSize: 13
                }
                MouseArea{
                    id: delegateArea
                    anchors.fill: parent
                    onClicked: {
                        root.state = ""
                        var prevSelection = chosenItemText.text
                        chosenItemText.text = modelData.name
                        if (chosenItemText.text != prevSelection) {
                            chosenItem.panelModel = modelData.configurationFields
                            chosenItem.detector   = modelData.detector
                            root.comboClicked()
                        }
                        listView.currentIndex = index
                    }
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                }
            }
        }
    }

    states: State {
        name: "dropDown"
        PropertyChanges {
            target: dropDown
            height: 40 * root.model.length
        }
    }

    transitions: Transition {
        NumberAnimation {
            target: dropDown
            properties: "height"
            easing.type: Easing.OutExpo
            duration: 1000
        }
    }
}


