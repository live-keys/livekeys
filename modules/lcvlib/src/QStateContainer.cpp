#include "QStateContainer.hpp"
#include <QQuickView>
#include <QQuickItem>

QStateContainerManager* QStateContainerManager::m_instance = 0;

QStateContainerManager &QStateContainerManager::instance(QQuickView *view, QObject* parent){
    if ( m_instance == 0 )
        m_instance = new QStateContainerManager(view, parent);
    return *m_instance;
}

QStateContainerManager::QStateContainerManager(QQuickView *view, QObject *parent)
    : QObject(parent){

    atexit(&cleanStateManager);

    connect(view->rootObject(), SIGNAL(beforeCompile()), SLOT(beforeCompile()));
}

void QStateContainerManager::beforeCompile(){
    for ( QLinkedList<QStateContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        (*it)->beforeCompile();
}

QStateContainerManager::~QStateContainerManager(){
    for ( QLinkedList<QStateContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        delete *it;
    m_stateContainerList.clear();
}

void QStateContainerManager::registerStateContainer(QStateContainerBase *container){
    m_stateContainerList.append(container);
}

void QStateContainerManager::cleanStateManager() {
    delete m_instance;
    m_instance = 0;
}


QStateContainerBase::QStateContainerBase(){
}
