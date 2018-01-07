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

#ifndef LVFILTER_H
#define LVFILTER_H

#include "live/lvbaseglobal.h"
#include "live/shareddata.h"
#include <functional>

namespace lv{

class FilterWorker;

class LV_BASE_EXPORT Filter{

    typedef SharedData* SharedDataPtr;

public:
    class SharedDataLocker{

    private:
        template<typename T> bool recurseReadSingle(const T&){
            return true;
        }

       bool recurseReadSingle(const SharedDataPtr& sd){
            if ( sd->lockForRead(m_filter)){
                m_locks.append(sd);
                return true;
            }
            return false;
        }

        template<typename T, typename... Args> bool recurseRead(const T& first){
            return recurseReadSingle(first);
        }

        template<typename T, typename... Args> bool recurseRead(const T& first, Args... args){
            if ( !recurseReadSingle(first) )
                return false;

            return recurseRead(args...);
        }

        template<typename T> bool recurseWriteSingle(const T&){
            return true;
        }

        bool recurseWriteSingle(const SharedDataPtr& sd){
            if ( sd->lockForWrite(m_filter)){
                m_locks.append(sd);
                return true;
            }
            return false;
        }

        template<typename T, typename... Args> bool recurseWrite(const T& first){
            return recurseWriteSingle(first);
        }

        template<typename T, typename... Args> bool recurseWrite(const T& first, Args... args){
            if ( !recurseWriteSingle(first) )
                return false;

            return recurseWrite(args...);
        }

    public:
        template<typename... Args> SharedDataLocker* read(Args... args){
            if ( !m_filter->workerThread() )
                return this;

            if ( !recurseRead(args...) )
                m_allLocked = false;

            return this;
        }

        template<typename... Args> SharedDataLocker* write(Args... args){
            if ( !m_filter->workerThread() )
                return this;

            if ( !recurseWrite(args...) )
                m_allLocked = false;

            return this;
        }

        friend class Filter;
        friend class FilterWorker;

    private:
        SharedDataLocker(Filter* filter) : m_filter(filter), m_allLocked(true){}
        ~SharedDataLocker(){
            for ( auto it = m_locks.begin(); it != m_locks.end(); ++it ){
                (*it)->unlock(m_filter);
            }
        }

        Filter*            m_filter;
        bool               m_allLocked;
        QList<SharedData*> m_locks;
    };

private:
    FilterWorker* m_workerThread = 0;

public:
    Filter();
    virtual ~Filter();

    void setWorkerThread(FilterWorker* worker){
        m_workerThread = worker;
    }

    FilterWorker* workerThread(){
        return m_workerThread;
    }

    virtual void process(){}

    void use(
        SharedDataLocker* locker,
        const std::function<void()>& cb,
        const std::function<void()>& rs);

    SharedDataLocker* createLocker(){
        return new SharedDataLocker(this);
    }

};

}// namespace

#endif // LVFILTER_H
