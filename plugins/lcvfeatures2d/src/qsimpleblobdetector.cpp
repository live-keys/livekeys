/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#include "qsimpleblobdetector.h"
#include "opencv2/features2d.hpp"
QSimpleBlobDetector::QSimpleBlobDetector(QQuickItem* parent)
    : QFeatureDetector(cv::SimpleBlobDetector::create(), parent)
{
}

QSimpleBlobDetector::~QSimpleBlobDetector(){
}

void QSimpleBlobDetector::initialize(const QVariantMap &settings){
    cv::SimpleBlobDetector::Params params;

    params.filterByInertia     = false;
    params.filterByConvexity   = false;
    params.filterByColor       = false;
    params.filterByCircularity = false;
    params.filterByArea        = false;

    if ( settings.contains("thresholdStep") )
        params.thresholdStep       = settings["thresholdStep"].toFloat();
    if ( settings.contains("minThreshold") )
        params.minThreshold        = settings["minThreshold"].toFloat();
    if ( settings.contains("maxThreshold") )
        params.maxThreshold        = settings["maxThreshold"].toFloat();
    if ( settings.contains("minRepeatability") )
        params.minRepeatability    = static_cast<size_t>(settings["minRepeatability"].toLongLong());
    if ( settings.contains("minDistBetweenBlobs") )
        params.minDistBetweenBlobs = settings["minDistBetweenBlobs"].toFloat();

    if ( settings.contains("blobColor") ){
        params.filterByColor = true;
        params.blobColor     = static_cast<uchar>(settings["blobColor"].toUInt());
    }

    if ( settings.contains("minArea") ){
        params.filterByArea = true;
        params.minArea      = settings["minArea"].toFloat();
    }
    if ( settings.contains("maxArea") ){
        params.filterByArea = true;
        params.maxArea      = settings["maxArea"].toFloat();
    }

    if ( settings.contains("minCircularity") ){
        params.filterByCircularity = true;
        params.minCircularity      = settings["minCircularity"].toFloat();
    }
    if ( settings.contains("maxCircularity") ){
        params.filterByCircularity = true;
        params.maxCircularity      = settings["maxCircularity"].toFloat();
    }

    if ( settings.contains("minInertiaRatio") ){
        params.filterByInertia = true;
        params.minInertiaRatio = settings["minInertiaRatio"].toFloat();
    }
    if ( settings.contains("maxInertiaRatio") ){
        params.filterByInertia = true;
        params.maxInertiaRatio = settings["maxInertiaRatio"].toFloat();
    }

    if ( settings.contains("minConvexity") ){
        params.filterByConvexity = true;
        params.minConvexity      = settings["minConvexity"].toFloat();
    }
    if ( settings.contains("maxConvexity") ){
        params.filterByConvexity = true;
        params.maxConvexity      = settings["maxConvexity"].toFloat();
    }

    initializeDetector(cv::SimpleBlobDetector::create(params));
}
