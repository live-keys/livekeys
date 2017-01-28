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

#include "qmserfeaturedetector.h"
#include "opencv2/features2d.hpp"

QMSerFeatureDetector::QMSerFeatureDetector(QQuickItem *parent)
    : QFeatureDetector(cv::MSER::create(), parent)
{
}

QMSerFeatureDetector::~QMSerFeatureDetector(){
}

void QMSerFeatureDetector::initialize(const QVariantMap& settings){
    int delta            = 5;
    int minArea          = 60;
    int maxArea          = 14400;
    float maxVariation   = 0.25;
    float minDiversity   = 0.2f;
    int maxEvolution     = 200;
    double areaThreshold = 1.01;
    double minMargin     = 0.003;
    int edgeBlurSize     = 5;

    if ( settings.contains("delta") )
        delta = settings["delta"].toInt();
    if ( settings.contains("minArea") )
        minArea = settings["minArea"].toInt();
    if ( settings.contains("maxArea") )
        maxArea = settings["maxArea"].toInt();
    if ( settings.contains("maxVariation") )
        maxVariation = settings["maxVariation"].toFloat();
    if ( settings.contains("minDiversity") )
        minDiversity = settings["minDiversity"].toFloat();
    if ( settings.contains("maxEvolution") )
        maxEvolution = settings["maxEvolution"].toInt();
    if ( settings.contains("areaThreshold") )
        areaThreshold = settings["areaThreshold"].toDouble();
    if ( settings.contains("minMargin") )
        minMargin = settings["minMargin"].toDouble();
    if ( settings.contains("edgeBlurSize") )
        edgeBlurSize = settings["edgeBlurSize"].toInt();

    initializeDetector(cv::MSER::create(
       delta,
       minArea,
       maxArea,
       maxVariation,
       minDiversity,
       maxEvolution,
       areaThreshold,
       minMargin,
       edgeBlurSize
    ));
}

