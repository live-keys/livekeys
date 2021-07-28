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

#include "qcalibraterobertson.h"
#include "live/exception.h"
#include "live/viewengine.h"
#include "live/viewcontext.h"
#include "cvextras.h"

QCalibrateRobertson::QCalibrateRobertson(QObject *parent)
    : QObject(parent)
    , m_maxIter(30)
    , m_threshold(0.01f)
{
    createCalibrateRobertson();
}

QCalibrateRobertson::~QCalibrateRobertson(){
}

int QCalibrateRobertson::maxIter() const
{
    return m_maxIter;
}

void QCalibrateRobertson::setMaxIter(int maxIter)
{
    if (m_maxIter == maxIter)
        return;
    m_maxIter = maxIter;
    createCalibrateRobertson();
    emit maxIterChanged();
}

float QCalibrateRobertson::threshold() const
{
    return m_threshold;
}

void QCalibrateRobertson::setThreshold(float threshold)
{
    if (m_threshold == threshold)
        return;
    m_threshold = threshold;
    createCalibrateRobertson();
    emit thresholdChanged();
}

QMat *QCalibrateRobertson::process(lv::QmlObjectList *input, QList<qreal> times)
{
    if (!input || input->itemCount() != times.size() || input->itemCount() == 0 || !m_calibrateRobertson)
        return nullptr;

    try {
        std::vector<float> vectorTimes;
        for ( int i = 0; i < times.size(); ++i )
            vectorTimes.push_back(times[i]);

        auto asVector = [](lv::QmlObjectList* list) -> std::vector<cv::Mat> {
            std::vector<cv::Mat> result;
            for (int i = 0; i < list->itemCount(); ++i){
                QMat* m = qobject_cast<QMat*>(list->itemAt(i));
                if (!m) return std::vector<cv::Mat>();
                result.push_back(m->internal());
            }
            return result;
        };

        auto inputVector = asVector(input);

        QMat* result = new QMat();

        m_calibrateRobertson->process(inputVector, *result->internalPtr(), vectorTimes);

        return result;
    } catch ( cv::Exception& e ){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "CalibrateRobertson: ").jsThrow();
        return nullptr;
    }
}

void QCalibrateRobertson::createCalibrateRobertson()
{
    m_calibrateRobertson = cv::createCalibrateRobertson(m_maxIter, m_threshold);
}

