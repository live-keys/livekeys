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

#include "qtonemap.h"
#include "live/exception.h"
#include "live/viewengine.h"
#include "live/viewcontext.h"
#include "cvextras.h"

QTonemap::QTonemap(QObject *parent)
    : QObject(parent)
    , m_gamma(1.0f)
    , m_tonemapper()
{
}

QTonemap::QTonemap(cv::Ptr<cv::Tonemap> tonemapper, QObject *parent)
    : QObject(parent)
    , m_gamma(1.0f)
    , m_tonemapper(tonemapper){
}

QTonemap::~QTonemap(){
}


void QTonemap::initializeTonemapper(cv::Ptr<cv::Tonemap> tonemapper){
    m_tonemapper = tonemapper;
}

QMat* QTonemap::process(QMat* input){
    if ( !m_tonemapper || !input || input->internal().empty())
        return nullptr;

    try {
        QMat* result = new QMat();
        m_tonemapper->process(input->internal(), *result->internalPtr());
        return result;
    } catch ( cv::Exception& e ){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "Tonemap: ").jsThrow();
        return nullptr;
    }
}

float QTonemap::gamma() const
{
    return m_gamma;
}

void QTonemap::setGamma(float gamma)
{
    m_gamma = gamma;
}

