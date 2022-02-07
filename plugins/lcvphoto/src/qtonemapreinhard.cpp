/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

#include "qtonemapreinhard.h"

QTonemapReinhard::QTonemapReinhard(QQuickItem *parent)
    : QTonemap(parent)
    , m_intensity(0.0f)
    , m_lightAdapt(1.0f)
    , m_colorAdapt(0.0f)
{
    createTonemapReinhard();
    connect(this, SIGNAL(gammaChanged()), this, SLOT(createTonemapReinhard()));
}

float QTonemapReinhard::intensity() const
{
    return m_intensity;
}

void QTonemapReinhard::setIntensity(float intensity)
{
    if (m_intensity == intensity)
        return;
    m_intensity = intensity;
    createTonemapReinhard();
    emit intensityChanged();
}

float QTonemapReinhard::lightAdapt() const
{
    return m_lightAdapt;
}

void QTonemapReinhard::setLightAdapt(float lightAdapt)
{
    if (m_lightAdapt == lightAdapt)
        return;
    m_lightAdapt = lightAdapt;
    createTonemapReinhard();
    emit lightAdaptChanged();
}

float QTonemapReinhard::colorAdapt() const
{
    return m_colorAdapt;
}

void QTonemapReinhard::setColorAdapt(float colorAdapt)
{
    if (m_colorAdapt == colorAdapt)
        return;
    m_colorAdapt = colorAdapt;
    createTonemapReinhard();
    emit colorAdaptChanged();
}

void QTonemapReinhard::createTonemapReinhard()
{
    initializeTonemapper(cv::createTonemapReinhard(
        gamma(), m_intensity, m_lightAdapt, m_colorAdapt
    ));
}
