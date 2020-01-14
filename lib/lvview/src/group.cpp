#include "group.h"
#include "live/visuallogqt.h"
#include "live/viewengine.h"
#include "live/typeinfo.h"
#include <QQmlComponent>
#include <QUrl>

namespace lv{

Group::Group(QObject *parent)
    : QObject(parent)
    , m_isComponentComplete(false)
{
}

Group::~Group(){
}

void Group::componentComplete(){
    m_isComponentComplete = true;
    emit complete();
}

QByteArray Group::typeDefaultValue(const QByteArray &typeString){
    if ( typeString == "bool" )
        return "false";
    else if ( typeString == "double" || typeString == "int" || typeString == "enumeration" || typeString == "real" )
        return "0";
    else if ( typeString == "list" )
        return "[]";
    else if ( typeString == "string" || typeString == "url " || typeString == "QUrl" || typeString == "QString" )
        return "\"\"";
    else if ( typeString == "color" || typeString == "QColor" )
        return "\"transparent\"";
    else if ( typeString == "point" || typeString == "QPoint" )
        return "\"0x0\"";
    else if ( typeString == "size" || typeString == "QSize" )
        return "\"0x0\"";
    else if ( typeString == "rect" || typeString == "QRect" )
        return "\50,50,100x100\"";
    else if ( typeString == "var" )
        return "undefined";
    else
        return "null";
}

Group *Group::createWithProperties(QQmlEngine *engine, const std::map<QByteArray, QByteArray> &properties){
    QByteArray propertyString;
    for ( auto it = properties.begin(); it != properties.end(); ++it ){
        propertyString += "property " + it->second + " " + it->first + ": " + typeDefaultValue(it->second) + "\n";
    }

    QByteArray propertyWrap = "import base 1.0\nimport QtQml 2.0\nGroup{\n" + propertyString + "}";

    QQmlComponent c(engine);
    c.setData(propertyWrap, QUrl("NewGroup.qml"));
    lv::Group* group = qobject_cast<lv::Group*>(c.create());

    if ( c.errors().size() > 0 ){
        qCritical("Failed to load group properties: %s", qPrintable(c.errorString()));
        return nullptr;
    }

    return group;
}

void Group::serialize(ViewEngine *engine, const QObject *o, MLNode &node){
    const Group* gr = qobject_cast<const Group*>(o);
    if ( !gr )
        THROW_EXCEPTION(lv::Exception, "Object not of group type.", Exception::toCode("~Group"));

    node = MLNode(MLNode::Object);
    node["__type"] = "lv::Group";

    const QMetaObject *meta = o->metaObject();
    for (int i = 0; i < meta->propertyCount(); i++){
        QMetaProperty property = meta->property(i);
        if ( property.name() != QByteArray("objectName") ){

            MLNode r;
            TypeInfo::serializeVariant(engine, property.read(o), r);
            node[property.name()] = r;
        }
    }
}

QObject *Group::deserialize(ViewEngine *engine, const MLNode &node){
    std::map<QByteArray, QByteArray> properties;
    for ( auto it = node.asObject().begin(); it != node.asObject().end(); ++it ){
        if ( it->first != "__type" ){
            switch( it->second.type() ){
            case MLNode::Type::Object: {
                if ( it->second.hasKey("__type") ){
                    properties[QByteArray::fromStdString(it->first)] = "QtObject";
                } else {
                    properties[QByteArray::fromStdString(it->first)] = "var";
                }
                break;
            }
            case MLNode::Type::Array:{
                properties[QByteArray::fromStdString(it->first)] = "var";
                break;
            }
            case MLNode::Type::Bytes:{
                THROW_EXCEPTION(lv::Exception, "Unexpected bytes type.", 0);
            }
            case MLNode::Type::String:{
                properties[QByteArray::fromStdString(it->first)] = "string";
                break;
            }
            case MLNode::Type::Boolean:{
                properties[QByteArray::fromStdString(it->first)] = "bool";
                break;
            }
            case MLNode::Type::Integer:{
                properties[QByteArray::fromStdString(it->first)] = "int";
                break;
            }
            case MLNode::Type::Float:{
                properties[QByteArray::fromStdString(it->first)] = "real";
                break;
            }
            default:
                break;
            }
        }
    }

    Group* gr = Group::createWithProperties(engine->engine(), properties);
    for ( auto it = node.asObject().begin(); it != node.asObject().end(); ++it ){
        if ( it->first != "__type" ){
            gr->setProperty(it->first.c_str(), TypeInfo::deserializeVariant(engine, it->second));
        }
    }

    return gr;
}

}// namespace
