#include "qmlworkerpoolobject.h"

namespace lv{

QmlWorkerPoolObject::QmlWorkerPoolObject(QObject *parent)
    : QmlWorkerInterface(parent)
    , m_workerPool(QmlWorkerPool::globalInstance())
{
}

QmlWorkerPoolObject::~QmlWorkerPoolObject(){
}

void QmlWorkerPoolObject::start(QmlWorkerPool::Task *task, int priority){
    m_workerPool->start(task, priority);
}

}// namespace lv
