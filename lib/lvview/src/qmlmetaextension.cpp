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

#include "qmlmetaextension.h"
#include <QVariant>

#include "live/viewengine.h"
#include "live/exception.h"

namespace lv{

QmlMetaExtension::I::~I(){
}

QmlMetaExtension::QmlMetaExtension(const QByteArray &name, QmlMetaExtension::TypeConstructor tc)
    : m_name(name)
{
    m_constructor = tc;
}

QmlMetaExtension::~QmlMetaExtension(){

}

QmlMetaExtension::Ptr QmlMetaExtension::create(const QByteArray &name, QmlMetaExtension::TypeConstructor tc){
    return QmlMetaExtension::Ptr(new QmlMetaExtension(name, tc));
}

void QmlMetaExtension::store(QmlMetaExtension::I::Identifier identif, QmlMetaExtension::I *interface){
    if ( m_interfaces.find(identif) != m_interfaces.end() ){
        THROW_EXCEPTION(lv::Exception, Utf8("Interface already exists with identifier").format(identif), lv::Exception::toCode("Exists"));
    }
    m_interfaces[identif] = interface;
}


void QmlMetaExtension::serializeVariant(ViewEngine *engine, const QVariant &v, MLNode &node){
    if ( v.type() == QVariant::UInt ||
         v.type() == QVariant::ULongLong ||
         v.type() == QVariant::Int ||
         v.type() == QVariant::LongLong
    ){
        node = v.toLongLong();
    } else if ( v.type() == QVariant::String || v.type() == QVariant::ByteArray ){
        node = v.toByteArray().toStdString();
    } else if ( v.type() == QVariant::Double ){
        node = v.toDouble();
    } else if ( v.type() == QVariant::Bool ){
        node = v.toBool();
    } else if ( v.value<QObject*>() ){
        QObject* ob = v.value<QObject*>();
        //TODO
//        const QMetaObject* mo = ob->metaObject();
//        if ( mo->inherits(&lv::Group::staticMetaObject))
//            mo = &lv::Group::staticMetaObject;
//        QmlMetaInfo::Ptr ti = engine->typeInfo(mo);
//        if ( ti.isNull() || !ti->isSerializable() ){
//            node = MLNode(MLNode::Object);
//            lv::Exception e = CREATE_EXCEPTION(
//                lv::Exception, std::string("Non serializable object: ") + ob->metaObject()->className(), 0
//            );
//            engine->throwError(&e);
//            return;
//        }

//        MLNode obSerialize;
//        ti->serialize(engine, ob, obSerialize);

//        if ( obSerialize.type() == MLNode::Object ){
//            obSerialize["__type"] = ti->name().toStdString();
//        }
//        node = obSerialize;

    } else if ( v.canConvert(QMetaType::QVariantList) ){

        QVariantList vl = v.value<QVariantList>();
        node = MLNode(MLNode::Array);

        for ( auto it = vl.begin(); it != vl.end(); ++it ){
            MLNode result;
            QmlMetaExtension::serializeVariant(engine, *it, result);
            node.append(result);
        }
    }
}

QVariant QmlMetaExtension::deserializeVariant(ViewEngine *engine, const MLNode &n){
    switch( n.type() ){
    case MLNode::Type::Object: {
        //Object / QVariantMap
        if ( n.hasKey("__type") ){
            QByteArray objectType = QByteArray(n["__type"].asString().c_str());
            //TODO
//            QmlMetaInfo::Ptr ti = engine->typeInfo(objectType);
//            if ( ti.isNull() || !ti->isSerializable() ){
//                THROW_EXCEPTION(lv::Exception, "Tuple deserialize: Unknown type: \'" + objectType .toStdString()+ "\'", 0);
//            }
//            QObject* ob = ti->deserialize(engine, n);
            QObject* ob = nullptr;
            return QVariant::fromValue(ob);
        }
        break;
    }
    case MLNode::Type::Array:{
        QVariantList l;
        for ( auto it = n.begin(); it != n.end(); ++it ){
            QVariant result = deserializeVariant(engine, it.value());
            l.append(result);
        }
        return l;
    }
    case MLNode::Type::Bytes:{
        THROW_EXCEPTION(lv::Exception, "Unexpected bytes type.", 0);
        break;
    }
    case MLNode::Type::String:{
        return QString::fromStdString(n.asString());
    }
    case MLNode::Type::Boolean:{
        return n.asBool();
    }
    case MLNode::Type::Integer:{
        return n.asInt();
    }
    case MLNode::Type::Float:{
        return n.asFloat();
    }
    case MLNode::Type::Null:{
        return QVariant();
    }
    }
    return QVariant();
}


}// namespace
