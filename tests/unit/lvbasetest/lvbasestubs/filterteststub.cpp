/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#include "filterteststub.h"
#include "live/filterworker.h"

#include <QDebug>

#include <functional>

FilterTestStub::FilterTestStub(QObject *parent)
    : QObject(parent)
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

    use(locker()->read(m_input1, m_input2)->write(m_output), cb, [this](){
//        emit this->outputChanged();
    });
}

//SharedDataLock::template<>

//    if ( workerThread() ){

//        bool canExecute = true;
//        if ( !m_input1->lockForRead(this) )
//            canExecute = false;
//        if( !m_input2->lockForRead(this) )
//            canExecute = false;
//        if ( !m_output->lockForWrite(this) )
//            canExecute = false;

//        if ( canExecute ){
//            workerThread()->postWork(cb, [this](){
//                m_input1->unlock(this);
//                m_input2->unlock(this);
//                m_output->unlock(this);

//                emit outputChanged();
//            });
//        }
//    } else {
//        cb();

//        emit outputChanged();
//    }

