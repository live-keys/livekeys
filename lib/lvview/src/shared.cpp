/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
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

#include "live/shared.h"
#include "live/memory.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QDateTime>
#include <QJSValue>
#include <QJSValueIterator>

#include <QDebug>

namespace lv{

Shared::Shared(QObject *parent)
    : QObject(parent)
    , m_refs(0)
{
}

Shared::~Shared(){
}

QJSValue Shared::transfer(const QVariant &v, QJSEngine *engine){
    if( v.canConvert(QVariant::Map) ){
        QJSValue vm = engine->newObject();
        auto qm = v.toMap();
        for ( auto it = qm.begin(); it != qm.end(); ++it ){
            vm.setProperty(it.key(), transfer(it.value(), engine));
        }
        return vm;
    } else if ( v.canConvert(QVariant::List) ){
        auto qa = v.toList();
        QJSValue va = engine->newArray(static_cast<quint32>(qa.length()));

        quint32 index = 0;
        for ( auto it = qa.begin(); it != qa.end(); ++it ){
            va.setProperty(index++, transfer(*it, engine));
        }
        return va;
    } else if ( v.type() == QVariant::String) {
        return QJSValue(v.toString());
    } else if ( v.type() == QVariant::Bool ){
        return QJSValue(v.toBool());
    } else if ( v.type() == QVariant::Int ){
        return QJSValue(v.toInt());
    } else if ( v.type() == QVariant::ByteArray ){
        return QJSValue(v.toString());
    } else if ( v.canConvert(QVariant::Double) ){
        return QJSValue(v.toDouble());
    } else if ( v.canConvert<QObject*>() ){
        return engine->newQObject(v.value<QObject*>());
    }
    return QJSValue();
}

QVariant Shared::transfer(const QJSValue &value, QList<Shared *> &shared){
    if ( value.isQObject() ){
        QObject* ob = value.toQObject();
        Shared* shob = static_cast<Shared*>(ob);
        if ( shob ){
            shared.append(shob);
            return  QVariant::fromValue(ob);
        }
    } else if ( value.isArray() ){
       QJSValueIterator it(value);
       QVariantList l;
       while ( it.hasNext() ){
           it.next();
           if ( it.name() != "length" ){
               l.append(transfer(it.value(), shared));
           }
       }
       return l;
    } else if ( value.isDate() ){
        return QVariant(value.toDateTime());
    } else if ( value.isObject() ){
        if ( value.property("constructor").property("name").toString() == "ArrayBuffer" ){
            return QVariant(value.toString());
        }

        QJSValueIterator it(value);
        QVariantMap vm;
        while ( it.next() ){
            vm.insert(it.name(), transfer(it.value(), shared));
        }
        return vm;
    } else if ( value.isString() ){
        return value.toString();
    } else if ( value.isBool() ){
        return value.toBool();
    } else if ( value.isNumber() ){
        return value.toNumber();
    }
    return QVariant();
}

QVariant Shared::transfer(const QVariant &v, QList<Shared *> &shared){
    if ( v.canConvert<QJSValue>()){
        return transfer(v.value<QJSValue>(), shared);
    }
    else if( v.canConvert(QVariant::Map) ){
        QVariantMap vm;

        auto qm = v.toMap();
        for ( auto it = qm.begin(); it != qm.end(); ++it ){
            vm[it.key()] = transfer(it.value(), shared);
        }
    } else if ( v.canConvert(QVariant::List) ){
        QVariantList va;

        auto qa = v.toList();
        for ( auto it = qa.begin(); it != qa.end(); ++it ){
            va.append(transfer(*it, shared));
        }
    } else if ( v.canConvert(QVariant::ByteArray) ||
                v.canConvert(QVariant::Bool) ||
                v.canConvert(QVariant::Int) ||
                v.canConvert(QVariant::Double) )
    {
        return v;
    } else if ( v.canConvert<QObject*>() ){
        QObject* o = v.value<QObject*>();
        Shared* so = static_cast<Shared*>(o);
        if ( so ){
            Shared* sotransfer = so->transfer();
            if ( sotransfer ){
                shared.append(sotransfer);
                return QVariant::fromValue(sotransfer);
            }
        }

    }
    return QVariant();
}

}// namespace
