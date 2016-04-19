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

#include "qbriskfeaturedetector.h"
#include "opencv2/features2d/features2d.hpp"

QBriskFeatureDetector::QBriskFeatureDetector(QQuickItem *parent) :
    QFeatureDetector(new cv::BRISK, parent)
{
}

QBriskFeatureDetector::~QBriskFeatureDetector(){
}

void QBriskFeatureDetector::initialize(const QVariantMap &settings){
    int thresh  = 30;
    int octaves = 3;
    float patternScale = 1.0f;
    if ( settings.contains("thresh") )
        thresh = settings["thresh"].toInt();
    if ( settings.contains("octaves") )
        thresh = settings["octaves"].toInt();
    if ( settings.contains("patternScale") )
        patternScale = settings["patternScale"].toFloat();
    initializeDetector(new cv::BRISK(thresh, octaves, patternScale));
}
