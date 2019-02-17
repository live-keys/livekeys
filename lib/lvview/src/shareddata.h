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

#include "live/lvviewglobal.h"

#include <QSet>

class QObject;

namespace lv{

class Filter;

/// @private
class LV_VIEW_EXPORT SharedData{

private:
    // Reservee class
    // --------------

    class Reserver{
    public:
        Reserver() : reservedWriter(nullptr){}

        Filter*       reservedWriter;
        QSet<Filter*> reserverdReaders;

        QSet<Filter*> observers;
    };

public:
    SharedData();
    virtual ~SharedData();

    void unlockReservation(Filter* filter);
    bool reserveForRead(Filter* filter);
    bool reserveForWrite(Filter* filter);

    bool hasReserver();
    void createReserver();

private:
    void releaseObservers();

    Reserver* m_reserver;
};

inline bool SharedData::reserveForRead(Filter *filter){
    if ( m_reserver->reservedWriter ){
        return false;
    }
    m_reserver->reserverdReaders.insert(filter);
    return true;
}

inline bool SharedData::reserveForWrite(Filter *filter){
    if ( m_reserver->reserverdReaders.size() > 0 ){
        m_reserver->observers.insert(filter);
        return false;
    }
    m_reserver->reservedWriter = filter;
    return true;
}

}// namespace

#endif // LVSHAREDDATA_H
