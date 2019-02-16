#include "tuplepropertymap.h"

namespace lv{

bool TuplePropertyMap::reserveForRead(const QQmlPropertyMap &t, Shared::ReadScope *locker, Filter *filter){
    QStringList keys = t.keys();

    for (auto it = keys.begin(); it != keys.end(); ++it ){

        QObject* ob = t.property(it->toUtf8()).value<QObject*>();
        if ( ob ){
            Tuple* t = qobject_cast<lv::Tuple*>(ob);
            Shared* sd = dynamic_cast<Shared*>(ob);
            if ( t ){
                if ( !t->reserveForRead(locker, filter) )
                    return false;
            } else if ( sd ){
                if ( !locker->read(sd) )
                    return false;
            } else {
                QQmlPropertyMap* pm = qobject_cast<QQmlPropertyMap*>(ob);
                if ( pm ){
                    reserveForRead(*pm, locker, filter);
                }
            }
        }
    }

    return true;
}

bool TuplePropertyMap::reserveForWrite(const QQmlPropertyMap &t, Shared::ReadScope *locker, Filter *filter){
    QStringList keys = t.keys();

    for (auto it = keys.begin(); it != keys.end(); ++it ){

        QObject* ob = t.property(it->toUtf8()).value<QObject*>();
        if ( ob ){
            Tuple* t = qobject_cast<lv::Tuple*>(ob);
            Shared* sd = dynamic_cast<Shared*>(ob);
            if ( t ){
//                if ( !t->reserveForWrite(locker, filter) )
//                    return false;
            } else if ( sd ){
//                if ( !locker->write(sd) )
//                    return false;
            } else {
                QQmlPropertyMap* pm = qobject_cast<QQmlPropertyMap*>(ob);
                if ( pm ){
                    reserveForWrite(*pm, locker, filter);
                }
            }
        }
    }

    return true;
}

void TuplePropertyMap::serialize(ViewEngine *engine, const QQmlPropertyMap &t, MLNode &node){
    node = MLNode(MLNode::Object);

    QStringList keys = t.keys();

    for (auto it = keys.begin(); it != keys.end(); ++it ){

        QVariant v = t.property(it->toUtf8());

        if ( v.type() == QVariant::UInt ||
             v.type() == QVariant::ULongLong ||
             v.type() == QVariant::Int ||
             v.type() == QVariant::LongLong
        ){
            node[it->toStdString()] = v.toLongLong();
        } else if ( v.type() == QVariant::String || v.type() == QVariant::ByteArray ){
            node[it->toStdString()] = v.toByteArray().toStdString();
        } else if ( v.type() == QVariant::Double ){
            node[it->toStdString()] = v.toDouble();
        } else if ( v.type() == QVariant::Bool ){
            node[it->toStdString()] = v.toBool();
        } else if ( v.value<QObject*>()){
            QObject* ob = v.value<QObject*>();

            QQmlPropertyMap* obpm = qobject_cast<QQmlPropertyMap*>(ob);

            if ( obpm ){
                MLNode obSerialize;
                TuplePropertyMap::serialize(engine, *obpm, obSerialize);
                obSerialize["__type"] = "Tuple";
                node[it->toStdString()] = obSerialize;

            } else if ( ob ){
                TypeInfo::Ptr ti = engine->typeInfo(ob->metaObject());
                if ( ti.isNull() || !ti->isSerializable() ){
                    node = MLNode(MLNode::Object);
                    lv::Exception e = CREATE_EXCEPTION(
                        lv::Exception, "Non serializable Tuple object: " + QString(*it).toStdString(), 0
                    );
                    engine->throwError(&e);
                    return;
                }

                MLNode obSerialize;
                ti->serialize(ob, obSerialize);
                if ( obSerialize.type() == MLNode::Object ){
                    obSerialize["__type"] = ti->name().toStdString();
                }
                node[it->toStdString()] = obSerialize;

            } else {
                MLNode obSerialize = MLNode(MLNode::Object);
                node[it->toStdString()] = obSerialize;
            }

        }
    }
}

void TuplePropertyMap::deserialize(ViewEngine *engine, const MLNode &n, QQmlPropertyMap &t){
    try{
        if ( n.type() == MLNode::Type::Object ){
            for ( auto it = n.begin(); it != n.end(); ++it ){
                if ( it.key() != "__type" ){
                    QVariant result;
                    deserialize(engine, it.value(), result);
                    t.insert(QByteArray::fromStdString(it.key().c_str()), result);
                }
            }
        } else {
            lv::Exception e = CREATE_EXCEPTION(lv::Exception, "MLNode not of object type.", 0);
            engine->throwError(&e);
            return;
        }
    } catch ( lv::Exception& e ){
        engine->throwError(&e, 0);
    }
}

void TuplePropertyMap::deserialize(ViewEngine *engine, const MLNode &n, QVariant &v){
    switch( n.type() ){
    case MLNode::Type::Object: {
        //Object / QVariantMap
        if ( n.hasKey("__type") ){

            QByteArray objectType = QByteArray(n["__type"].asString().c_str());

            if ( objectType == "Tuple" ){

                QQmlPropertyMap* qpm = new QQmlPropertyMap;
                deserialize(engine, n, *qpm);
                v.setValue(qpm);

            } else {
                TypeInfo::Ptr ti = engine->typeInfo(objectType);
                if ( ti.isNull() || !ti->isSerializable() ){
                    THROW_EXCEPTION(
                        lv::Exception, "Tuple deserialize: Unknown type: \'" + objectType.toStdString() + "\'", 0
                    );
                }

                QObject* ob = ti->newInstance();
                ti->deserialize(n, ob);
                v.setValue(ob);
            }
        }
        break;
    }
    case MLNode::Type::Array:{
        QVariantList l;
        for ( auto it = n.begin(); it != n.end(); ++it ){
            QVariant result;
            deserialize(engine, it.value(), result);
            l.append(v);
        }
        v = l;
        break;
    }
    case MLNode::Type::Bytes:{
        THROW_EXCEPTION(lv::Exception, "Unexpected bytes type.", 0);
        break;
    }
    case MLNode::Type::String:{
        v = QString::fromStdString(n.asString());
        break;
    }
    case MLNode::Type::Boolean:{
        v = n.asBool();
        break;
    }
    case MLNode::Type::Integer:{
        v = n.asInt();
        break;
    }
    case MLNode::Type::Float:{
        v = n.asFloat();
        break;
    }
    default:
        break;
    }
}


} // namespace
