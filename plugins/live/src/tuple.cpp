#include "tuple.h"
#include <QHash>
#include <QHashIterator>
#include <QMetaProperty>
#include <QQmlProperty>
#include <QDebug>

namespace lv{

Tuple::Tuple(QObject *parent)
    : QObject(parent)
    , m_componentComplete(false)
{
}

Tuple::~Tuple(){
}

void Tuple::componentComplete(){
    m_componentComplete = true;

    const QMetaObject *meta = metaObject();

    for (int i = 0; i < meta->propertyCount(); i++){
        QMetaProperty property = meta->property(i);
        QByteArray name = property.name();
        if ( name != "objectName" ){
            m_properties[name] = i;

            QQmlProperty pp(this, name);

            Tuple::PropertyDescription pd;
            pd.typeCategory = pp.propertyTypeCategory();
            pd.typeClass    = pp.propertyTypeName();
            m_propertyTypes[i] = pd;
        }
    }
}

bool Tuple::reserveForRead(Shared::ReadScope *locker, Act *filter){
    const QMetaObject *meta = metaObject();
    for (int i = 0; i < meta->propertyCount(); i++){
        QMetaProperty property = meta->property(i);
        QObject* ob = property.read(this).value<QObject*>();
        if ( ob ){
            Tuple* t = qobject_cast<lv::Tuple*>(ob);
            Shared* sd = dynamic_cast<Shared*>(ob);
            if ( t ){
                if ( !t->reserveForRead(locker, filter) )
                    return false;
            } else if ( sd ){
                if ( !locker->read(sd) )
                    return false;
            }
        }
    }

    return true;
}

int Tuple::totalProperties(){
    return m_properties.size();
}

QHash<QByteArray, int>::ConstIterator Tuple::propertiesBegin(){
    return m_properties.begin();
}

QHash<QByteArray, int>::ConstIterator Tuple::propertiesEnd(){
    return m_properties.end();
}

//TODO: Lists are not yet supported
void Tuple::serialize(lv::ViewEngine* engine, const lv::Tuple &t, MLNode &node){
    node = MLNode(MLNode::Object);
    node["__type"] = "Tuple";

    const QMetaObject *meta = t.metaObject();
    for (int i = 0; i < meta->propertyCount(); i++){
        QMetaProperty property = meta->property(i);
        if ( property.name() != QByteArray("objectName") ){

            QVariant v = property.read(&t);

            if ( v.type() == QVariant::UInt ||
                 v.type() == QVariant::ULongLong ||
                 v.type() == QVariant::Int ||
                 v.type() == QVariant::LongLong
            ){
                node[property.name()] = v.toLongLong();
            } else if ( v.type() == QVariant::String || v.type() == QVariant::ByteArray ){
                node[property.name()] = v.toByteArray().toStdString();
            } else if ( v.type() == QVariant::Double ){
                node[property.name()] = v.toDouble();
            } else if ( v.type() == QVariant::Bool ){
                node[property.name()] = v.toBool();
            } else if ( v.value<QObject*>() || t.m_propertyTypes[i].typeCategory == QQmlProperty::Object){
                QObject* ob = v.value<QObject*>();
                if ( ob ){
                    TypeInfo::Ptr ti = engine->typeInfo(ob->metaObject());
                    if ( ti.isNull() || !ti->isSerializable() ){
                        node = MLNode(MLNode::Object);
                        lv::Exception e = CREATE_EXCEPTION(
                            lv::Exception, std::string("Non serializable Tuple object: ") + property.name(), 0
                        );
                        engine->throwError(&e);
                        return;
                    }

                    MLNode obSerialize;
                    ti->serialize(ob, obSerialize);

                    if ( obSerialize.type() == MLNode::Object ){
                        obSerialize["__type"] = ti->name().toStdString();
                    }
                    node[property.name()] = obSerialize;

                } else {
                    MLNode obSerialize = MLNode(MLNode::Object);
                    QByteArray obClass = t.m_propertyTypes[i].typeClass;
                    if ( !obClass.isEmpty() ){
                        obClass.replace("*", "").trimmed();
                        obSerialize["__type"] = obClass.constData();
                    }

                    node[property.name()] = obSerialize;
                }

            } else {
                node = MLNode(MLNode::Object);
                lv::Exception e = CREATE_EXCEPTION(
                    lv::Exception, std::string("Non serializable Tuple property: ") + property.name(), 0
                );
                engine->throwError(&e);
                return;
            }
        }
    }
}

void Tuple::deserialize(lv::ViewEngine *engine, const MLNode &n, lv::Tuple &t){
    try{
        if ( n.type() == MLNode::Type::Object ){
            for ( auto it = n.begin(); it != n.end(); ++it ){
                QVariant tp = t.property(it.key().c_str());
                if ( !tp.isValid() ){
                    lv::Exception e = CREATE_EXCEPTION(
                        lv::Exception, std::string("Tuple does not contain key \'") + it.key() + "\'" , 0
                    );
                    engine->throwError(&e);
                    return;
                }

                QVariant result;
                deserialize(engine, it.value(), result);
                t.setProperty(it.key().c_str(), result);
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

void Tuple::deserialize(ViewEngine *engine, const MLNode &n, QVariant &v){
    switch( n.type() ){
    case MLNode::Type::Object: {
        //Object / QVariantMap
        if ( n.hasKey("__type") ){
            QByteArray objectType = QByteArray(n["__type"].asString().c_str());
            TypeInfo::Ptr ti = engine->typeInfo(objectType);
            if ( ti.isNull() || !ti->isSerializable() ){
                THROW_EXCEPTION(lv::Exception, "Tuple deserialize: Unknown type: \'" + objectType .toStdString()+ "\'", 0);
            }

            QObject* ob = ti->newInstance();
            ti->deserialize(n, ob);

            v.setValue(ob);
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

}// namespace

