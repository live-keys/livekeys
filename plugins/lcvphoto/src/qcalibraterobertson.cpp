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

QCalibrateRobertson::QCalibrateRobertson(QObject *parent)
    : QObject(parent)
    , m_input(nullptr)
    , m_output(new QMat)
    , m_calibrateRobertson(cv::createCalibrateRobertson())
    , m_componentComplete(false)
{
}

QCalibrateRobertson::~QCalibrateRobertson(){
}

void QCalibrateRobertson::setParams(const QVariantMap &params){
    if (m_params == params)
        return;

    m_params = params;
    emit paramsChanged();

    int maxIter     = 30;
    float threshold = 0.01f;

    if ( params.contains("maxIter") )
        maxIter = params["maxIter"].toInt();
    if ( params.contains("threshold") )
        threshold = params["threshold"].toFloat();

    m_calibrateRobertson = cv::createCalibrateRobertson(maxIter, threshold);

    filter();
}

void QCalibrateRobertson::filter(){
    if ( m_componentComplete &&
         m_input &&
         m_input->itemCount() == m_times.size() &&
         m_input->itemCount() > 0 &&
         m_calibrateRobertson)
    {
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

            std::vector<float> times;
            for ( int i = 0; i < m_times.size(); ++i )
                times.push_back(m_times[i]);

            m_calibrateRobertson->process(asVector(m_input), *m_output->internalPtr(), times);
        } catch ( cv::Exception& e ){
            lv::Exception lve = CREATE_EXCEPTION(lv::Exception, e.what(), e.code);
            lv::ViewContext::instance().engine()->throwError(&lve, this);
            return;
        }

    }
}
