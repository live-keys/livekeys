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

#include "qcalibratedebevec.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "cvextras.h"
#include "qmatext.h"

QCalibrateDebevec::QCalibrateDebevec(QObject *parent)
    : QObject(parent)
    , m_samples(70)
    , m_lambda(10.0f)
    , m_random(false)
{
    createCalibrateDebevec();
}

QCalibrateDebevec::~QCalibrateDebevec(){
}

int QCalibrateDebevec::samples() const
{
    return m_samples;
}

void QCalibrateDebevec::setSamples(int samples)
{
    if (m_samples == samples)
        return;
    m_samples = samples;
    emit samplesChanged();
}

float QCalibrateDebevec::lambda() const
{
    return m_lambda;
}

void QCalibrateDebevec::setLambda(float lambda)
{
    if (m_lambda == lambda)
        return;
    m_lambda = lambda;
    emit lambdaChanged();
}

bool QCalibrateDebevec::random() const
{
    return m_random;
}

void QCalibrateDebevec::setRandom(bool random)
{
    if (m_random == random)
        return;
    m_random = random;
    emit randomChanged();
}

QMat *QCalibrateDebevec::process(lv::QmlObjectList *input, QList<qreal> times)
{
    if (!input || input->itemCount() != times.size() || input->itemCount() == 0 || !m_calibrateDebevec)
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

        m_calibrateDebevec->process(inputVector, *result->internalPtr(), vectorTimes);

        return result;
    } catch ( cv::Exception& e ){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "CalibrateDebevec: ").jsThrow();
        return nullptr;
    }
}

void QCalibrateDebevec::createCalibrateDebevec()
{
    m_calibrateDebevec = cv::createCalibrateDebevec(m_samples, m_lambda, m_random);
}

