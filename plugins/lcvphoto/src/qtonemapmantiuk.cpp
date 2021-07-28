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

#include "qtonemapmantiuk.h"

QTonemapMantiuk::QTonemapMantiuk(QQuickItem *parent)
    : QTonemap(parent)
    , m_scale(0.7f)
    , m_saturation(1.0f)
{
    createTonemapMantiuk();
    connect(this, SIGNAL(gammaChanged()), this, SLOT(createTonemapMantiuk()));

}

void QTonemapMantiuk::createTonemapMantiuk()
{
    initializeTonemapper(cv::createTonemapMantiuk(
        gamma(), m_scale, m_saturation
    ));
}

float QTonemapMantiuk::saturation() const
{
    return m_saturation;
}

void QTonemapMantiuk::setSaturation(float saturation)
{
    if (m_saturation == saturation)
        return;
    m_saturation = saturation;
    createTonemapMantiuk();
    emit saturationChanged();
}

float QTonemapMantiuk::scale() const
{
    return m_scale;
}

void QTonemapMantiuk::setScale(float scale)
{
    if (m_scale == scale)
        return;
    m_scale = scale;
    createTonemapMantiuk();
    emit scaleChanged();
}


