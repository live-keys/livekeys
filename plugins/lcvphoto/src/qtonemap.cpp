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

#include "qtonemap.h"

QTonemap::QTonemap(QQuickItem *parent)
    : QMatDisplay(parent)
    , m_input(QMat::nullMat())
    , m_tonemapper()
{
}

QTonemap::QTonemap(cv::Ptr<cv::Tonemap> tonemapper, QQuickItem *parent)
    : QMatDisplay(parent)
    , m_input(QMat::nullMat())
    , m_tonemapper(tonemapper){
}

QTonemap::~QTonemap(){
}

void QTonemap::initialize(const QVariantMap &){
}

void QTonemap::initializeTonemapper(cv::Ptr<cv::Tonemap> tonemapper){
    m_tonemapper = tonemapper;
    process();
}

void QTonemap::process(){
    if ( m_tonemapper && !m_input->data().empty() && isComponentComplete() ){
        m_tonemapper->process(m_input->data(), m_store);
//        m_store = m_store * 3;

        m_store.convertTo(*output()->cvMat(), CV_8UC3, 255);

        setImplicitWidth(output()->data().cols);
        setImplicitHeight(output()->data().rows);

        emit outputChanged();
        update();
    }
}

void QTonemap::componentComplete(){
    QQuickItem::componentComplete();
    process();
}
