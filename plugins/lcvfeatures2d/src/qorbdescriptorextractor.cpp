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

#include "qorbdescriptorextractor.h"
#include "opencv2/features2d.hpp"

QOrbDescriptorExtractor::QOrbDescriptorExtractor(QQuickItem *parent)
    : QDescriptorExtractor(cv::ORB::create(), parent)
{
}

QOrbDescriptorExtractor::~QOrbDescriptorExtractor(){
}

void QOrbDescriptorExtractor::initialize(const QVariantMap &params){
    int patchSize = 32;
    if ( params.contains("patchSize") )
        patchSize = params["patchSize"].toInt();

    initializeExtractor(
        cv::ORB::create(500, 1.2f, 8, 31, 0, 2, cv::ORB::HARRIS_SCORE, patchSize)
    );

}

