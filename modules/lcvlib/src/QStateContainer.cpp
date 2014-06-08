#include "QStateContainer.hpp"


QStateContainerManager* QStateContainerManager::m_instance = 0;

QStateContainerManager &QStateContainerManager::instance(){
    if ( m_instance == 0 )
        m_instance = new QStateContainerManager();
    return *m_instance;
}

QStateContainerManager::QStateContainerManager() {
    atexit(&cleanStateManager);
}


QStateContainerManager::~QStateContainerManager(){
}

void QStateContainerManager::cleanStateManager() {
    delete m_instance;
    m_instance = 0;
}

