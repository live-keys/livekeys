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

#include "filterteststub.h"
#include "live/workerthread.h"

#include <QDebug>

#include <functional>

FilterTestStub::FilterTestStub(QObject *parent)
    : lv::Act(parent)
    , m_input1(0)
    , m_input2(0)
    , m_output(new SharedDataTestStub(this))
    , m_sleepTime(0)
{
}

FilterTestStub::~FilterTestStub(){
}

void FilterTestStub::process(){
    if ( !m_input1 || !m_input2 )
        return;

    std::function<void()> cb = [this](){
        if ( m_input1->items().size() == m_input2->items().size() ){
            m_output->items().clear();
            for ( int i = 0; i < m_input1->items().size(); ++i ){
                int item1 = m_input1->items()[i];
                int item2 = m_input2->items()[i];
                m_output->items().append(item1 + item2);
            }
        }
    };

    onRun(lv::Shared::refScope(this, m_input1, m_input2), cb, [this](){
        emit this->outputChanged(m_output);
    });
}
