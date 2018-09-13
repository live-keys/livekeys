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

#ifndef LVFILTER_H
#define LVFILTER_H

#include "live/lvbaseglobal.h"
#include "live/shareddata.h"
#include <functional>

namespace lv{

class FilterWorker;

class LV_BASE_EXPORT Filter{

    typedef SharedData* SharedDataPtr;
    typedef QObject*    QObjectPtr;

public:
    /**
     * @brief The SharedDataLocker class
     * Blocks resources according to the thread that's using them.
     *
     * For example, given 2 filters:
     *   Fitler1{ reads ... / writes to: B, C} -- UI Thread
     *   Filter2{ reads B, C / writes ... }    -- WK Thread
     *
     *   UI Thread reaches Filter2, Filter2 locks B, C for read and starts processing
     *   UI Thread reaches Filter1 (Filter2 still processing) ==> UI Thread locks to B, C till
     *   they are released
     *
     */
    class SharedDataLocker{

    private:
        bool recurseReadSingle(const SharedDataPtr& sd){
           // create lock if worker thread
           if ( m_filter->workerThread() && !sd->hasLock())
               sd->createLock();

           // no lock => free to process
           if ( !sd->hasLock() )
               return true;

           // reserve for read
           if ( sd->reserveForRead(m_filter) ){
               m_readLocks.append(sd);
               return true;
           }

           return false;
        }

        bool recurseWriteSingle(const SharedDataPtr& sd){
           // create lock if there's a worker thread
           if ( m_filter->workerThread() && !sd->hasLock())
               sd->createLock();

           // if there's no lock we're free to process
           if ( !sd->hasLock() )
               return true;

           // reserve for write
           if ( sd->reserveForWrite(m_filter)){
               m_writeLocks.append(sd);
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
            if ( !recurseRead(args...) )
                m_allReserved = false;

            return this;
        }

        template<typename... Args> SharedDataLocker* write(Args... args){
            if ( !recurseWrite(args...) )
                m_allReserved = false;

            return this;
        }

        friend class Filter;
        friend class FilterWorker;
        friend class FilterWorkerPrivate;

    private:
        void clearReservations();

        SharedDataLocker(Filter* filter) : m_filter(filter), m_allReserved(true){}
        ~SharedDataLocker();

        Filter*            m_filter;
        bool               m_allReserved;
        QList<SharedData*> m_readLocks;
        QList<SharedData*> m_writeLocks;
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
    void deleteLocker(SharedDataLocker* locker);

};

}// namespace

#endif // LVFILTER_H
