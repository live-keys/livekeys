#include "QStateContainer.hpp"
#include <QQuickView>
#include <QQuickItem>

QStateContainerManager* QStateContainerManager::m_instance = 0;

QStateContainerManager &QStateContainerManager::instance(QQuickItem *item, QObject* parent){
    if ( m_instance == 0 )
        m_instance = new QStateContainerManager(item, parent);
    return *m_instance;
}

QStateContainerManager::QStateContainerManager(QQuickItem *item, QObject *parent)
    : QObject(parent){

    atexit(&cleanStateManager);

    connect(item, SIGNAL(windowChanged(QQuickWindow*)), SLOT(attachWindow(QQuickWindow*)));
}

void QStateContainerManager::beforeCompile(){
    for ( QLinkedList<QStateContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        (*it)->beforeCompile();
}

void QStateContainerManager::afterCompile(){
    for ( QLinkedList<QStateContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        (*it)->afterCompile();
}

void QStateContainerManager::attachWindow(QQuickWindow *window){
    if (window != 0){
        QQuickView* view = qobject_cast<QQuickView*>(window);
        if ( view == 0 )
            qDebug() << "Cannot connect to window compilation signal. State objects will not get destroyed, "
                     << "which will cause the application to slow down over time.";
        else {
            connect(view->rootObject(), SIGNAL(beforeCompile()), SLOT(beforeCompile()));
            connect(view->rootObject(), SIGNAL(afterCompile()),  SLOT(afterCompile()));
        }
    }
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
