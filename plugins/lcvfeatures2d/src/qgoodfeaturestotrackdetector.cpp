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

#include "qgoodfeaturestotrackdetector.h"
#include "opencv2/features2d/features2d.hpp"
/// \private
QGoodFeaturesToTrackDetector::QGoodFeaturesToTrackDetector(QQuickItem *parent)
    : QFeatureDetector(new cv::GoodFeaturesToTrackDetector, parent)

{
}

QGoodFeaturesToTrackDetector::~QGoodFeaturesToTrackDetector(){
}

void QGoodFeaturesToTrackDetector::initialize(const QVariantMap &settings){
    int maxCorners         = 1000;
    double qualityLevel    = 0.01;
    double minDistance     = 1;
    int blockSize          = 3;
    bool useHarrisDetector = false;
    double k               = 0.04;

    if ( settings.contains("maxCorners") )
        maxCorners = settings["maxCorners"].toInt();
    if ( settings.contains("qualityLevel") )
        qualityLevel = settings["qualityLevel"].toDouble();
    if ( settings.contains("minDistance") )
        minDistance = settings["minDistance"].toDouble();
    if ( settings.contains("blockSize") )
        blockSize = settings["blockSize"].toInt();
    if ( settings.contains("useHarrisDetector") )
        useHarrisDetector = settings["useHarrisDetector"].toBool();
    if ( settings.contains("k") )
        k = settings["k"].toDouble();

    initializeDetector(new cv::GoodFeaturesToTrackDetector(
        maxCorners, qualityLevel, minDistance, blockSize, useHarrisDetector, k
    ));
}
