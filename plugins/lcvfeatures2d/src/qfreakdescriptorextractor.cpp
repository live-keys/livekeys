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

#include "qfreakdescriptorextractor.h"
#include "opencv2/features2d.hpp"

QFreakDescriptorExtractor::QFreakDescriptorExtractor(QQuickItem *parent) :
    QDescriptorExtractor(new cv::FREAK, parent)
{
}

QFreakDescriptorExtractor::~QFreakDescriptorExtractor(){
}

void QFreakDescriptorExtractor::initialize(const QVariantMap &params){
    bool orientationNormalized = true;
    bool scaleNormalized       = true;
    float patternScale         = 22.0f;
    int nOctaves               = 4;

    if ( params.contains("orientationNormalized") )
        orientationNormalized = params["orientationNormalized"].toBool();
    if ( params.contains("scaleNormalized") )
        scaleNormalized = params["scaleNormalized"].toBool();
    if ( params.contains("patternScale") )
        patternScale = params["patternScale"].toFloat();
    if ( params.contains("nOctaves") )
        nOctaves = params["nOctaves"].toInt();

    initializeExtractor(new cv::FREAK(orientationNormalized, scaleNormalized, patternScale, nOctaves));
}
