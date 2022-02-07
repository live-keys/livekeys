/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

#include "qfastfeaturedetector.h"
#include "opencv2/features2d.hpp"

QFastFeatureDetector::QFastFeatureDetector(QQuickItem *parent)
    : QFeatureDetector(cv::FastFeatureDetector::create(), parent)
{
}

QFastFeatureDetector::~QFastFeatureDetector(){
}

void QFastFeatureDetector::initialize(const QVariantMap& settings){
    int threshold = 10;
    bool nonmaxSuppression = true;

    if ( settings.contains("threshold") )
        threshold = settings["threshold"].toInt();
    if ( settings.contains("nonmaxSuppresion") )
        nonmaxSuppression = settings["nonmaxSuppresion"].toBool();

    initializeDetector(cv::FastFeatureDetector::create(threshold, nonmaxSuppression));
}
