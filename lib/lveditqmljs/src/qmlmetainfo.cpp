#include "qmlmetainfo_p.h"
#include <QQmlEngine>
#include <QQmlProperty>

namespace lv {

QmlMetaInfo::QmlMetaInfo(QObject *parent) : QObject(parent)
{

}

bool QmlMetaInfo::isCppOwned(QObject *obj)
{
    return QQmlEngine::objectOwnership(obj) == QQmlEngine::CppOwnership;
}

bool QmlMetaInfo::isJsOwned(QObject *obj)
{
    return QQmlEngine::objectOwnership(obj) == QQmlEngine::JavaScriptOwnership;
}

QObject* QmlMetaInfo::cppParent(QObject *obj){
    return obj->parent();
}

bool QmlMetaInfo::isWritable(QObject *obj, QString name)
{
    return QQmlProperty(obj, name).isWritable();
}

}
