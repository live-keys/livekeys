#include "qstaticcontainer.h"
#include <QQuickWindow>
#include <QQuickItem>
#include <QQmlContext>

QStaticContainer::QStaticContainer(QObject *parent)
    : QObject(parent)
{
}

QStaticContainer::~QStaticContainer(){
    for ( QLinkedList<QStaticTypeContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        delete *it;
    m_stateContainerList.clear();
}

void QStaticContainer::statecontainer(QStaticTypeContainerBase *container){
    m_stateContainerList.append(container);
}

QStaticContainer *QStaticContainer::grabFromContext(QQuickItem *item, const QString &contextProperty){
    return static_cast<QStaticContainer*>(qmlContext(item)->contextProperty(contextProperty).value<QObject*>());
}

void QStaticContainer::beforeCompile(){
    QSTATIC_ITEM_CONTAINER_DEBUG("-----Before Compile-----");
    for ( QLinkedList<QStaticTypeContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        (*it)->beforeCompile();
}

void QStaticContainer::afterCompile(){
    QSTATIC_ITEM_CONTAINER_DEBUG("-----After Compile-----");
    for ( QLinkedList<QStaticTypeContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        (*it)->afterCompile();
}

void QStaticContainer::clearStates(){
    QSTATIC_ITEM_CONTAINER_DEBUG("-----Clear States-----");
    for ( QLinkedList<QStaticTypeContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        (*it)->clearStates();
}
