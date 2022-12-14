#include "qmlcontainer.h"

namespace lv{

QmlContainer::QmlContainer(QObject *parent)
    : QObject(parent)
{
}

QmlContainer::~QmlContainer(){
}

QQmlListProperty<QObject> QmlContainer::childObjects(){
    return QQmlListProperty<QObject>(this, this,
             &QmlContainer::appendChildObject,
             &QmlContainer::childObjectCount,
             &QmlContainer::childObject,
             &QmlContainer::clearChildObjects);
}

void QmlContainer::appendChildObject(QObject * object){
    m_childObjects.append(object);
}

int QmlContainer::childObjectCount() const{
    return m_childObjects.size();
}

QObject *QmlContainer::childObject(qsizetype index) const{
    return m_childObjects.at(index);
}

void QmlContainer::clearChildObjects(){
    m_childObjects.clear();
}

QObject *QmlContainer::prevChild(QObject *child) const{
    for ( auto it = m_childObjects.begin(); it != m_childObjects.end(); ++it ){
        if ( *it == child ){
            if ( it == m_childObjects.begin() )
                return nullptr;

            --it;
            return *it;
        }
    }
    return nullptr;
}

QObject *QmlContainer::nextChild(QObject *child) const{
    for ( auto it = m_childObjects.begin(); it != m_childObjects.end(); ++it ){
        if ( *it == child ){
            ++it;
            if ( it != m_childObjects.end() )
                return *it;
        }
    }
    return nullptr;
}

void QmlContainer::appendChildObject(QQmlListProperty<QObject>* list, QObject* o){
    reinterpret_cast<QmlContainer*>(list->data)->appendChildObject(o);
}

int QmlContainer::childObjectCount(QQmlListProperty<QObject>* list){
    return reinterpret_cast<QmlContainer*>(list->data)->childObjectCount();
}

QObject* QmlContainer::childObject(QQmlListProperty<QObject>* list, int index){
    return reinterpret_cast<QmlContainer*>(list->data)->childObject(index);
}

void QmlContainer::clearChildObjects(QQmlListProperty<QObject>* list){
    reinterpret_cast<QmlContainer*>(list->data)->clearChildObjects();
}

}// namespace
