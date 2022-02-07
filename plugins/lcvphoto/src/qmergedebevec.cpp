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

#include "qmergedebevec.h"
#include "live/exception.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "cvextras.h"

QMergeDebevec::QMergeDebevec(QObject *parent)
    : QObject(parent)
    , m_mergeDebevec(cv::createMergeDebevec())
{

}

QMergeDebevec::~QMergeDebevec(){
}

QMat *QMergeDebevec::process(lv::QmlObjectList *input, QList<qreal> times, QMat *response)
{
        if ( !response ||
             response->internal().empty() ||
             !input ||
             input->itemCount() != times.size() ||
             input->itemCount() == 0)
            return nullptr;
    try {
        std::vector<float> timesVector;
        for ( int i = 0; i < times.size(); ++i )
            timesVector.push_back(times[i]);

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

        m_mergeDebevec->process(inputVector, *result->internalPtr(), timesVector, *response->internalPtr());

        return result;
    } catch ( cv::Exception& e ){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "MergeDebevec: ").jsThrow();
        return nullptr;
    }
}
