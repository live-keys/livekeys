#include "qmlmetainfo_p.h"
#include "live/viewengine.h"

#include <QQmlEngine>
#include <QQmlProperty>

namespace lv {

QmlMetaInfo::QmlMetaInfo(QObject *parent)
    : QObject(parent)
{
}

QmlMetaInfo::~QmlMetaInfo(){
}

bool QmlMetaInfo::isCppOwned(QObject *obj){
    return QQmlEngine::objectOwnership(obj) == QQmlEngine::CppOwnership;
}

bool QmlMetaInfo::isJsOwned(QObject *obj){
    return QQmlEngine::objectOwnership(obj) == QQmlEngine::JavaScriptOwnership;
}

QObject* QmlMetaInfo::cppParent(QObject *obj){
    return obj->parent();
}

bool QmlMetaInfo::isWritable(QObject *obj, QString name){
    return QQmlProperty(obj, name).isWritable();
}

QJSValue QmlMetaInfo::listProperties(QObject *obj){
    QQmlEngine* engine = qobject_cast<QQmlEngine*>(parent());
    if ( !engine )
        return QJSValue();

    const QMetaObject* meta = obj->metaObject();

    QList<QString> properties;

    for ( int i = 0; i < meta->propertyCount(); i++ ){
        QMetaProperty property = meta->property(i);
        properties.push_back(property.name());
    }

    QJSValue v = engine->newArray(properties.size());
    for ( int i = 0; i < properties.length(); ++i ){
        v.setProperty(i, properties[i]);
    }
    return v;
}

}
