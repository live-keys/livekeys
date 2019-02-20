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
#include <QQmlEngine>

namespace lv{

class Act;
class Act;

class LV_VIEW_EXPORT Shared : public QObject{

public:
    /**
     * \brief Blocks shared data for reading until destroyed.
     */
    class RefScope{

    public:
        friend class Shared;
        typedef Shared* SharedPtr;

    public:
        void readSingle(const SharedPtr& sd){
            Shared::ref(sd);
        }

        template<typename T, typename... Args> void read(const T& first){
            return readSingle(first);
        }

        template<typename T, typename... Args> void read(const T& first, Args... args){
            readSingle(first);
            read(args...);
        }

    private:
        RefScope(Act* act) : m_act(act){}
        void clear(){ foreach( Shared* sd, m_refs ) Shared::unref(sd); }

    public:
        ~RefScope(){ clear(); }

    private:
        Act*           m_act;
        QList<Shared*> m_refs;
    };

public:
    Shared(QObject* parent = nullptr);
    virtual ~Shared();

    template<typename ...Args> static RefScope* refScope(Act* call, Args... args){
        RefScope* rs = new RefScope(call);
        rs->read(args...);

        return rs;
    }

    static void ref(Shared* data);
    static void unref(Shared* data);
    static void ownCpp(Shared* data);
    static void ownJs(Shared* data);

    virtual void recycleSize(int) const{}

private:
    int m_refs;
};

inline void Shared::ownCpp(Shared *data){
    QQmlEngine::setObjectOwnership(data, QQmlEngine::CppOwnership);
}

inline void Shared::ownJs(Shared *data){
    QQmlEngine::setObjectOwnership(data, QQmlEngine::JavaScriptOwnership);
}

inline void Shared::ref(Shared *data){
    if ( data->m_refs == 0){
        ownCpp(data);
        data->m_refs++;
    }
}

inline void Shared::unref(Shared *data){
    if ( --data->m_refs == 0 ){
        ownJs(data);
    }
}

}// namespace

#endif // LVSHARED_H
