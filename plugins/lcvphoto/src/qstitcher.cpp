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

#include "qstitcher.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/viewcontext.h"
#include "cvextras.h"

QStitcher::QStitcher(QObject *parent)
    : QObject(parent)
    , m_mode(Mode::Panorama)
    , m_tryUseGpu(true)
{
    createSticher();
}

QMat* QStitcher::stitch(lv::QmlObjectList* input){
    if (!input || input->itemCount() <= 1)
        return nullptr;
    try{
        auto asVector = [](lv::QmlObjectList* list) -> std::vector<cv::Mat> {
            std::vector<cv::Mat> result;
            for (int i = 0; i < list->itemCount(); ++i){
                QMat* m = qobject_cast<QMat*>(list->itemAt(i));
                if (!m) return std::vector<cv::Mat>();
                result.push_back(m->internal());
            }
            return result;
        };

        auto vectorInput = asVector(input);
        QMat* result = new QMat();

        #if (CV_VERSION_MAJOR >= 3 && CV_VERSION_MINOR > 2) || CV_VERSION_MAJOR >= 4
            cv::Stitcher::Status status = m_stitcher->stitch(vectorInput, *result->internalPtr());
        #else
            cv::Stitcher::Status status = m_stitcher.stitch(vectorInput, *result->internalPtr());
        #endif


        if ( status != cv::Stitcher::OK ){
            emit error(status);
            lv::CvExtras::toLocalError(cv::Exception(), lv::ViewContext::instance().engine(), this, "Stitcher: " + std::to_string(status)).jsThrow();
            return nullptr;
        }

        return result;
    } catch ( cv::Exception& e ){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "Stitcher: ").jsThrow();
        return nullptr;
    }
}

void QStitcher::createSticher()
{
    #if CV_VERSION_MAJOR >= 4
        m_stitcher = cv::Stitcher::create(static_cast<cv::Stitcher::Mode>(m_mode));
    #elif CV_VERSION_MAJOR >= 3 && CV_VERSION_MINOR > 2
        m_stitcher = cv::Stitcher::create(static_cast<cv::Stitcher::Mode>(m_mode), m_tryUseGpu);
    #else
        m_stitcher = cv::Stitcher::createDefault(m_tryUseGpu);
    #endif
}

bool QStitcher::tryUseGpu() const
{
    return m_tryUseGpu;
}

void QStitcher::setTryUseGpu(bool tryUseGpu)
{
    if (m_tryUseGpu == tryUseGpu)
        return;
    m_tryUseGpu = tryUseGpu;
    createSticher();
    emit tryUseGpuChanged();
}

int QStitcher::mode() const
{
    return m_mode;
}

void QStitcher::setMode(int mode)
{
#if (CV_VERSION_MAJOR >= 3 && CV_VERSION_MINOR > 2) || CV_VERSION_MAJOR >= 4
    if (m_mode == mode)
        return;
    m_mode = mode;
    createSticher();
    emit modeChanged();
#endif
}


