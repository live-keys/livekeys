#ifndef LVQMLWORKERPOOL_P_H
#define LVQMLWORKERPOOL_P_H

#include "qmlworkerpool.h"

namespace lv{

class QmlWorkerPoolPrivate{

    friend class QmlWorkerPool;

public:
    QmlWorkerPoolPrivate();

    bool tryStart(QmlWorkerPool::Task *task);
    void enqueueTask(QmlWorkerPool::Task *task, int priority = 0);
    int activeThreadCount() const;

    void tryToStartMoreThreads();
    bool tooManyThreadsActive() const;

    void startThread(QmlWorkerPool::Task *task = 0);
    void reset();
    void waitForDone();

    mutable QMutex mutex;
    QWaitCondition taskReady;
    QSet<QmlWorkerPool::Thread*>   allThreads;
    QQueue<QmlWorkerPool::Thread*> expiredThreads;
    QList< QPair<QmlWorkerPool::Task*,int> > queue;
    QWaitCondition noActiveThreads;

    bool isExiting;
    int  expiryTimeout;
    int  maxThreadCount;
    int  reservedThreads;
    int  waitingThreads;
    int  activeThreads;
};

}// namespace

#endif // LVQMLWORKERPOOL_P_H
