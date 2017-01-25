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

#include "qbruteforcematcher.h"
#include "opencv2/features2d/features2d.hpp"
#include <QVariant>

QBruteForceMatcher::QBruteForceMatcher(QQuickItem* parent)
    : QDescriptorMatcher(0, parent)
{
}

QBruteForceMatcher::~QBruteForceMatcher(){
}

void QBruteForceMatcher::initialize(const QVariantMap &params){
    int normType = cv::NORM_L2;
    bool crossCheck = false;

    if ( params.contains("normType") )
        normType = params["normType"].toInt();
    if ( params.contains("crossCheck") )
        crossCheck = params["crossCheck"].toBool();

    initializeMatcher(new cv::BFMatcher(normType, crossCheck));
}
