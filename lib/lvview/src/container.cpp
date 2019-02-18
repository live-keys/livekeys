#include "container.h"

namespace lv{

Container::Container(QObject *parent)
    : QObject(parent)
{
}

Container::~Container(){
}

QQmlListProperty<QObject> Container::childObjects(){
    return QQmlListProperty<QObject>(this, this,
             &Container::appendChildObject,
             &Container::childObjectCount,
             &Container::childObject,
             &Container::clearChildObjects);
}

void Container::appendChildObject(QObject * object){
    m_childObjects.append(object);
}

int Container::childObjectCount() const{
    return m_childObjects.size();
}

QObject *Container::childObject(int index) const{
    return m_childObjects.at(index);
}

void Container::clearChildObjects(){
    m_childObjects.clear();
}


void Container::appendChildObject(QQmlListProperty<QObject>* list, QObject* o){
    reinterpret_cast<Container*>(list->data)->appendChildObject(o);
}

int Container::childObjectCount(QQmlListProperty<QObject>* list){
    return reinterpret_cast<Container*>(list->data)->childObjectCount();
}

QObject* Container::childObject(QQmlListProperty<QObject>* list, int index){
    return reinterpret_cast<Container*>(list->data)->childObject(index);
}

void Container::clearChildObjects(QQmlListProperty<QObject>* list){
    reinterpret_cast<Container*>(list->data)->clearChildObjects();
}

}// namespace
