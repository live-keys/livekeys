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

#ifndef LVSHARED_H
#define LVSHARED_H

#include "live/lvviewglobal.h"

#include <QObject>
#include <QSet>

namespace lv{

class Act;
class Act;

class LV_VIEW_EXPORT Shared : public QObject{

private:
    class Readers{
    public:
        QSet<Act*> reserved;
        Act*       observer;
    };

public:
    /**
     * \brief Blocks shared data for reading until destroyed.
     */
    class ReadScope{

    public:
        friend class Shared;
        typedef Shared* SharedPtr;

    public:
        bool readSingle(const SharedPtr& sd){
            return Shared::read(m_filter, sd);
        }

        template<typename T, typename... Args> bool read(const T& first){
            return readSingle(first);
        }

        template<typename T, typename... Args> bool read(const T& first, Args... args){
            if ( !readSingle(first) )
                return false;

            return read(args...);
        }

    private:
        ReadScope(Act* filter) : m_filter(filter), m_reserved(true){}
        void clear(){ foreach( Shared* sd, m_read ) Shared::release(m_filter, sd); }

    public:
        ~ReadScope(){ clear(); }

        bool reserved() const{ return m_reserved; }

    private:
        Act*        m_filter;
        bool           m_reserved;
        QList<Shared*> m_read;

    };

public:
    Shared(QObject* parent = nullptr);
    virtual ~Shared();

    static bool read(Act* call, Shared* data);
    static void release(Act* call, Shared* data);

    template<typename ...Args> static ReadScope* readScope(Act* call, Args... args){
        ReadScope* rs = new ReadScope(call);
        if ( !rs->read(args...) )
            rs->m_reserved = false;

        return rs;
    }

    static bool isValid(const Shared* data);
    static void invalidate(Shared* data);

    virtual Shared* reloc();

private:
    Readers* readers();

    bool     m_isValid;
    Readers* m_readers;
};

}// namespace

#endif // LVSHARED_H
