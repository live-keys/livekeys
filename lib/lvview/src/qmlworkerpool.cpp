#include "qmlworkerpool.h"
#include "qmlworkerpool_p.h"

#include "live/applicationcontext.h"
#include "live/viewcontext.h"
#include "live/visuallogqt.h"
#include "live/shared.h"
#include "live/packagegraph.h"

#include <QJSEngine>
#include <QQmlEngine>
#include <QtDebug>
#include <QCoreApplication>

inline bool operator < (int priority, const QPair<lv::QmlWorkerPool::Task *, int> &p){
    return p.second < priority;
}

inline bool operator < (const QPair<lv::QmlWorkerPool::Task *, int> &p, int priority){
    return priority < p.second;
}

namespace lv{

Q_GLOBAL_STATIC(QmlWorkerPool, theInstance)

// class QmlWorkerPool::WorkerData
// ----------------------------------------------------------------------------

QmlWorkerPool::WorkerData::WorkerData()
    : m_engine(nullptr)
    , m_shouldClearCache(false)
{
}

QJSValue QmlWorkerPool::WorkerData::compileJsModule(
        const QString &file, const QString &id, const QByteArray &imports, const QByteArray &source)
{
    if ( m_shouldClearCache ){
        m_compiledSections.clear();
        m_shouldClearCache = false;
    }

    QString compileId = file + "#" + id;
    auto v = m_compiledSections.find(compileId);
    if ( v != m_compiledSections.end() ){
        return v.value();
    }

    ViewEngine::ComponentResult::Ptr cr = engine()->compileJsModule(imports, source, file);
    if ( cr->hasError() ){
        return QmlError::join(cr->errors).value();
    }

    QJSValue compiledFn = cr->object->property("exports").value<QJSValue>();
    m_compiledSections[compileId] = compiledFn;

    return compiledFn;
}

ViewEngine *QmlWorkerPool::WorkerData::engine(){
    if ( !m_engine ){
        m_engine = new ViewEngine(new QQmlEngine, ViewContext::instance().engine()->fileIO());
        m_engine->engine()->installExtensions(QJSEngine::ConsoleExtension);
        m_engine->setPackageGraph(new PackageGraph);
        QStringList engineImportPaths = m_engine->engine()->importPathList();

        engineImportPaths.removeAll(QString::fromStdString(ApplicationContext::instance().applicationPath()));
        engineImportPaths.removeAll(QString::fromStdString(ApplicationContext::instance().executablePath()));

        // Add the plugins directory to the import paths
        engineImportPaths.append(QString::fromStdString(ApplicationContext::instance().pluginPath()));

        m_engine->engine()->setImportPathList(engineImportPaths);
    }
    return m_engine;
}

// class QmlWorkerPool::Task
// ----------------------------------------------------------------------------

QmlWorkerPool::Task::~Task(){
}

// class QmlWorkerPool::QmlFunctionTask
// ----------------------------------------------------------------------------

QmlWorkerPool::QmlFunctionTask::QmlFunctionTask(
        const QString &file,
        const QString &id,
        const QByteArray& imports,
        const QByteArray &source,
        const QmlWorkerPool::TransferArguments& arguments,
        QObject* resultListener)
    : m_file(file)
    , m_id(id)
    , m_imports(imports)
    , m_source(source)
    , m_error(nullptr)
    , m_resultListener(resultListener)
    , m_arguments(arguments)
{
}

QmlWorkerPool::QmlFunctionTask::~QmlFunctionTask(){
    delete m_error;
}

void QmlWorkerPool::QmlFunctionTask::run(QmlWorkerPool::Thread* thread){
    QJSValue compiledFn = thread->workerData()->compileJsModule(m_file, m_id, m_imports, m_source);

    if ( compiledFn.isError() ){
        QmlError qe(thread->workerData()->engine(), compiledFn);

        if ( m_resultListener ){
            setAutoDelete(false);

            m_error = new Exception(Exception::create<Exception>(
                qe.message().toStdString(),
                static_cast<Exception::Code>(qe.code()),
                qe.fileName().toStdString(),
                qe.lineNumber(),
                qe.functionName().toStdString(),
                StackTrace::Ptr(nullptr)
            ));

            QCoreApplication::postEvent(m_resultListener, new QmlWorkerPool::TaskReadyEvent(this));
            return;

        } else {
            vlog().e() << "Uncaught error when compiling: "
                       << qe.toString(QmlError::PrintMessage | QmlError::PrintLocation | QmlError::PrintMessage);
            return;
        }

    }

    // run

    for ( Shared* obj : m_arguments.transfers ){
        obj->moveToThread(thread);
    }
    QJSValueList args;
    for ( const QVariant& v : m_arguments.values ){
        args.append(Shared::transfer(v, thread->workerData()->engine()->engine()));
    }

    QJSValue result = compiledFn.call(args);

    if ( m_resultListener ){
        setAutoDelete(false);

        QList<Shared*> robj;
        m_result = Shared::transfer(result, robj);

        for ( Shared* sh : robj ){
            QObject* o = static_cast<QObject*>(sh);
            o->moveToThread(m_resultListener->thread());
        }

        QCoreApplication::postEvent(m_resultListener, new QmlWorkerPool::TaskReadyEvent(this));
    }
}

const QVariant &QmlWorkerPool::QmlFunctionTask::result() const{
    return m_result;
}

bool QmlWorkerPool::QmlFunctionTask::isErrored() const{
    return m_error != nullptr;
}

Exception QmlWorkerPool::QmlFunctionTask::error() const{
    if ( !isErrored() )
        return Exception("");
    return *m_error;
}


// class QmlWorkerPool::TaskReadyEvent
// ----------------------------------------------------------------------------

QmlWorkerPool::TaskReadyEvent::~TaskReadyEvent(){
}


// class QmlWorkerPool::Thread
// ----------------------------------------------------------------------------

QmlWorkerPool::Thread::Thread(QmlWorkerPoolPrivate *workerContainer)
    : m_workerContainer(workerContainer)
    , m_workerData(new QmlWorkerPool::WorkerData)
    , m_task(nullptr)
{
}

void QmlWorkerPool::Thread::run()
{
    QMutexLocker locker(&m_workerContainer->mutex);
    for(;;) {
        QmlWorkerPool::Task *t = m_task;
        m_task = nullptr;

        do {
            if (t) {
                const bool autoDelete = t->autoDelete();

                // run the task
                locker.unlock();
#ifndef QT_NO_EXCEPTIONS
                try {
#endif
                    t->run(this);
#ifndef QT_NO_EXCEPTIONS
                } catch (...) {
                    qWarning("QmlWorkerPool: Exception caught in worker thread. This is not supported.");
                    registerTheadInactive();
                    throw;
                }
#endif
                locker.relock();

                if (autoDelete && !--t->m_ref)
                    delete t;
            }

            // if too many threads are active, expire this thread
            if (m_workerContainer->tooManyThreadsActive())
                break;

            t = !m_workerContainer->queue.isEmpty() ? m_workerContainer->queue.takeFirst().first : nullptr;
        } while (t != nullptr);

        if (m_workerContainer->isExiting) {
            registerTheadInactive();
            break;
        }

        // if too many threads are active, expire this thread
        bool expired = m_workerContainer->tooManyThreadsActive();
        if (!expired) {
            ++m_workerContainer->waitingThreads;
            registerTheadInactive();
            // wait for work, exiting after the expiry timeout is reached
            expired = !m_workerContainer->taskReady.wait(locker.mutex(), m_workerContainer->expiryTimeout);
            ++m_workerContainer->activeThreads;

            if (expired)
                --m_workerContainer->waitingThreads;
        }
        if (expired) {
            m_workerContainer->expiredThreads.enqueue(this);
            registerTheadInactive();
            break;
        }
    }
}

void QmlWorkerPool::Thread::registerTheadInactive()
{
    if (--m_workerContainer->activeThreads == 0)
        m_workerContainer->noActiveThreads.wakeAll();
}

QmlWorkerPool::WorkerData *QmlWorkerPool::Thread::workerData() const{
    return m_workerData;
}

QmlWorkerPoolPrivate:: QmlWorkerPoolPrivate()
    : isExiting(false)
    , expiryTimeout(30000)
    , maxThreadCount(qAbs(QThread::idealThreadCount()))
    , reservedThreads(0)
    , waitingThreads(0)
    , activeThreads(0)
{ }

bool QmlWorkerPoolPrivate::tryStart(QmlWorkerPool::Task *task)
{
    if (allThreads.isEmpty()) {
        // always create at least one thread
        startThread(task);
        return true;
    }

    // can't do anything if we're over the limit
    if (activeThreadCount() >= maxThreadCount)
        return false;

    if (waitingThreads > 0) {
        // recycle an available thread
        --waitingThreads;
        enqueueTask(task);
        return true;
    }

    if (!expiredThreads.isEmpty()) {
        // restart an expired thread
        QmlWorkerPool::Thread *thread = expiredThreads.dequeue();
        Q_ASSERT(thread->m_task == nullptr);

        ++activeThreads;

        if (task->autoDelete())
            ++task->m_ref;
        thread->m_task = task;
        thread->start();
        return true;
    }

    // start a new thread
    startThread(task);
    return true;
}

void QmlWorkerPoolPrivate::enqueueTask(QmlWorkerPool::Task *task, int priority)
{
    if (task->autoDelete())
        ++task->m_ref;

    // put it on the queue
    QList<QPair<QmlWorkerPool::Task *, int> >::iterator at =
        qUpperBound(queue.begin(), queue.end(), priority);
    queue.insert(at, qMakePair(task, priority));
    taskReady.wakeOne();
}

int QmlWorkerPoolPrivate::activeThreadCount() const
{
    // To improve scalability this function is called without holding
    // the mutex lock -- keep it thread-safe.
    return (allThreads.count()
            - expiredThreads.count()
            - waitingThreads
            + reservedThreads);
}

void QmlWorkerPoolPrivate::tryToStartMoreThreads()
{
    // try to push tasks on the queue to any available threads
    while (!queue.isEmpty() && tryStart(queue.first().first))
        queue.removeFirst();
}

bool QmlWorkerPoolPrivate::tooManyThreadsActive() const
{
    const int activeThreadCount = this->activeThreadCount();
    return activeThreadCount > maxThreadCount && (activeThreadCount - reservedThreads) > 1;
}

/*! \internal
*/
void QmlWorkerPoolPrivate::startThread(QmlWorkerPool::Task *task)
{
    QScopedPointer <QmlWorkerPool::Thread> thread(new QmlWorkerPool::Thread(this));
    allThreads.insert(thread.data());
    ++activeThreads;

    if (task->autoDelete())
        ++task->m_ref;
    thread->m_task = task;
    thread.take()->start();
}

/*! \internal
    Makes all threads exit, waits for each tread to exit and deletes it.
*/
void QmlWorkerPoolPrivate::reset()
{
    QMutexLocker locker(&mutex);
    isExiting = true;
    taskReady.wakeAll();

    do {
        // make a copy of the set so that we can iterate without the lock
        QSet<QmlWorkerPool::Thread *> allThreadsCopy = allThreads;
        allThreads.clear();
        locker.unlock();

        foreach (QmlWorkerPool::Thread *thread, allThreadsCopy) {
            thread->wait();
            delete thread;
        }

        locker.relock();
        // repeat until all newly arrived threads have also completed
    } while (!allThreads.isEmpty());

    waitingThreads = 0;
    expiredThreads.clear();

    isExiting = false;
}

void QmlWorkerPoolPrivate::waitForDone()
{
    QMutexLocker locker(&mutex);
    while (!(queue.isEmpty() && activeThreads == 0))
        noActiveThreads.wait(locker.mutex());
}

void QmlWorkerPoolPrivate::clearThreadCache(){
    for ( auto thread : allThreads ){
        thread->workerData()->clearCache();
    }
}

QmlWorkerPool::QmlWorkerPool(QObject *parent)
    : QObject(parent)
{
    d_ptr = new QmlWorkerPoolPrivate;
}

QmlWorkerPool::~QmlWorkerPool()
{
    d_func()->waitForDone();
    d_func()->reset();
}

QmlWorkerPool *QmlWorkerPool::globalInstance(){
    return theInstance();
}

void QmlWorkerPool::start(QmlWorkerPool::Task *task, int priority){
    if (!task)
        return;

    Q_D(QmlWorkerPool);
    QMutexLocker locker(&d->mutex);
    if (!d->tryStart(task))
        d->enqueueTask(task, priority);
}

bool QmlWorkerPool::tryStart(QmlWorkerPool::Task *task){
    if (!task)
        return false;

    Q_D(QmlWorkerPool);

    // To improve scalability perform a check on the thread count
    // before locking the mutex.
    if (d->allThreads.isEmpty() == false && d->activeThreadCount() >= d->maxThreadCount)
        return false;

    QMutexLocker locker(&d->mutex);
    return d->tryStart(task);
}

int QmlWorkerPool::expiryTimeout() const
{
    Q_D(const QmlWorkerPool);
    return d->expiryTimeout;
}

void QmlWorkerPool::setExpiryTimeout(int expiryTimeout)
{
    Q_D(QmlWorkerPool);
    if (d->expiryTimeout == expiryTimeout)
        return;
    d->expiryTimeout = expiryTimeout;
}

int QmlWorkerPool::maxThreadCount() const{
    Q_D(const QmlWorkerPool);
    return d->maxThreadCount;
}

void QmlWorkerPool::setMaxThreadCount(int maxThreadCount){
    Q_D(QmlWorkerPool);
    QMutexLocker locker(&d->mutex);

    if (maxThreadCount == d->maxThreadCount)
        return;

    d->maxThreadCount = maxThreadCount;
    d->tryToStartMoreThreads();
}

int QmlWorkerPool::activeThreadCount() const{
    Q_D(const QmlWorkerPool);
    return d->activeThreadCount();
}

void QmlWorkerPool::reserveThread(){
    Q_D(QmlWorkerPool);
    QMutexLocker locker(&d->mutex);
    ++d->reservedThreads;
}

void QmlWorkerPool::releaseThread(){
    Q_D(QmlWorkerPool);
    QMutexLocker locker(&d->mutex);
    --d->reservedThreads;
    d->tryToStartMoreThreads();
}

void QmlWorkerPool::waitForDone(){
    Q_D(QmlWorkerPool);
    d->waitForDone();
    d->reset();
}

void QmlWorkerPool::clearThreadCache(){
    Q_D(QmlWorkerPool);
    d->clearThreadCache();
}

}// namespace
