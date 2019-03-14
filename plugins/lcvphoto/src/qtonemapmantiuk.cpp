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

#include "qtonemapmantiuk.h"

QTonemapMantiuk::QTonemapMantiuk(QQuickItem *parent)
    : QTonemap(cv::createTonemapMantiuk(), parent)
{
}

void QTonemapMantiuk::initialize(const QVariantMap &options){
    float gamma = 1.0f;
    float scale = 0.7f;
    float saturation = 1.0f;

    if ( options.contains("gamma") )
        gamma = options["gamma"].toFloat();
    if ( options.contains("scale") )
        scale = options["scale"].toFloat();
    if ( options.contains("saturation") )
        saturation = options["saturation"].toFloat();

    initializeTonemapper(cv::createTonemapMantiuk(
        gamma, scale, saturation
    ));

}


