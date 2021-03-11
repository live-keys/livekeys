/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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
            property string name: "BriskDescriptorExtractor"
            property DescriptorExtractor extractor: BriskDescriptorExtractor{}
            property list<Component> configurationFields
        },
        QtObject{
            property string name: "OrbDescriptorExtractor"
            property DescriptorExtractor extractor: OrbDescriptorExtractor{}
            property list<Component> configurationFields : [
                Component{
                    ConfigurationField{
                        label : "Patch Size"
                        editor : InputBox{
                            text: globalConfig.item.configuration["OrbDescriptorExtractor"]["patchSize"]
                            onTextChanged: root.updateConfiguration("OrbDescriptorExtractor", "patchSize", text)
                        }
                    }
                }
            ]
        },
        QtObject{
            property string name: "FreakDescriptorExtractor"
            property DescriptorExtractor extractor: OrbDescriptorExtractor{}
            property list<Component> configurationFields : [
                Component{
                    ConfigurationField{
                        label : "Orientation Normalized"
                        editor : InputBox{
                            text: globalConfig.item.configuration["FreakDescriptorExtractor"]["orientationNormalized"]
                            onTextChanged: root.updateConfiguration("FreakDescriptorExtractor", "orientationNormalized", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "Scale Normalized"
                        editor : InputBox{
                            text: globalConfig.item.configuration["FreakDescriptorExtractor"]["scaleNormalized"]
                            onTextChanged: root.updateConfiguration("FreakDescriptorExtractor", "scaleNormalized", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "Pattern Scale"
                        editor : InputBox{
                            text: globalConfig.item.configuration["FreakDescriptorExtractor"]["patternScale"]
                            onTextChanged: root.updateConfiguration("FreakDescriptorExtractor", "patternScale", text)
                        }
                    }
                },
                Component{
                    ConfigurationField{
                        label : "N Octaves"
                        editor : InputBox{
                            text: globalConfig.item.configuration["FreakDescriptorExtractor"]["nOctaves"]
                            onTextChanged: root.updateConfiguration("FreakDescriptorExtractor", "nOctaves", text)
                        }
                    }
                }
            ]
        }
    ]

    function staticLoad(id){
        globalConfig.staticLoad(id)
    }

    property string stateId : "descriptorExtractorConfig"

    StaticLoader{
        id : globalConfig
        source : Item{
            id : globalConfigItem

            property var configuration : {
                "BriskDescriptorExtractor" : {
                },
                "OrbDescriptorExtractor" : {
                  'patchSize' : 32
                },
                "FreakDescriptorExtractor" : {
                  'orientationNormalized' : true,
                  'scaleNormalized' : true,
                  'patternScale' : 22.0,
                  'nOctaves' : 4
                }
            }
            property int selectedIndex : 0
        }
        Component.onCompleted: {
            staticLoad(root.stateId)
            listView.model        = root.model
            listView.currentIndex = globalConfig.item.selectedIndex

            chosenItemText.text   = root.model[globalConfig.item.selectedIndex].name
            chosenItem.extractor  = root.model[globalConfig.item.selectedIndex].extractor
            chosenItem.panelModel = root.model[globalConfig.item.selectedIndex].configurationFields
        }
    }

    signal configurationUpdate()

    function updateConfiguration(extractorKey, propertyKey, value){
        if ( globalConfig.item.configuration[extractorKey][propertyKey] !== value ){
            globalConfig.item.configuration[extractorKey][propertyKey] = value
            if ( selectedExtractor ){
                selectedExtractor.params = globalConfig.item.configuration[extractorKey]
            }
            root.configurationUpdate()
        }
    }

    property FeatureDetector detector: null
    onDetectorChanged: selectedExtractor.keypoints = detector ? detector.keypoints : null
    Connections{
        target : detector
        function onKeypointsChanged(){ selectedExtractor.keypoints = detector.keypoints }
    }

    property alias selectedName: chosenItemText.text
    property alias selectedExtractor: chosenItem.extractor

    property alias selectedPanelModel: chosenItem.panelModel
    property alias selectedIndex: listView.currentIndex

    property color backgroundColor: "#061a29"
    property color highlightColor: "#071825"
    property color textColor: "#fff"

    property int dropBoxHeight: 120

    signal comboClicked

    z: 100

    width: 500
    height : 35

    Rectangle {
        id: chosenItem
        width: parent.width
        height: root.height
        color: chosenItemMouse.containsMouse ? root.highlightColor : root.backgroundColor

        property DescriptorExtractor extractor: null
        onExtractorChanged: {
            extractor.params    = globalConfig.item.configuration[selectedName]
            extractor.keypoints = detector ? detector.keypoints : null
        }

        property var panelModel: []

        Text{
            id: chosenItemText
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            text: ''
            font.family: "Ubuntu Mono, Arial, Helvetica, sans-serif"
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

    Rectangle{
        id: dropDown
        width: root.width
        height: 0
        clip: true
        anchors.top: chosenItem.bottom
        color: root.backgroundColor

        ScrollView{
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
                                chosenItem.extractor  = modelData.extractor
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

