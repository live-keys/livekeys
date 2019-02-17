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
    : m_reserver(nullptr)
{
}

SharedData::~SharedData(){
    delete m_reserver;
}

void SharedData::unlockReservation(Filter *filter){
    if ( m_reserver->reservedWriter ){
        m_reserver->reservedWriter = 0;
    } else {
        m_reserver->reserverdReaders.remove(filter);
        if (m_reserver->reserverdReaders.isEmpty() ){
            releaseObservers();
        }
    }
}

bool SharedData::hasReserver(){
    return (m_reserver ? true : false);
}

void SharedData::createReserver(){
    m_reserver = new SharedData::Reserver;
}

void SharedData::releaseObservers(){
    QSet<Filter*> observers = m_reserver->observers;
    m_reserver->observers.clear();
    for ( auto it = observers.begin(); it != observers.end(); ++it ){
        (*it)->process();
    }
}

}// namespace
