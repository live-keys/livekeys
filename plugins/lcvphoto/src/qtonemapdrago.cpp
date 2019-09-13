/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#include "qtonemapdrago.h"

QTonemapDrago::QTonemapDrago(QQuickItem *parent)
    : QTonemap(cv::createTonemapDrago(), parent)
{
}

void QTonemapDrago::initialize(const QVariantMap &options){
    float gamma = 1.0f;
    float saturation =   1.0f;
    float bias = 0.85f;

    if ( options.contains("gamma") )
        gamma = options["gamma"].toFloat();
    if ( options.contains("saturation") )
        saturation = options["saturation"].toFloat();
    if ( options.contains("bias") )
        bias = options["bias"].toFloat();

    initializeTonemapper(cv::createTonemapDrago(gamma, saturation, bias));
}
