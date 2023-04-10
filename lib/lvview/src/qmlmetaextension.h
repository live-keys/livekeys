/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
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

#ifndef LVQMLMETAEXTENSION_H
#define LVQMLMETAEXTENSION_H

#include <QByteArray>
#include <QObject>

#include <cstdint>

#include "live/lvviewglobal.h"
#include "live/mlnode.h"
#include "live/visuallog.h"

namespace lv{

class ViewEngine;

class LV_VIEW_EXPORT QmlMetaExtension{

public:
    typedef std::shared_ptr<QmlMetaExtension> Ptr;
    typedef std::function<QObject*()> TypeConstructor;

    class LV_VIEW_EXPORT I{
    public:
        typedef std::uintptr_t Identifier;
        ~I();
    public:
        class Info{
        public:
            typedef std::shared_ptr<const Info> ConstPtr;
            static ConstPtr create(const QByteArray& name){ return Info::ConstPtr(new Info(name)); }
            Info(const QByteArray& name): m_name(name){}
            const QByteArray& name(){ return m_name; }
        private:
            QByteArray m_name;
        };
    };

public:
    ~QmlMetaExtension();
    template<typename T> static QmlMetaExtension::Ptr create(TypeConstructor tc);

    const QByteArray& name();
    QObject* newInstance();

    QmlMetaExtension::I* i(I::Identifier identif) const;
    template<typename T> T* i() const;

    static void serializeVariant(lv::ViewEngine* engine, const QVariant& v, lv::MLNode& node);
    static QVariant deserializeVariant(lv::ViewEngine* engine, const lv::MLNode& node);

    template<typename T, typename I, typename... Args> static void add(Args... args);

    void store(QmlMetaExtension::I::Identifier identif, QmlMetaExtension::I* interface);

private:
    static Ptr create(const QByteArray& name, TypeConstructor tc);
    QmlMetaExtension(const QByteArray& name, TypeConstructor tc);

    // disable copy
    QmlMetaExtension(const QmlMetaExtension&);
    QmlMetaExtension& operator = (const QmlMetaExtension&);

    QByteArray      m_name;
    TypeConstructor m_constructor;
    std::map<QmlMetaExtension::I::Identifier, QmlMetaExtension::I*> m_interfaces;

};

template<typename T, typename TI, typename... Args>
void QmlMetaExtension::add(Args... args)
{
    T::metaExtension()->store(TI::interfaceIdentifier(), new TI(args...));
}

template<typename T> T* QmlMetaExtension::i() const{
    QmlMetaExtension::I* interf = i(T::interfaceIdentifier());
    if ( !interf )
        return nullptr;
    return static_cast<T*>(interf);
}

inline const QByteArray &QmlMetaExtension::name(){
    return m_name;
}

inline QObject *QmlMetaExtension::newInstance(){
    if ( m_constructor )
        return m_constructor();
    return nullptr;
}

inline QmlMetaExtension::I *QmlMetaExtension::i(QmlMetaExtension::I::Identifier identif) const{
    auto it = m_interfaces.find(identif);
    return it != m_interfaces.end() ? it->second : nullptr;
}

template<typename T> QmlMetaExtension::Ptr QmlMetaExtension::create(QmlMetaExtension::TypeConstructor tc){
    return QmlMetaExtension::create(typeid (T).name(), tc);
}

class LV_VIEW_EXPORT QmlExtensionObjectI{
public:
    ~QmlExtensionObjectI();

    virtual const lv::QmlMetaExtension::Ptr& typeMetaExtension() const = 0;
};

}// namespace

#define META_EXTENSION_BASE(_class, _constructor) \
    public: virtual const lv::QmlMetaExtension::Ptr& typeMetaExtension() const{ return metaExtension(); } \
    public: static lv::QmlMetaExtension::Ptr& metaExtension(){ static lv::QmlMetaExtension::Ptr mi = lv::QmlMetaExtension::create<_class>(_constructor); return mi; }

#define META_EXTENSION(_class, _constructor) \
    public: virtual const lv::QmlMetaExtension::Ptr& typeMetaExtension() const override{ return metaExtension(); } \
    public: static lv::QmlMetaExtension::Ptr& metaExtension(){ static lv::QmlMetaExtension::Ptr mi = lv::QmlMetaExtension::create<_class>(_constructor); return mi; }

#define META_EXTENSION_INTERFACE(_class) \
    public: static const lv::QmlMetaExtension::I::Info::ConstPtr& metaExtensionInterface(){ static lv::QmlMetaExtension::I::Info::ConstPtr minterf = lv::QmlMetaExtension::I::Info::create(typeid(_class).name()); return minterf; } \
    public: static lv::QmlMetaExtension::I::Identifier interfaceIdentifier(){ return reinterpret_cast<lv::QmlMetaExtension::I::Identifier>(metaExtensionInterface().get()); }


namespace lv{

class LV_VIEW_EXPORT MetaSerializableI : public QmlMetaExtension::I{

    META_EXTENSION_INTERFACE(MetaSerializableI);

public:
    typedef std::function<void(ViewEngine*, const QObject*, MLNode&)> Serialize;
    typedef std::function<QObject*(ViewEngine*, const MLNode&)>       Deserialize;

    MetaSerializableI(Serialize serialize, Deserialize deserialize) : m_serialize(serialize), m_deserialize(deserialize){}

    void serialize(ViewEngine* engine, const QObject* object, MLNode& node);
    QObject* deserialize(ViewEngine* engine, const MLNode& node);

    template<typename T> static MetaSerializableI* from(){return MetaSerializableI::from(T::metaExtension()); }
    static MetaSerializableI* from(const QmlMetaExtension::Ptr& me){ return me->i<MetaSerializableI>(); }

private:
    Serialize   m_serialize;
    Deserialize m_deserialize;
};

inline void MetaSerializableI::serialize(ViewEngine *engine, const QObject *object, MLNode &node){
    m_serialize(engine, object, node);
}

inline QObject* MetaSerializableI::deserialize(ViewEngine* engine, const MLNode &node){
    return m_deserialize(engine, node);
}



class MetaLogI : public QmlMetaExtension::I{

    META_EXTENSION_INTERFACE(MetaLogI);

public:
    typedef std::function<void(ViewEngine*, QObject*, VisualLog&)> Log;

    MetaLogI(Log log) : m_log(log){}

    void log(ViewEngine* ve, QObject* o, VisualLog& vl);

    template<typename T> static MetaLogI* from(){return MetaLogI::from(T::metaExtension()); }
    static MetaLogI* from(const QmlMetaExtension::Ptr& me){ return me->i<MetaLogI>(); }

private:
    Log m_log;
};

inline void MetaLogI::log(ViewEngine *ve, QObject *o, VisualLog &vl){
    m_log(ve, o, vl);
}



}// namespace

#endif // LVQMLMETAEXTENSION_H
