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

#include "typeinfo.h"
#include <QVariant>
#include "live/viewengine.h"
#include "live/group.h"

namespace lv{

TypeInfo::TypeInfo(const QByteArray &name)
    : m_name(name)
    , m_serialize(nullptr)
    , m_log(nullptr)
{
}

TypeInfo::~TypeInfo(){

}

TypeInfo::Ptr TypeInfo::create(const QByteArray &name){
    return TypeInfo::Ptr(new TypeInfo(name));
}

void TypeInfo::serializeVariant(ViewEngine *engine, const QVariant &v, MLNode &node){
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

        const QMetaObject* mo = ob->metaObject();
        if ( mo->inherits(&lv::Group::staticMetaObject))
            mo = &lv::Group::staticMetaObject;

        TypeInfo::Ptr ti = engine->typeInfo(mo);
        if ( ti.isNull() || !ti->isSerializable() ){
            node = MLNode(MLNode::Object);
            lv::Exception e = CREATE_EXCEPTION(
                lv::Exception, std::string("Non serializable object: ") + ob->metaObject()->className(), 0
            );
            engine->throwError(&e);
            return;
        }

        MLNode obSerialize;
        ti->serialize(engine, ob, obSerialize);

        if ( obSerialize.type() == MLNode::Object ){
            obSerialize["__type"] = ti->name().toStdString();
        }
        node = obSerialize;

    } else if ( v.canConvert(QMetaType::QVariantList) ){

        QVariantList vl = v.value<QVariantList>();
        node = MLNode(MLNode::Array);

        for ( auto it = vl.begin(); it != vl.end(); ++it ){
            MLNode result;
            TypeInfo::serializeVariant(engine, *it, result);
            node.append(result);
        }
    }
}

QVariant TypeInfo::deserializeVariant(ViewEngine *engine, const MLNode &n){
    switch( n.type() ){
    case MLNode::Type::Object: {
        //Object / QVariantMap
        if ( n.hasKey("__type") ){
            QByteArray objectType = QByteArray(n["__type"].asString().c_str());
            TypeInfo::Ptr ti = engine->typeInfo(objectType);
            if ( ti.isNull() || !ti->isSerializable() ){
                THROW_EXCEPTION(lv::Exception, "Tuple deserialize: Unknown type: \'" + objectType .toStdString()+ "\'", 0);
            }

            QObject* ob = ti->deserialize(engine, n);

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
