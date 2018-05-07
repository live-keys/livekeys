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

#ifndef LVSHAREDDATA_H
#define LVSHAREDDATA_H

#include "live/lvbaseglobal.h"

#include <QSet>
#include <QDebug>

class QObject;
class QReadWriteLock;

namespace lv{

class Filter;

//TODO: Optimize size

class LV_BASE_EXPORT SharedData{

public:
    SharedData();
    virtual ~SharedData();

    void unlockReservation(Filter* filter);
    bool reserveForRead(Filter* filter);
    bool reserveForWrite(Filter* filter);

    bool hasLock();
    void createLock();

private:
    void releaseObservers();

    QReadWriteLock* m_lock;

    Filter*       m_reservedWriter;
    QSet<Filter*> m_reserverdReaders;
    QSet<Filter*> m_observers;
};

inline bool SharedData::reserveForRead(Filter *filter){
    if ( m_reservedWriter ){
        return false;
    }
    m_reserverdReaders.insert(filter);
    return true;
}

inline bool SharedData::reserveForWrite(Filter *filter){
    if ( m_reserverdReaders.size() > 0 ){
        m_observers.insert(filter);
        return false;
    }
    m_reservedWriter = filter;
    return true;
}

}// namespace

#endif // LVSHAREDDATA_H
