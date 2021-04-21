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

QCalibrateDebevec::QCalibrateDebevec(QObject *parent)
    : QObject(parent)
    , m_input(new lv::QmlObjectList)
    , m_output(new QMat)
    , m_calibrateDebevec(cv::createCalibrateDebevec())
    , m_componentComplete(false)
{
}

QCalibrateDebevec::~QCalibrateDebevec(){
    delete m_output;
}

void QCalibrateDebevec::setParams(const QVariantMap &params){
    if (m_params == params)
        return;

    m_params = params;
    emit paramsChanged();

    int samples  = 70;
    float lambda = 10.0f;
    bool random  = false;

    if ( params.contains("samples") )
        samples = params["samples"].toInt();
    if ( params.contains("lambda") )
        lambda = params["lambda"].toFloat();
    if ( params.contains("random") )
        random = params["random"].toBool();

    m_calibrateDebevec = cv::createCalibrateDebevec(samples, lambda, random);

    filter();
}

void QCalibrateDebevec::filter(){
    if ( m_componentComplete &&
         m_input &&
         m_input->itemCount() == m_times.size() &&
         m_input->itemCount() > 0 &&
         m_calibrateDebevec)
    {
        try{
            std::vector<float> times;
            for ( int i = 0; i < m_times.size(); ++i )
                times.push_back(m_times[i]);

            auto asVector = [](lv::QmlObjectList* list) -> std::vector<cv::Mat> {
                std::vector<cv::Mat> result;
                for (int i = 0; i < list->itemCount(); ++i){
                    QMat* m = qobject_cast<QMat*>(list->itemAt(i));
                    if (!m) return std::vector<cv::Mat>();
                    result.push_back(m->internal());
                }
                return result;
            };

            m_calibrateDebevec->process(asVector(m_input), *m_output->internalPtr(), times);

            emit outputChanged();

        } catch ( cv::Exception& e ){
            lv::Exception lve = CREATE_EXCEPTION(lv::Exception, e.what(), e.code);
            lv::ViewContext::instance().engine()->throwError(&lve, this);
            return;
        }

    }
}
