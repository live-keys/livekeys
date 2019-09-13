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

#include "qtonemapreinard.h"

QTonemapReinard::QTonemapReinard(QQuickItem *parent)
    : QTonemap(cv::createTonemapReinhard(), parent)
{
}

void QTonemapReinard::initialize(const QVariantMap &options){
    float gamma = 1.0f;
    float intensity = 0.0f;
    float lightAdapt = 1.0f;
    float colorAdapt = 0.0f;

    if ( options.contains("gamma") )
        gamma = options["gamma"].toFloat();
    if ( options.contains("intensity") )
        intensity = options["intensity"].toFloat();
    if ( options.contains("lightAdapt") )
        lightAdapt = options["lightAdapt"].toFloat();
    if ( options.contains("colorAdapt") )
        colorAdapt = options["colorAdapt"].toFloat();

    initializeTonemapper(cv::createTonemapReinhard(
        gamma, intensity, lightAdapt, colorAdapt
    ));

}
