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
import live 1.0
import lcvcore 1.0
import lcvfeatures2d 1.0 

Column{   
    id : root 
    anchors.left: parent.left
    anchors.right : parent.right
        
    property FeatureDetector trainFeatureDetector : FastFeatureDetector{}
    property DescriptorExtractor trainDescriptorExtractor : OrbDescriptorExtractor{}
    
    property FeatureDetector queryFeatureDetector : FastFeatureDetector{}
    Connections{
        target : queryFeatureDetector
        onKeypointsChanged : {
            queryDescriptorExtractor.keypoints = queryFeatureDetector.keypoints
        }
    }

    property DescriptorExtractor queryDescriptorExtractor : OrbDescriptorExtractor{
        keypoints : queryFeatureDetector.keypoints
    }
    onQueryDescriptorExtractorChanged : {
        queryDescriptorExtractor.keypoints = queryFeatureDetector.keypoints
    }
     
    property var imageSource : ImRead{}
    property var querySource : ImRead{}
    Connections{
        target : querySource
        onOutputChanged : queryFeatureDetector.input = querySource.output
    }

    property double minMatchDistanceCoeff : 2.5
    property double matchNndrRatio : 0.8

    function addObject(region, width, height, x, y){
        featureObjectList.addObject(region, width, height, x, y)
    }
    signal objectListCreated()

    property alias objectList: featureObjectList.objectList

    property KeyPointVector drawQueryKeypoints : null
    property int drawMatchIndex : 0
    
    FeatureObjectList{
        id : featureObjectList
        imageSource : root.imageSource
        featureDetector : root.trainFeatureDetector
        descriptorExtractor : root.trainDescriptorExtractor
        onObjectAdded : {
            var oc = keypointHomography.objectCorners
            oc.push(points)
            keypointHomography.objectCorners = oc
             
            var ocolors = keypointHomography.objectColors
            ocolors.push(color)
            keypointHomography.objectColors = ocolors

            drawMatches.keypoints2  = featureObjectList.objectList.keypoints[featureObjectList.objectList.keypoints.length - 1]
            drawMatches.matchIndex = featureObjectList.objectList.keypoints.length - 1
            matchesToLocalKeypoint.trainKeypointVectors = featureObjectList.objectList.keypoints

            descriptorMatcherComponent.item.add(descriptors)
            descriptorMatcherComponent.item.train()
            descriptorMatchFilter.matches1to2 = descriptorMatcherComponent.item.matches
        }
        onObjectListLoaded : {
            matchesToLocalKeypoint.trainKeypointVectors = featureObjectList.objectList.keypoints
            keypointHomography.objectCorners = corners
            keypointHomography.objectColors  = colors

            if ( featureObjectList.objectList.keypoints.length > 0 ){
                drawMatches.keypoints2  = featureObjectList.objectList.keypoints[featureObjectList.selectedIndex]
                drawMatches.matchIndex = featureObjectList.selectedIndex
            }
        }
        onSelectedIndexChanged: {
            if ( featureObjectList.objectList.keypoints.length > 0 ){
                drawMatches.keypoints2 = featureObjectList.objectList.keypoints[featureObjectList.selectedIndex]
                drawMatches.matchIndex     = featureObjectList.selectedIndex
            }
        }
        onObjectListCreated: root.objectListCreated()
    }
    
    property string stateId : "descriptorMatcher"
    function staticLoad(id){
        descriptorMatcherComponent.staticLoad(id)
    }

    StaticLoader{
        id : descriptorMatcherComponent

        source : BruteForceMatcher{
            id : descriptorMatcher
            knn : 2
            params : {
                'normType' : BruteForceMatcher.NORM_HAMMING
            }
        }
        Component.onCompleted: {
            staticLoad(root.stateId)
            item.queryDescriptors = root.queryDescriptorExtractor.descriptors
            descriptorMatchFilter.matches1to2 = item.matches
        }
        Connections{
            target : root.queryDescriptorExtractor
            onDescriptorsChanged : {
                if ( descriptorMatcherComponent.item ){
                    descriptorMatcherComponent.item.queryDescriptors = root.queryDescriptorExtractor.descriptors
                    descriptorMatchFilter.matches1to2 = descriptorMatcherComponent.item.matches
                }
            }
        }
    }
    
    DescriptorMatchFilter{
        id : descriptorMatchFilter
        onMatches1to2OutChanged :
            matchesToLocalKeypoint.setQueryWithMatches(root.queryFeatureDetector.keypoints, matches1to2Out)
        minDistanceCoeff : root.minMatchDistanceCoeff
        nndrRatio : root.matchNndrRatio
    }
    
    MatchesToLocalKeypoint{
        id : matchesToLocalKeypoint
    }
    
    Row{

        KeypointHomography{
            id : keypointHomography
            queryImage : querySource.output
            keypointsToScene : matchesToLocalKeypoint.output
            objectCorners : []
            objectColors : []
        }

        DrawMatches{
            id : drawMatches
            keypoints1 : root.queryFeatureDetector.keypoints
            keypoints2 : null
            matches1to2 : descriptorMatchFilter.matches1to2Out
            matchIndex : 0//drawMatches.matchIndex
            width : implicitWidth / 2
            height : implicitHeight / 2
        }
    }
    
}

