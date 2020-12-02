#ifndef LVQMLWORKERPOOLOBJECT_H
#define LVQMLWORKERPOOLOBJECT_H

#include <QObject>
#include "live/qmlworkerinterface.h"
#include "live/qmlworkerpool.h"

namespace lv{

class QmlWorkerPoolObject : public QmlWorkerInterface{

    Q_OBJECT

public:
    explicit QmlWorkerPoolObject(QObject *parent = nullptr);
    ~QmlWorkerPoolObject() override;

    void start(QmlWorkerPool::Task *task, int priority = 9) override;

private:
    QmlWorkerPool* m_workerPool;
    static bool    m_hasInitializedCacheReset;
};

}// namespace

#endif // LVQMLWORKERPOOLOBJECT_H
