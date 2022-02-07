/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import live 1.0
import lcvcore 1.0
import lcvfeatures2d 1.0

Rectangle{
    id: root

    property list<QtObject> model : [
        QtObject{
            property string name: "FastFeatureDetector"
            property FeatureDetector detector: FastFeatureDetector{}
            property list<Component> configurationFields : [
                Component{
                    ConfigurationField{
                        label : "Threshold"
                        editor : InputBox{
                            text: globalConfig.item.configuration["FastFeatureDetector"]["threshold"]
                            onTextChanged: root.updateConfiguration("FastFeatureDetector", "threshold", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "Non Max Suppression"
                        editor : InputBox{
                            text: globalConfig.item.configuration["FastFeatureDetector"]["nonmaxSuppresion"]
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
                            text: globalConfig.item.configuration["BriskFeatureDetector"]["thresh"];
                            onTextChanged: root.updateConfiguration("BriskFeatureDetector", "thresh", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "Octaves"
                        editor : InputBox{
                            text: globalConfig.item.configuration["BriskFeatureDetector"]["octaves"];
                            onTextChanged: root.updateConfiguration("BriskFeatureDetector", "octaves", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "Pattern Scale"
                        editor : InputBox{
                            text: globalConfig.item.configuration["BriskFeatureDetector"]["patternScale"];
                            onTextChanged: root.updateConfiguration("BriskFeatureDetector", "patternScale", text)
                        }
                    }
                }
            ]
        },
        QtObject{
            property string name: "OrbFeatureDetector"
            property FeatureDetector detector: OrbFeatureDetector{}
            property list<Component> configurationFields : [
                Component{
                    ConfigurationField{
                        label : "NFeatures"
                        editor : InputBox{
                            text: globalConfig.item.configuration["OrbFeatureDetector"]["nfeatures"]
                            onTextChanged: root.updateConfiguration("OrbFeatureDetector", "nfeatures", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "Scale Factor"
                        editor : InputBox{
                            text: globalConfig.item.configuration["OrbFeatureDetector"]["scaleFactor"]
                            onTextChanged: root.updateConfiguration("OrbFeatureDetector", "scaleFactor", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "NLevels"
                        editor : InputBox{
                            text: globalConfig.item.configuration["OrbFeatureDetector"]["nlevels"]
                            onTextChanged: root.updateConfiguration("OrbFeatureDetector", "nlevels", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "Edge Threshold"
                        editor : InputBox{
                            text: globalConfig.item.configuration["OrbFeatureDetector"]["edgeThreshold"]
                            onTextChanged: root.updateConfiguration("OrbFeatureDetector", "edgeThreshold", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "First Level"
                        editor : InputBox{
                            text: globalConfig.item.configuration["OrbFeatureDetector"]["firstLevel"]
                            onTextChanged: root.updateConfiguration("OrbFeatureDetector", "firstLevel", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "WTA_K"
                        editor : InputBox{
                            text: globalConfig.item.configuration["OrbFeatureDetector"]["WTA_K"]
                            onTextChanged: root.updateConfiguration("OrbFeatureDetector", "WTA_K", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "Score Type"
                        editor : InputBox{
                            text: globalConfig.item.configuration["OrbFeatureDetector"]["scoreType"]
                            onTextChanged: root.updateConfiguration("OrbFeatureDetector", "scoreType", text)
                        }
                    }
                }
            ]
        },
        QtObject{
            property string name: "MSerFeatureDetector"
            property FeatureDetector detector: MSerFeatureDetector{}
            property list<Component> configurationFields : [
                Component{
                    ConfigurationField{
                        label : "Delta"
                        editor : InputBox{
                            text: globalConfig.item.configuration["MSerFeatureDetector"]["delta"]
                            onTextChanged: root.updateConfiguration("MSerFeatureDetector", "delta", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "Min Area"
                        editor : InputBox{
                            text: globalConfig.item.configuration["MSerFeatureDetector"]["minArea"]
                            onTextChanged: root.updateConfiguration("MSerFeatureDetector", "minArea", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "Max Area"
                        editor : InputBox{
                            text: globalConfig.item.configuration["MSerFeatureDetector"]["maxArea"]
                            onTextChanged: root.updateConfiguration("MSerFeatureDetector", "maxArea", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "Max Variation"
                        editor : InputBox{
                            text: globalConfig.item.configuration["MSerFeatureDetector"]["maxVariation"]
                            onTextChanged: root.updateConfiguration("MSerFeatureDetector", "maxVariation", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "Min Diversity"
                        editor : InputBox{
                            text: globalConfig.item.configuration["MSerFeatureDetector"]["minDiversity"]
                            onTextChanged: root.updateConfiguration("MSerFeatureDetector", "minDiversity", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "Max Evolution"
                        editor : InputBox{
                            text: globalConfig.item.configuration["MSerFeatureDetector"]["maxEvolution"]
                            onTextChanged: root.updateConfiguration("MSerFeatureDetector", "maxEvolution", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "Area Threshold"
                        editor : InputBox{
                            text: globalConfig.item.configuration["MSerFeatureDetector"]["areaThreshold"]
                            onTextChanged: root.updateConfiguration("MSerFeatureDetector", "areaThreshold", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "Min Margin"
                        editor : InputBox{
                            text: globalConfig.item.configuration["MSerFeatureDetector"]["minMargin"]
                            onTextChanged: root.updateConfiguration("MSerFeatureDetector", "minMargin", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "Edge Blur Size"
                        editor : InputBox{
                            text: globalConfig.item.configuration["MSerFeatureDetector"]["edgeBlurSize"]
                            onTextChanged: root.updateConfiguration("MSerFeatureDetector", "edgeBlurSize", text)
                        }
                    }
                }
            ]
        }
    ]

    property string stateId : "featureDetectorConfig"

    function staticLoad(id){
        globalConfig.staticLoad(id)
    }

    StaticLoader{
        id : globalConfig
        source : Item{
            id : globalConfigItem

            property int selectedIndex : 0
            property var configuration : {
               "FastFeatureDetector" : {
                   'threshold' : 10,
                   'nonmaxSuppresion': true
               },
               "BriskFeatureDetector" : {
                   'thresh' : 30,
                   'octaves' : 3,
                   'patternScale' : 1.0
               },
               "OrbFeatureDetector" : {
                   'nfeatures' : 500,
                   'scaleFactor' : 1.2,
                   'nlevels' : 8,
                   'edgeThreshold' : 31,
                   'firstLevel' : 0,
                   'WTA_K' : 2,
                   'scoreType' : 0
               },
               "MSerFeatureDetector" : {
                   'delta' : 5,
                   'minArea' : 60,
                   'maxArea' : 14400,
                   'maxVariation' : 0.25,
                   'minDiversity' : 0.2,
                   'maxEvolution' : 200,
                   'areaThreshold' : 1.01,
                   'minMargin' : 0.003,
                   'edgeBlurSize' : 5
               }
            }
        }
        Component.onCompleted: {
            staticLoad(root.stateId)
            listView.model        = root.model
            listView.currentIndex = globalConfig.item.selectedIndex

            chosenItemText.text   = root.model[globalConfig.item.selectedIndex].name
            chosenItem.detector   = root.model[globalConfig.item.selectedIndex].detector
            chosenItem.panelModel = root.model[globalConfig.item.selectedIndex].configurationFields
        }
    }

    signal configurationUpdate()

    function updateConfiguration(detectorKey, propertyKey, value){
        if ( globalConfig.item.configuration[detectorKey][propertyKey] !== value ){
            globalConfig.item.configuration[detectorKey][propertyKey] = value
            if ( selectedDetector ){
                selectedDetector.params = globalConfig.item.configuration[detectorKey]
            }
            root.configurationUpdate()
        }
    }

    property Mat   detectorInput : null

    property alias selectedName: chosenItemText.text
    property alias selectedDetector: chosenItem.detector

    property alias selectedPanelModel: chosenItem.panelModel
    property alias selectedIndex: listView.currentIndex

    property color backgroundColor: "#061a29"
    property color highlightColor: "#071825"
    property color textColor: "#fff"

    property int dropBoxHeight: 120

    signal comboClicked

    z: 200
    
    width: 500
    height : 35

    Rectangle {
        id: chosenItem
        width: parent.width
        height: root.height
        color: chosenItemMouse.containsMouse ? root.highlightColor : root.backgroundColor

        property FeatureDetector detector: null
        onDetectorChanged: {
            detector.params = globalConfig.item.configuration[selectedName]
            detector.input  = detectorInput
        }

        property var panelModel: []

        Text{
            id: chosenItemText
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            text: ''
            font.family: "Ubuntu Mono, Helvetica, sans-serif"
            font.pixelSize: 13
            color: root.textColor
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
        z: 201

        ScrollView{
            z : 202
            width : parent.width
            height : root.dropBoxHeight
            style : ScrollViewStyle {
                transientScrollBars: false
                handle: Item {
                    implicitWidth: 10
                    implicitHeight: 10
                    Rectangle {
                        color: "#0b1f2e"
                        anchors.fill: parent
                    }
                }
                scrollBarBackground: Item{
                    implicitWidth: 10
                    implicitHeight: 10
                    Rectangle{
                        anchors.fill: parent
                        color: "#091823"
                    }
                }
                decrementControl: null
                incrementControl: null
                frame: Rectangle{color: "transparent"}
                corner: Rectangle{color: "#091823"}
            }

            ListView {
                id: listView
                model: []
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
                        color: root.textColor
                        font.family: "Ubuntu Mono, Arial, Helvetica, sans-serif"
                        font.pixelSize: 13
                    }
                    MouseArea{
                        id: delegateArea
                        anchors.fill: parent
                        onClicked: {
                            root.state = ""
                            var prevSelection   = chosenItemText.text
                            chosenItemText.text = modelData.name
                            if (chosenItemText.text != prevSelection){
                                chosenItem.panelModel = modelData.configurationFields
                                chosenItem.detector   = modelData.detector
                                root.comboClicked()
                            }
                            listView.currentIndex = index
                            globalConfig.item.selectedIndex = index
                        }
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                    }
                }
            }
        }
    }

    states: State {
        name: "dropDown"
        PropertyChanges {
            target: dropDown
            height: root.dropBoxHeight
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


