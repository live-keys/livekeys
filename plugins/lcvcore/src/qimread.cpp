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

#include "qimread.h"
#include "qmatstate.h"
#include "qmatnode.h"
#include "opencv2/highgui.hpp"

#include "live/visuallog.h"
#include "live/stacktrace.h"

#include <QSGSimpleMaterial>


QImRead::QImRead(QQuickItem *parent)
    : QMatDisplay(parent)
    , m_iscolor(CV_LOAD_IMAGE_COLOR)
{
}

QImRead::~QImRead(){
}


void QImRead::componentComplete(){
    QQuickItem::componentComplete();
    loadImage();
}

void QImRead::loadImage(){
    if ( m_file != "" && isComponentComplete() ){
        cv::Mat temp = cv::imread(m_file.toStdString(), m_iscolor);
        if ( !temp.empty() ){
            temp.copyTo(*output()->cvMat());
            setImplicitWidth(output()->cvMat()->size().width);
            setImplicitHeight(output()->cvMat()->size().height);
            emit outputChanged();
            update();
        }
    }
}
