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

#include "qmergerobertson.h"

QMergeRobertson::QMergeRobertson(QObject *parent)
    : QObject(parent)
    , m_input(nullptr)
    , m_response(nullptr)
    , m_output(new QMat)
    , m_mergeRobertson(cv::createMergeRobertson())
    , m_componentComplete(false)
{
}

QMergeRobertson::~QMergeRobertson(){
}

void QMergeRobertson::filter(){
    if ( m_componentComplete &&
         m_response &&
         m_input &&
         m_input->itemCount() == m_times.size() &&
         m_input->itemCount() > 0)
    {
        auto asVector = [](lv::QmlObjectList* list) -> std::vector<cv::Mat> {
            std::vector<cv::Mat> result;
            for (int i = 0; i < list->itemCount(); ++i){
                QMat* m = qobject_cast<QMat*>(list->itemAt(i));
                if (!m) return std::vector<cv::Mat>();
                result.push_back(m->data());
            }
            return result;
        };

        std::vector<float> times;
        for ( int i = 0; i < m_times.size(); ++i )
            times.push_back(m_times[i]);

        m_mergeRobertson->process(asVector(m_input), *m_output->cvMat(), times, *m_response->cvMat());

        emit outputChanged();
    }
}
