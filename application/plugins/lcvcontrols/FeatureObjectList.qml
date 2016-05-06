/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
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
import lcvcore 1.0
import lcvcontrols 1.0
import lcvfeatures2d 1.0

Rectangle{
    id : root
    
    anchors.left: parent.left
    anchors.right: parent.right
    color: "transparent"
    
    height : 200
    
    property var imageSource : null
    property FeatureDetector featureDetector : FastFeatureDetector{}
    property DescriptorExtractor descriptorExtractor : BriefDescriptorExtractor{}
    
    signal objectAdded(Mat descriptors, var points, var color)
    signal objectListLoaded(MatList list, var keypoints, var corners, var colors)
    
    property alias objectList : objectListComponent.item

    GlobalItem{
        id : objectListComponent
        stateId : "objectListComponent"
        source : Item{
            property MatList objectList : MatList{}
            property variant keypoints : new Array()
            property variant corners : new Array()
            property variant colors : new Array()
        }
        Component.onCompleted: {
            trainImages.model = item.objectList
            root.objectListLoaded(item.objectList, item.keypoints, item.corners, item.colors)
        }
    }
    
    ScrollView{
        width : parent.width
        height: parent.height
        anchors.left: parent.left
        anchors.leftMargin : 50
        style : LiveCVScrollStyle{}
        
        Row{
            height: root.height

            Repeater{
                id : trainImages
                property int selectedIndex : 0
                height : root.height

                delegate : MatView{
                    id : matView
                    height : parent.height
                    width : (parent.height / mat.dataSize().height) * mat.dataSize().width
                    mat : modelData
                    Rectangle{
                        color : objectListComponent.item.colors[index].toString()
                        width : matView.width
                        height : 3
                    }
                }
            }
        }

    }
    
    SelectionWindow{
        id : selectionWindow
        minimumWidth : 300
        
        onRegionSelected : {
            root.featureDetector.input = region

            var generatedColor = Qt.hsla(Math.random(), Math.random() * 0.5 + 0.5, Math.random() * 0.5 + 0.5, 1)
            var keypoints = root.featureDetector.keypoints.createOwnedObject()
            var corners   = [Qt.point(0, 0), Qt.point(width, 0), Qt.point(width, height), Qt.point(0, height)]

            objectListComponent.item.colors.push(generatedColor)
            objectListComponent.item.keypoints.push(keypoints)
            objectListComponent.item.corners.push(corners)
            objectListComponent.item.objectList.appendMat(root.featureDetector.output.createOwnedObject())

            root.descriptorExtractor.keypoints = keypoints
            root.objectAdded(
                root.descriptorExtractor.descriptors,
                corners,
                generatedColor
            )
        }
    }


    TextButton{
        anchors.left: parent.left
        width : 50
        height : parent.height
        text : "+"
        fontPixelSize : 30
        onClicked : {
            if ( root.imageSource !== null )
                selectionWindow.mat = root.imageSource.output.cloneMat()
            selectionWindow.show()
        }
    }
    
}

