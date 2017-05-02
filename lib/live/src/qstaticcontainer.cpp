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

void QStaticContainer::setWindow(QQuickWindow *window){
    connect(window, SIGNAL(aboutToRecompile()), SLOT(beforeCompile()));
    connect(window, SIGNAL(afterCompile()),  SLOT(afterCompile()));
    connect(window, SIGNAL(projectActiveChanged()), SLOT(clearStates()));
}

QStaticContainer *QStaticContainer::grabFromContext(QQuickItem *item, const QString &contextProperty){
    return static_cast<QStaticContainer*>(qmlContext(item)->contextProperty(contextProperty).value<QObject*>());
}

void QStaticContainer::beforeCompile(){
    for ( QLinkedList<QStaticTypeContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        (*it)->beforeCompile();
}

void QStaticContainer::afterCompile(){
    for ( QLinkedList<QStaticTypeContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        (*it)->afterCompile();
}

void QStaticContainer::clearStates(){
    for ( QLinkedList<QStaticTypeContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        (*it)->clearStates();
}
