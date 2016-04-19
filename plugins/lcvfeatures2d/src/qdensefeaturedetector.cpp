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

#include "qdensefeaturedetector.h"
#include "opencv2/features2d/features2d.hpp"

QDenseFeatureDetector::QDenseFeatureDetector(QQuickItem *parent) :
    QFeatureDetector(new cv::DenseFeatureDetector, parent)
{
}


QDenseFeatureDetector::~QDenseFeatureDetector(){
}

void QDenseFeatureDetector::initialize(const QVariantMap &settings){
    float initFeatureScale     = 1.f;
    int featureScaleLevels     = 1;
    float featureScaleMul      = 0.1f;
    int initXyStep             = 6;
    int initImgBound           = 0;
    bool varyXyStepWithScale   = true;
    bool varyImgBoundWithScale = false;

    if ( settings.contains("initFeatureScale") )
        initFeatureScale = settings["initFeatureScale"].toFloat();
    if ( settings.contains("featureScaleLevels") )
        featureScaleLevels = settings["featureScaleLevels"].toInt();
    if ( settings.contains("featureScaleMul") )
        featureScaleMul = settings["featureScaleMul"].toFloat();
    if ( settings.contains("initXyStep") )
        initXyStep = settings["initXyStep"].toInt();
    if ( settings.contains("initImgBound") )
        initImgBound = settings["initImgBound"].toInt();
    if ( settings.contains("varyXyStepWithScale") )
        varyXyStepWithScale = settings["varyXyStepWithScale"].toBool();
    if ( settings.contains("varyImgBoundWithScale") )
        varyImgBoundWithScale = settings["varyImgBoundWithScale"].toBool();

    initializeDetector(new cv::DenseFeatureDetector(
        initFeatureScale, featureScaleLevels, featureScaleMul, initXyStep, initImgBound,
        varyXyStepWithScale, varyImgBoundWithScale
    ));
}
