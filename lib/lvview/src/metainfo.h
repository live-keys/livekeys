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

#ifndef METAINFO_H
#define METAINFO_H

#include <QByteArray>
#include <QSharedPointer>

#include "live/lvviewglobal.h"
#include "live/mlnode.h"
#include "live/visuallog.h"

namespace lv{

class ViewEngine;

/// \private
class LV_VIEW_EXPORT MetaInfo{

public:
    typedef QSharedPointer<MetaInfo> Ptr;

public:
    ~MetaInfo();

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
    MetaInfo(const QByteArray& name);

    // disable copy
    MetaInfo(const MetaInfo&);
    MetaInfo& operator = (const MetaInfo&);

    QByteArray m_name;
    std::function<QObject*()> m_constructor;
    std::function<void(ViewEngine*, const QObject*, lv::MLNode&)> m_serialize;
    std::function<QObject*(ViewEngine*, const lv::MLNode&)>  m_deserialize;
    std::function<void(lv::VisualLog& vl, const QObject*)> m_log;
};

template<typename T>
void MetaInfo::addLogging(){
    m_log = [](lv::VisualLog& vl, const QObject* obj){
        const T* objtype = qobject_cast<const T*>(obj);
        vl.object(*objtype);
    };
}

template<typename T>
void MetaInfo::addSerialization(
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

inline void MetaInfo::addSerialization(
    std::function<void (ViewEngine *, const QObject *, MLNode &)> serialize,
    std::function<QObject *(ViewEngine *, const MLNode &)> deserialize)
{
    if ( serialize && deserialize ){
        m_serialize = serialize;
        m_deserialize = deserialize;
    }
}

inline bool MetaInfo::isSerializable() const{
    return m_serialize ? true : false;
}

inline bool MetaInfo::isLoggable() const{
    return m_log ? true : false;
}

inline void MetaInfo::log(VisualLog &vl, const QObject *object){
    m_log(vl, object);
}

inline void MetaInfo::addConstructor(std::function<QObject *()> ctor){
    m_constructor = ctor;
}

inline QObject *MetaInfo::newInstance(){
    if ( m_constructor )
        return m_constructor();
    return nullptr;
}

inline const QByteArray &MetaInfo::name() const{
    return m_name;
}

inline void MetaInfo::serialize(ViewEngine *engine, const QObject *object, MLNode &node){
    m_serialize(engine, object, node);
}

inline QObject* MetaInfo::deserialize(ViewEngine* engine, const MLNode &node){
    return m_deserialize(engine, node);
}

}// namespace

#endif // METAINFO_H
