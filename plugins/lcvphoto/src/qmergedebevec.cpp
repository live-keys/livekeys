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

#include "qmergedebevec.h"
#include "live/exception.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"

QMergeDebevec::QMergeDebevec(QObject *parent)
    : QObject(parent)
    , m_input(0)
    , m_response(0)
    , m_output(new QMat)
    , m_mergeDebevec(cv::createMergeDebevec())
    , m_componentComplete(false)
{
}

QMergeDebevec::~QMergeDebevec(){
    delete m_output;
}

void QMergeDebevec::filter(){
    if ( m_componentComplete &&
         m_response &&
         !m_response->data().empty() &&
         m_input &&
         m_input->size() == m_times.size() &&
         m_input->size() > 0)
    {
        try{
            std::vector<float> times;
            for ( int i = 0; i < m_times.size(); ++i )
                times.push_back(m_times[i]);

            m_mergeDebevec->process(m_input->asVector(), *m_output->cvMat(), times, *m_response->cvMat());

            emit outputChanged();

        } catch ( cv::Exception& e ){
            lv::Exception lve = CREATE_EXCEPTION(lv::Exception, e.what(), e.code);
            lv::ViewContext::instance().engine()->throwError(&lve, this);
            return;
        }
    }
}
