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
import lcvcore 1.0
import lcvcontrols 1.0
import lcvfeatures2d 1.0

Column{
    anchors.left: parent.left
    anchors.right : parent.right
    
    FeatureObjectList{
        id : featureObjectList
        imageSource : ImRead{
            file : "/home/dinu/Work/livecv/samples/_images/object_101_piano_train1.jpg"
        }
        onObjectAdded : {
            var oc = keypointHomography.objectCorners
            oc.push(points)
            keypointHomography.objectCorners = oc
            drawMatches.keypoints2 = featureObjectList.objectList.keypoints[featureObjectList.objectList.keypoints.length - 1]
            matchesToLocalKeypoint.trainKeypointVectors = featureObjectList.objectList.keypoints
            descriptorMatcher.add(descriptors)
            descriptorMatcher.train()
        }
    }
    
    ImRead{
        id : queryImage
        file : "/home/dinu/Work/livecv/samples/_images/object_101_piano_train1.jpg"
        visible : false
    }
    FastFeatureDetector{
        id : queryFeatureDetect
        input : queryImage.output
    }
    BriefDescriptorExtractor{
        id : queryFeatureExtract
        keypoints : queryFeatureDetect.keypoints
    }
    
    BruteForceMatcher{
        id : descriptorMatcher
        queryDescriptors : queryFeatureExtract.descriptors
        knn : 2
    }
    
    DescriptorMatchFilter{
        id : descriptorMatchFilter
        matches1to2 : descriptorMatcher.matches
        minDistanceCoeff : 2.5
        nndrRatio : 0.8
    }
    
    MatchesToLocalKeypoint{
        id : matchesToLocalKeypoint
        matches1to2 : descriptorMatchFilter.matches1to2Out
        trainKeypointVectors : featureObjectList.objectList.keypoints
        queryKeypointVector : queryFeatureDetect.keypoints
    }
    
    KeypointHomography{
        id : keypointHomography
        queryImage : queryImage.output
        keypointsToScene : matchesToLocalKeypoint.output
        objectCorners : []
    }
    
    DrawMatches{
        id : drawMatches
        keypoints1 : queryFeatureDetect.keypoints
        keypoints2 : null
        matches1to2 : descriptorMatchFilter.matches1to2Out
        matchIndex : 0
        Component.onCompleted : {
            console.log(featureObjectList.objectList.keypoints.length)
        }
    }
    
}

