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

#include "qstitcher.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/viewcontext.h"

QStitcher::QStitcher(QQuickItem *parent)
    : QMatDisplay(parent)
#if CV_VERSION_MAJOR >= 3 && CV_VERSION_MINOR > 2
    , m_stitcher(cv::Stitcher::create())
#else
    , m_stitcher(cv::Stitcher::createDefault())
#endif
{

}

void QStitcher::filter(){
    if ( m_input && m_input->size() > 1 ){
        try{
            #if CV_VERSION_MAJOR >= 3 && CV_VERSION_MINOR > 2
                cv::Stitcher::Status status = m_stitcher->stitch(m_input->asVector(), *output()->cvMat());
            #else
                cv::Stitcher::Status status = m_stitcher.stitch(m_input->asVector(), *output()->cvMat());
            #endif

            if ( status == cv::Stitcher::OK ){
                setImplicitWidth(output()->data().cols);
                setImplicitHeight(output()->data().rows);
                emit outputChanged();
                update();
            } else {
                emit error(status);
            }
        } catch ( cv::Exception& e ){
            lv::Exception lve = CREATE_EXCEPTION(lv::Exception, e.what(), e.code);
            lv::ViewContext::instance().engine()->throwError(&lve, this);
            return;
        }
    }
}

void QStitcher::setParams(const QVariantMap &params){
    if (m_params == params)
        return;

    m_params = params;
    emit paramsChanged(m_params);

#if CV_VERSION_MAJOR >= 3 && CV_VERSION_MINOR > 2
    cv::Stitcher::Mode mode = cv::Stitcher::PANORAMA;
    if ( params.contains("mode") )
        mode = static_cast<cv::Stitcher::Mode>(params["mode"].toInt());
#endif

    bool tryUseGpu = false;
    if ( params.contains("tryUseGpu") )
        tryUseGpu = params["tryUseGpu"].toBool();

#if CV_VERSION_MAJOR >= 3 && CV_VERSION_MINOR > 2
    m_stitcher = cv::Stitcher::create(mode, tryUseGpu);
#else
    m_stitcher = cv::Stitcher::createDefault(tryUseGpu);
#endif

    filter();
}
