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

#include "qorbfeaturedetector.h"
#include "opencv2/features2d.hpp"
/// \private
QOrbFeatureDetector::QOrbFeatureDetector(QQuickItem *parent) :
    QFeatureDetector(cv::ORB::create(), parent)
{
}

QOrbFeatureDetector::~QOrbFeatureDetector()
{
}

void QOrbFeatureDetector::initialize(const QVariantMap &settings){
    int nfeatures     = 500;
    float scaleFactor = 1.2f;
    int nlevels       = 8;
    int edgeThreshold = 31;
    int firstLevel    = 0;
    int WTA_K         = 2;
    cv::ORB::ScoreType scoreType = cv::ORB::HARRIS_SCORE;

    if ( settings.contains("nfeatures") )
        nfeatures = settings["nfeatures"].toInt();
    if ( settings.contains("scaleFactor") )
        scaleFactor = settings["scaleFactor"].toFloat();
    if ( settings.contains("nlevels") )
        nlevels = settings["nlevels"].toInt();
    if ( settings.contains("edgeThreshold") )
        edgeThreshold = settings["edgeThreshold"].toInt();
    if ( settings.contains("firstLevel") )
        firstLevel = settings["firstLevel"].toInt();
    if ( settings.contains("WTA_K") )
        WTA_K = settings["WTA_K"].toInt();
    if ( settings.contains("scoreType") )
        scoreType = static_cast<cv::ORB::ScoreType>(settings["scoreType"].toInt());

    initializeDetector(cv::ORB::create(nfeatures, scaleFactor, nlevels, edgeThreshold, firstLevel, WTA_K, scoreType));
}
