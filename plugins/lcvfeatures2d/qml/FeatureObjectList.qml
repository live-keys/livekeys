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
import live 1.0
import lcvcore 1.0 as Cv
import base 1.0
import lcvfeatures2d 1.0

Rectangle{
    id : root

    anchors.left: parent.left
    anchors.right: parent.right
    color: "transparent"

    height : 200

    property var imageSource : null
    property FeatureDetector featureDetector : FastFeatureDetector{}
    property DescriptorExtractor descriptorExtractor : BriskDescriptorExtractor{}

    signal objectAdded(var descriptors, var points, var color)
    signal objectListLoaded(ObjectList list, var keypoints, var corners, var colors)
    signal objectListCreated()

    function addObject(region, x, y, width, height){

        root.featureDetector.input = region

        var generatedColor = Qt.hsla(Math.random(), Math.random() * 0.3 + 0.7, Math.random() * 0.25 + 0.5, 1)
        var keypoints = root.featureDetector.keypoints.createOwnedObject()
        var corners   = [Qt.point(0, 0), Qt.point(width, 0), Qt.point(width, height), Qt.point(0, height)]

        objectListComponent.item.colors.push(generatedColor)
        objectListComponent.item.keypoints.push(keypoints)
        objectListComponent.item.corners.push(corners)
        objectListComponent.item.objectListModel.appendData(root.featureDetector.output.createOwnedObject())

        root.descriptorExtractor.keypoints = keypoints
        root.objectAdded(
            root.descriptorExtractor.descriptors,
            corners,
            generatedColor
        )
    }

    property alias objectList : objectListComponent.item
    property alias selectedIndex : trainImages.selectedIndex

    property string stateId : "objectListComponent"

    StaticLoader{
        id : objectListComponent

        property bool itemCreated : false

        source : Item{

            property var objectList : Cv.MatOp.createMatList()
            property var objectListModel : objectList.model()

            property var keypoints : new Array()
            property var corners : new Array()
            property var colors : new Array()
        }
        onItemChanged : itemCreated = true

        Component.onCompleted: {
            staticLoad(root.stateId)
            if ( item.objectList )
                trainImages.model = item.objectListModel
            root.objectListCreated()
            root.objectListLoaded(item.objectList, item.keypoints, item.corners, item.colors)
        }
    }

    ScrollView{
        width : parent.width
        height: parent.height
        anchors.left: parent.left
        anchors.leftMargin : 50
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

        Row{
            height: root.height

            Repeater{
                id : trainImages
                property int selectedIndex : 0
                height : root.height
                delegate : Cv.MatView{
                    id : matView
                    mat : model.item
                    width : (parent.height / mat.dimensions().height) * mat.dimensions().width
                    height : parent.height
                    Rectangle{
                        color : objectListComponent.item.colors[index].toString()
                        width : matView.width
                        height : trainImages.selectedIndex === index ? 10 : 5
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: trainImages.selectedIndex = index
                    }
                }
            }
        }

    }

    SelectionWindow{
        id : selectionWindow
        minimumWidth : 300

        onRegionSelected : root.addObject(region, x, y, width, height)
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

