#include "qmlvalueflow.h"
#include "live/viewengine.h"
#include "live/visuallogqt.h"

namespace lv{

QmlValueFlow::QmlValueFlow(QObject *parent)
    : QObject(parent)
{

}

QmlValueFlow::~QmlValueFlow(){
}

QQmlListProperty<QObject> QmlValueFlow::childObjects(){
    return QQmlListProperty<QObject>(this, this,
             &QmlValueFlow::appendChildObject,
             &QmlValueFlow::childObjectCount,
             &QmlValueFlow::childObject,
             &QmlValueFlow::clearChildObjects);
}

void QmlValueFlow::appendChildObject(QObject *obj){
    m_childObjects.append(obj);
}

int QmlValueFlow::childObjectCount() const{
    return m_childObjects.size();
}

QObject *QmlValueFlow::childObject(int index) const{
    return m_childObjects.at(index);
}

void QmlValueFlow::clearChildObjects(){
    m_childObjects.clear();
}

void QmlValueFlow::appendChildObject(QQmlListProperty<QObject> *list, QObject *o){
    reinterpret_cast<QmlValueFlow*>(list->data)->appendChildObject(o);
}

int QmlValueFlow::childObjectCount(QQmlListProperty<QObject> *list){
    return reinterpret_cast<QmlValueFlow*>(list->data)->childObjectCount();
}

QObject *QmlValueFlow::childObject(QQmlListProperty<QObject> *list, int index){
    return reinterpret_cast<QmlValueFlow*>(list->data)->childObject(index);
}

void QmlValueFlow::clearChildObjects(QQmlListProperty<QObject> *list){
    reinterpret_cast<QmlValueFlow*>(list->data)->clearChildObjects();
}

}// namespace
