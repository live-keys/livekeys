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

#include "qautolevels.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"

#include <QQmlEngine>
#include <QJSValueIterator>

QAutoLevels::QAutoLevels(QObject *parent)
    : QObject(parent){

}

void QAutoLevels::setHistogram(QMat *histogram){
    m_histogram = histogram;
    emit histogramChanged();

    if (m_histogram == QMat::nullMat() )
        return;

    m_output = lv::ViewContext::instance().engine()->engine()->newObject();
    const cv::Mat& hist = *histogram->cvMat();

    if ( hist.cols < 3 )
        return;

    for ( int y = 0; y < hist.rows; ++y ){
        const ushort* p = hist.ptr<ushort>(y);

        int x = 0;
        while ( x < hist.cols - 2 ){
            if ( p[x] == 0 )
                ++x;
            else
                break;
        }
        int black = x;

        x = hist.cols - 1;
        while ( x > 0 ){
            if ( p[x] == 0 )
                --x;
            else
                break;
        }
        int white = x;

        if ( white < black + 2 )
            white = black + 2;

        QJSValue channelAutoLevels = lv::ViewContext::instance().engine()->engine()->newArray(3);
        channelAutoLevels.setProperty(0, black);
        channelAutoLevels.setProperty(1, 1.0);
        channelAutoLevels.setProperty(2, white);

        m_output.setProperty(y, channelAutoLevels);
    }

    emit outputChanged();
}
