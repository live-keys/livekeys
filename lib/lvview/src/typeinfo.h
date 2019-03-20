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

#ifndef TYPEINFO_H
#define TYPEINFO_H

#include <QByteArray>
#include <QSharedPointer>

#include "live/lvviewglobal.h"
#include "live/mlnode.h"
#include "live/visuallog.h"

namespace lv{

class ViewEngine;

/// \private
class LV_VIEW_EXPORT TypeInfo{

public:
    typedef QSharedPointer<TypeInfo> Ptr;

public:
    ~TypeInfo();

    static Ptr create(const QByteArray& name);

    bool isLoggable() const;
    void log(lv::VisualLog& vl, const QObject* object);
    template<typename T> void addLogging();

    void addConstructor(std::function<QObject*()> ctor);
    QObject* newInstance();

    const QByteArray& name() const;

    bool isSerializable() const;
    void serialize(ViewEngine* engine, const QObject* object, MLNode& node);
    QObject* deserialize(ViewEngine* engine, const MLNode& node);
    template<typename T> void addSerialization(
        std::function<void(const T&, MLNode&)> serialize,
        std::function<void(const MLNode&, T&)> deserialize
    );
    void addSerialization(
        std::function<void(ViewEngine*, const QObject*, MLNode&)> serialize,
        std::function<QObject*(ViewEngine*, const MLNode&)> deserialize
    );

    static void serializeVariant(lv::ViewEngine* engine, const QVariant& v, lv::MLNode& node);
    static QVariant deserializeVariant(lv::ViewEngine* engine, const lv::MLNode& node);

private:
    TypeInfo(const QByteArray& name);

    // disable copy
    TypeInfo(const TypeInfo&);
    TypeInfo& operator = (const TypeInfo&);

    QByteArray m_name;
    std::function<QObject*()> m_constructor;
    std::function<void(ViewEngine*, const QObject*, lv::MLNode&)> m_serialize;
    std::function<QObject*(ViewEngine*, const lv::MLNode&)>  m_deserialize;
    std::function<void(lv::VisualLog& vl, const QObject*)> m_log;
};

template<typename T>
void TypeInfo::addLogging(){
    m_log = [](lv::VisualLog& vl, const QObject* obj){
        const T* objtype = qobject_cast<const T*>(obj);
        vl.object(*objtype);
    };
}

template<typename T>
void TypeInfo::addSerialization(
    std::function<void(const T &, MLNode &)> serialize,
    std::function<void(const MLNode&, T&)> deserialize)
{
    if ( serialize && deserialize ){
        m_serialize = [serialize](ViewEngine*, const QObject* obj, lv::MLNode& node){
            const T* objtype = qobject_cast<const T*>(obj);
            serialize(*objtype, node);
        };
        m_deserialize = [this, deserialize](ViewEngine*, const lv::MLNode& node){
            T* objtype = qobject_cast<T*>(newInstance());
            deserialize(node, *objtype);
            return objtype;
        };
    }
}

inline void TypeInfo::addSerialization(
    std::function<void (ViewEngine *, const QObject *, MLNode &)> serialize,
    std::function<QObject *(ViewEngine *, const MLNode &)> deserialize)
{
    if ( serialize && deserialize ){
        m_serialize = serialize;
        m_deserialize = deserialize;
    }
}

inline bool TypeInfo::isSerializable() const{
    return m_serialize ? true : false;
}

inline bool TypeInfo::isLoggable() const{
    return m_log ? true : false;
}

inline void TypeInfo::log(VisualLog &vl, const QObject *object){
    m_log(vl, object);
}

inline void TypeInfo::addConstructor(std::function<QObject *()> ctor){
    m_constructor = ctor;
}

inline QObject *TypeInfo::newInstance(){
    if ( m_constructor )
        return m_constructor();
    return 0;
}

inline const QByteArray &TypeInfo::name() const{
    return m_name;
}

inline void TypeInfo::serialize(ViewEngine *engine, const QObject *object, MLNode &node){
    m_serialize(engine, object, node);
}

inline QObject* TypeInfo::deserialize(ViewEngine* engine, const MLNode &node){
    return m_deserialize(engine, node);
}

}// namespace

#endif // TYPEINFO_H
