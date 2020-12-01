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

#include "qabsdiff.h"

QAbsDiff::QAbsDiff(QQuickItem *parent)
    : QMatFilter(parent)
    , m_in2(QMat::nullMat())
{
}

QAbsDiff::~QAbsDiff(){
}

void QAbsDiff::transform(const cv::Mat &in, cv::Mat &out){
    if ( in.size() == m_in2->internalPtr()->size() )
        cv::absdiff(in, *m_in2->internalPtr(), out);
}

