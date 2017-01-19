/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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
import lcvcore 1.0
import lcvimgproc 1.0
import lcvfeatures2d 1.0

Column{
    id: container

    width: 400
    height: 600
    
    property color headerColor : "#061a29"
    property color headerTextColor : "#fff"
    
    property color backgroundColor : "#081520"
    property color itemBackgroundColor : "#081520"
    property color selectedItemBackgroundColor : "#071825"
        
    Rectangle{
        width: container.width
        height: 35
        color: container.headerColor
        Text{
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            text: 'Total Keypoints: ' + keypointView.model.length

            color: container.headerTextColor
            font.pixelSize: 14
            font.family: 'Ubuntu Mono, Courier New, Courier'
            font.bold: true
        }
    }
        
    property MatDraw keypointHighlighter : null
    property FeatureDetector detector : null
    property DescriptorExtractor extractor : null
    property variant descriptorValues : extractor ? extractor.descriptors.data().values() : []

    signal keypointMouseEnter(int index, point pt)
    signal keypointMouseExit(int index, point pt)
    signal keypointSelected(int index, point pt)

    property alias currentIndex : keypointView.currentIndex
        
    ScrollView{
        width: container.width
        height: container.height - 30
            
        style: LiveCVScrollStyle{}
            
        ListView{
            id : keypointView
            model : container.extractor
                      ? (container.extractor.keypoints ? container.extractor.keypoints.keyPointData() : [])
                      : container.detector
                        ? container.detector.keypoints.keyPointData()
                        : []
            
            width: parent.width
            height: parent.height
            clip: true
            currentIndex : -1 
            
            delegate: Rectangle{
                id: keypointContainer
                
                property point pt : modelData.pt ? modelData.pt : '0x0'
                
                width: keypointView.width
                height: ListView.isCurrentItem ? 60 + descriptorData.height : 40
                
                color: ListView.isCurrentItem  ? 
                    container.selectedItemBackgroundColor : keypointMouse.containsMouse ? 
                        container.selectedItemBackgroundColor : container.itemBackgroundColor
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
                    font.family: 'Ubuntu Mono, Courier New, Courier'
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
                    font.family: 'Ubuntu Mono, Courier New, Courier'
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
                            container.keypointHighlighter.circle(modelData.pt, 10, "#33cc33", 2, 8, 0);
                        container.keypointMouseEnter(index, modelData.pt)
                    }
                    onExited : {
                        container.keypointHighlighter.cleanUp();
                        if ( keypointView.currentItem.pt )
                            container.keypointHighlighter.circle(keypointView.currentItem.pt, 10, "#99cc88", 2, 8, 0);
                        container.keypointMouseExit(index, modelData.pt)
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
                        container.keypointSelected(index, modelData.pt)
                    }
                }
            }
        }
    }
}
