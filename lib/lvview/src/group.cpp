#include "group.h"
#include "live/visuallogqt.h"
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

    QByteArray propertyWrap = "import base 1.0\nGroup{\n" + propertyString + "}";

    QQmlComponent c(engine);
    c.setData(propertyWrap, QUrl("NewGroup.qml"));
    lv::Group* group = qobject_cast<lv::Group*>(c.create());

    if ( c.errors().size() > 0 ){
        qCritical("Failed to load group properties: %s", qPrintable(c.errorString()));
        return nullptr;
    }

    return group;
}

}// namespace
