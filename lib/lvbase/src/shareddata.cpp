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
    , m_writer(0)
{
}

SharedData::~SharedData(){
    delete m_lock;
}

bool SharedData::lockForWrite(Filter *filter){
    if ( m_readers.size() > 0 || m_writer ){
        m_observers.insert(filter);
        return false;
    }

    m_writer = filter;
    return true;
}

void SharedData::unlock(Filter *filter){
    if ( m_writer ){
        m_writer = 0;
        releaseObservers();
    } else {
        m_readers.remove(filter);
        if (m_readers.isEmpty() ){
            releaseObservers();
        }
    }
}

bool SharedData::lockForRead(Filter *filter){
    if ( m_writer ){
        m_observers.insert(filter);
        return false;
    }
    m_readers.insert(filter);
    return true;
}

QReadWriteLock *SharedData::lock(){
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
