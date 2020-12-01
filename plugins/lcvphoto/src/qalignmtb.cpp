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

#include "qalignmtb.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/viewcontext.h"
#include "qmat.h"
#include "qmatext.h"

QAlignMTB::QAlignMTB(QObject *parent)
    : QObject(parent)
    , m_input(nullptr)
    , m_output(lv::createMatList())
    , m_alignMTB(cv::createAlignMTB())
    , m_componentComplete(false)
{
}

QAlignMTB::~QAlignMTB(){
}

void QAlignMTB::componentComplete(){
    m_componentComplete = true;
    filter();
}

void QAlignMTB::setParams(const QVariantMap &params){
    if (m_params == params)
        return;

    m_params = params;
    emit paramsChanged();

    int maxBits = 6;
    int excludeRange = 4;
    bool cut = true;

    if ( params.contains("maxBits") )
        maxBits = params["maxBits"].toInt();
    if ( params.contains("excludeRange") )
        excludeRange = params["excludeRange"].toInt();
    if ( params.contains("cut") )
        cut = params["cut"].toInt();

    m_alignMTB = cv::createAlignMTB(maxBits, excludeRange, cut);

    filter();
}

void QAlignMTB::filter(){
    if ( m_componentComplete && m_input && m_input->itemCount() > 0 && m_output && m_alignMTB ){
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

            std::vector<cv::Mat> outputVector = asVector(m_output);
            m_alignMTB->process(asVector(m_input), outputVector);

            // clear previous contents
            for (int i = 0; i < m_output->itemCount(); ++i)
            {
                delete qobject_cast<QMat*>(m_output->itemAt(i));
            }
            m_output->clearItems();

            // copy contents

            for (size_t i = 0; i < outputVector.size(); ++i)
            {
                cv::Mat* copy = new cv::Mat(outputVector[i]);
                m_output->appendItem(new QMat(copy));
            }

            emit outputChanged();
        } catch ( cv::Exception& e ){
            lv::Exception lve = CREATE_EXCEPTION(lv::Exception, e.what(), e.code);
            lv::ViewContext::instance().engine()->throwError(&lve, this);
            return;
        }
    }
}

bool QAlignMTB::isComponentComplete(){
    return m_componentComplete;
}
