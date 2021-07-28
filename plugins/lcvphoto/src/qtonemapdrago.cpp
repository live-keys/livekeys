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
    : QTonemap(parent)
    , m_saturation(1.0f)
    , m_bias(0.85f)
{
    createTonemapDrago();
    connect(this, SIGNAL(gammaChanged()), this, SLOT(createTonemapDrago()));
}

void QTonemapDrago::createTonemapDrago()
{
    initializeTonemapper(cv::createTonemapDrago(gamma(), m_saturation, m_bias));
}

float QTonemapDrago::bias() const
{
    return m_bias;
}

void QTonemapDrago::setBias(float bias)
{
    if (m_bias == bias)
        return;
    m_bias = bias;
    createTonemapDrago();
    emit biasChanged();
}

float QTonemapDrago::saturation() const
{
    return m_saturation;
}

void QTonemapDrago::setSaturation(float saturation)
{
    if (m_saturation == saturation)
        return;
    m_saturation = saturation;
    createTonemapDrago();
    emit biasChanged();
}
