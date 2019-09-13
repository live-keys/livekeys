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

#include "qbrightnessandcontrast.h"

QBrightnessAndContrast::QBrightnessAndContrast(QQuickItem *parent)
    : QMatFilter(parent)
{
}

QBrightnessAndContrast::~QBrightnessAndContrast(){
}

void QBrightnessAndContrast::transform(const cv::Mat &in, cv::Mat &out){
    out.create(in.size(), in.type());
    for( int y = 0; y < in.rows; y++ ) {
        const uchar* pi = in.ptr<uchar>(y);
        uchar* po       = out.ptr<uchar>(y);

        for( int x = 0; x < in.cols; x++ ) {
            for( int c = 0; c < in.channels(); c++ ){
                int pos = x * in.channels() + c;
                po[pos] = cv::saturate_cast<uchar>(m_contrast * pi[pos] + m_brightness);
            }
        }
    }
}
