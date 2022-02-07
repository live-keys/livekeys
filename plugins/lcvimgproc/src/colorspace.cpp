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

#include "colorspace.h"
#include "cvextras.h"
#include "live/viewcontext.h"
#include "live/visuallogqt.h"

ColorSpace::ColorSpace(QObject *parent)
    : QObject(parent)
{

}

QMat *ColorSpace::cvtColor(QMat *input, int code, int dstCn)
{
    if (!input || input->internal().empty()) return nullptr;
    try{
        cv::Mat* rMat = new cv::Mat(
            input->dimensions().height(),
            input->dimensions().width(),
            CV_MAKETYPE(input->depth(), input->channels()));
        cv::cvtColor(input->internal(), *rMat, code, dstCn);
        return new QMat(rMat);

    } catch (cv::Exception& e){
        lv::CvExtras::toLocalError(e, engine(), this, "ColorSpace: ").jsThrow();
        return nullptr;
    }
}

lv::ViewEngine *ColorSpace::engine(){
    lv::ViewEngine* ve = lv::ViewEngine::grabFromQmlEngine(qobject_cast<QQmlEngine*>(parent()));
    if ( !ve )
        lv::QmlError::warnNoEngineCaptured(this, "ColorSpace");
    return ve;
}
