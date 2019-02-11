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

#include "live/shareddata.h"
#include "live/filter.h"
#include <QReadWriteLock>

namespace lv{

SharedData::SharedData()
    : m_lock(0)
    , m_reservedWriter(0)
{
}

SharedData::~SharedData(){
    delete m_lock;
}

void SharedData::unlockReservation(Filter *filter){
    if ( m_reservedWriter ){
        m_reservedWriter = 0;
//        releaseObservers();
    } else {
        m_reserverdReaders.remove(filter);
        if (m_reserverdReaders.isEmpty() ){
            releaseObservers();
        }
    }
}

bool SharedData::hasLock(){
    return m_lock;
}

void SharedData::createLock(){
    m_lock = new QReadWriteLock;
}

void SharedData::releaseObservers(){
    QSet<Filter*> observers = m_observers;
    m_observers.clear();
    for ( auto it = observers.begin(); it != observers.end(); ++it ){
        (*it)->process();
    }
}

}// namespace
