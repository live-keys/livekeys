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
#include "cvextras.h"

QAlignMTB::QAlignMTB(QObject *parent)
    : QObject(parent)
    , m_input(nullptr)
    , m_maxBits(6)
    , m_excludeRange(4)
    , m_cut(true)
{
    createAlignMTB();
}

QAlignMTB::~QAlignMTB(){
}

int QAlignMTB::maxBits() const
{
    return m_maxBits;
}

void QAlignMTB::setMaxBits(int maxBits)
{
    if (m_maxBits == maxBits)
        return;
    m_maxBits = maxBits;
    createAlignMTB();

    emit maxBitsChanged();
}

int QAlignMTB::excludeRange() const
{
    return m_excludeRange;
}

void QAlignMTB::setExcludeRange(int excludeRange)
{
    if (m_excludeRange == excludeRange)
        return;
    m_excludeRange = excludeRange;
    createAlignMTB();

    emit excludeRangeChanged();
}

bool QAlignMTB::cut() const
{
    return m_cut;
}

void QAlignMTB::setCut(bool cut)
{
    if (m_cut == cut)
        return;
    m_cut = cut;
    createAlignMTB();

    emit cutChanged();
}

lv::QmlObjectList *QAlignMTB::process(lv::QmlObjectList *input)
{
    if (!input || input->itemCount() == 0 || !m_alignMTB )
        return nullptr;

    try {
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
        std::vector<cv::Mat> resultVector;

        m_alignMTB->process(inputVector, resultVector);
        lv::QmlObjectList* result = lv::createMatList();

        for (size_t i = 0; i < resultVector.size(); ++i)
        {
            cv::Mat* copy = new cv::Mat(resultVector[i]);
            result->appendItem(new QMat(copy));
        }

        return result;
    } catch ( cv::Exception& e ){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "AlignMTB: ").jsThrow();
        return nullptr;
    }

}

void QAlignMTB::createAlignMTB()
{
    m_alignMTB = cv::createAlignMTB(m_maxBits, m_excludeRange, m_cut);
}

