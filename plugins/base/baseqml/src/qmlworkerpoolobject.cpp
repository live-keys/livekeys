#include "qmlworkerpoolobject.h"

#include "live/viewcontext.h"
#include "live/project.h"
#include "live/runnablecontainer.h"
#include "live/qmlbuild.h"

#include <QQmlContext>

#include <QtDebug>

namespace lv{

bool QmlWorkerPoolObject::m_hasInitializedCacheReset = false;

QmlWorkerPoolObject::QmlWorkerPoolObject(QObject *parent)
    : QmlWorkerInterface(parent)
    , m_workerPool(QmlWorkerPool::globalInstance())
{
    if ( !m_hasInitializedCacheReset ){
        QQmlEngine* engine = qobject_cast<QQmlEngine*>(parent);
        Project* proj = qobject_cast<Project*>(engine->rootContext()->contextProperty("project").value<QObject*>());
        if ( proj ){
            connect(proj->runnables(), &RunnableContainer::qmlBuild, m_workerPool, &QmlWorkerPool::clearThreadCache);
            m_hasInitializedCacheReset = true;
        }
    }
}

QmlWorkerPoolObject::~QmlWorkerPoolObject(){
}

void QmlWorkerPoolObject::start(QmlWorkerPool::Task *task, int priority){
    m_workerPool->start(task, priority);
}

}// namespace lv
