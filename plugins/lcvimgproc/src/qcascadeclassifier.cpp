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

#include "qcascadeclassifier.h"
#include "opencv2/imgproc.hpp"
#include <vector>
#include "cvextras.h"
#include "qqmlapplicationengine.h"
#include "live/viewcontext.h"

QCascadeClassifier::QCascadeClassifier()
    : m_cc(nullptr)
    , m_file("")
{

}

void QCascadeClassifier::setFile(QString &fileString)
{
    if (fileString == m_file) return;
    m_file = fileString;
    emit fileChanged();

    if (fileString.length() == 0) return;

    m_cc = new cv::CascadeClassifier(m_file.toStdString());
    if (m_cc->empty()){
        delete m_cc;
        m_cc = nullptr;
    }
}

QVariantList QCascadeClassifier::detectFaces(QMat *input, double scaleFactor, int minNeighbors, int flags)
{
    QVariantList result;
    if (!input || input->internal().empty() || !m_cc) return result;

    try {

        cv::Mat gray;

        cv::cvtColor(input->internal(), gray, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(gray, gray);
        std::vector<cv::Rect> cvResult;
        m_cc->detectMultiScale(gray, cvResult, scaleFactor, minNeighbors, flags);
        for (auto cvr: cvResult)
            result.push_back(QRect(cvr.x, cvr.y, cvr.width, cvr.height));

        return result;
    } catch (cv::Exception& e){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "CascadeClassifier: ").jsThrow();
    }
    return result;
}


