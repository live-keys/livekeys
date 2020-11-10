#ifndef LVQMLWORKERPOOL_H
#define LVQMLWORKERPOOL_H

#include "live/lvviewglobal.h"
#include "live/viewengine.h"

#include <QVariant>
#include <QVariantList>
#include <QEvent>
#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QSet>
#include <QMap>
#include <QQueue>
#include <QJSValue>
#include <QQmlEngine>

namespace lv{

class Shared;
class QmlWorkerPoolPrivate;

class LV_VIEW_EXPORT QmlWorkerPool : public QObject{

    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlWorkerPool)

public:
    class Thread;

    class TransferArguments{

    public:
        TransferArguments(
                const QVariantList& pvalues = QVariantList(),
                const QList<Shared*>& ptransfers = QList<Shared*>())
            : values(pvalues)
            , transfers(ptransfers)
        {}

    public:
        QVariantList   values;
        QList<Shared*> transfers;
    };

    class LV_VIEW_EXPORT WorkerData{

        friend class Thread;
        Q_DISABLE_COPY(WorkerData)

    public:
        WorkerData();

        QJSValue compileJsModule(
            const QString& file,
            const QString& id,
            const QByteArray &imports,
            const QByteArray &source);
        ViewEngine *engine();

    private:
        ViewEngine*             m_engine;
        QMap<QString, QJSValue> m_compiledSections;
    };

    class LV_VIEW_EXPORT Task{

        friend class QmlWorkerPool;
        friend class QmlWorkerPoolPrivate;
        friend class Thread;
        Q_DISABLE_COPY(Task)

    public:
        Task() : m_ref(0) {}
        virtual ~Task();

        virtual void run(Thread*) = 0;

        bool autoDelete() const { return m_ref != -1; }
        void setAutoDelete(bool _autoDelete) { m_ref = _autoDelete ? 0 : -1; }

    private:
        int m_ref;
    };

    class LV_VIEW_EXPORT Thread : public QThread{
    public:
        Thread(QmlWorkerPoolPrivate *manager);
        void run();
        void registerTheadInactive();

        WorkerData* workerData() const;

        QmlWorkerPoolPrivate *m_workerContainer;
        WorkerData           *m_workerData;
        Task                 *m_task;
    };

    class LV_VIEW_EXPORT QmlFunctionTask : public Task{

    public:
        QmlFunctionTask(
            const QString& file,
            const QString& id,
            const QByteArray& imports,
            const QByteArray& contents,
            const TransferArguments& arguments = TransferArguments(),
            QObject* resultListener = nullptr);
        ~QmlFunctionTask();
        void run(Thread* thread) override;

        const QVariant& result() const;

        bool isErrored() const;
        Exception error() const;

    private:
        QString    m_file;
        QString    m_id;
        QByteArray m_imports;
        QByteArray m_source;
        QVariant   m_result;
        Exception* m_error;
        QObject*   m_resultListener;

        TransferArguments m_arguments;
    };

    class LV_VIEW_EXPORT TaskReadyEvent : public QEvent{

    public:
        TaskReadyEvent(Task* task) : QEvent(QEvent::None), m_task(task){}
        Task* task(){ return m_task; }
        void clearTask(){ delete m_task; m_task = nullptr; }

    private:
        Task* m_task;
    };

public:
    QmlWorkerPool(QObject *parent = nullptr);
    ~QmlWorkerPool();

    static QmlWorkerPool *globalInstance();

    void start(Task *task, int priority = 0);
    bool tryStart(Task *task);

    int expiryTimeout() const;
    void setExpiryTimeout(int expiryTimeout);

    int maxThreadCount() const;
    void setMaxThreadCount(int maxThreadCount);

    int activeThreadCount() const;

    void reserveThread();
    void releaseThread();

    void waitForDone();

private:
    QmlWorkerPoolPrivate* d_ptr;
};

}// namespace

#endif // LVQMLWORKERPOOL_H
