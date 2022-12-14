#include "qmlvalueflowgraph.h"
#include "live/visuallogqt.h"
#include "live/qmlact.h"
#include "live/documentqmlinfo.h"
#include "live/qmlstreamprovider.h"
#include "live/qmlstream.h"

#include "qmlfunctionobjectcall.h"

#include <QQmlContext>
#include <QJSValue>
#include <QJSValueIterator>
#include <QCoreApplication>

namespace lv{

// class QmlValueFlowGraph::WorkerTask
// ----------------------------------------------------------------------------

QmlValueFlowGraph::WorkerTask::WorkerTask(
        const QmlFunctionObjectCall::GraphSource& source,
        const QmlWorkerPool::TransferArguments &arguments,
        QObject *resultListener)
    : m_source(source)
    , m_error(nullptr)
    , m_resultListener(resultListener)
    , m_arguments(arguments)
{
}

QmlValueFlowGraph::WorkerTask::~WorkerTask(){
    delete m_error;
}

void QmlValueFlowGraph::WorkerTask::run(QmlWorkerPool::Thread *thread){
    ViewEngine* ve = thread->workerData()->engine();

    std::vector<QmlFunctionObjectCall*> graph;
    try{
        std::vector<QmlFunctionObjectCall*> collectedGraph = QmlFunctionObjectCall::mapSourceToObjects(ve, m_source);
        graph = QmlFunctionObjectCall::resolveDependencies(collectedGraph);
    } catch( lv::Exception& e ){
        if ( m_resultListener ){
            setAutoDelete(false);
            m_error = new Exception(e);
            QCoreApplication::postEvent(m_resultListener, new QmlWorkerPool::TaskReadyEvent(this));
        } else {
            vlog().e() << "Uncaught error when compiling: " << e.message();
        }
        return;
    }

    for ( Shared* obj : m_arguments.transfers ){
        obj->moveToThread(thread);
    }
    QJSValue arg = m_arguments.values.size() ? Shared::transfer(m_arguments.values.first(), ve->engine()) : QJSValue();

    QJSValue result = QmlFunctionObjectCall::runGraph(ve, arg, graph);
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

const QVariant &QmlValueFlowGraph::WorkerTask::result() const{
    return m_result;
}

bool QmlValueFlowGraph::WorkerTask::isErrored() const{
    return m_error != nullptr;
}

Exception QmlValueFlowGraph::WorkerTask::error() const{
    if ( !isErrored() )
        return Exception("");
    return *m_error;
}

// class QmlValueFlowGraph
// ----------------------------------------------------------------------------

QmlValueFlowGraph::QmlValueFlowGraph(QObject *parent)
    : QObject(parent)
    , m_source(nullptr)
    , m_isComponentComplete(false)
    , m_graphSource(new QmlFunctionObjectCall::GraphSource)
    , m_graph(new std::vector<QmlFunctionObjectCall*>)
    , m_waitOn(nullptr)
    , m_worker(nullptr)
    , m_currentTask(nullptr)
{
}

QmlValueFlowGraph::~QmlValueFlowGraph(){
    if ( m_waitOn ){
        m_waitOn->clearInternalCallbacks();
        Shared::unref(m_waitOn);
        m_waitOn = nullptr;
    }
    delete m_graphSource;
    delete m_graph;
}

void QmlValueFlowGraph::setReturns(QJSValue returns){
    if ( !m_isComponentComplete ){
        if ( returns.isString() ){
            m_result = ViewEngine::typeDefaultValue(returns.toString(), ViewEngine::grab(this));
        }
    }

    m_returns = returns;
    emit returnsChanged();
}

void QmlValueFlowGraph::componentComplete(){
    m_isComponentComplete = true;

    try{
        createGraphFromSource();
        if ( !m_input.isUndefined() ){
            exec();
        }
    } catch ( lv::Exception& e ){
        ViewEngine* ve = ViewEngine::grab(this);
        QmlError( ve, e, this).jsThrow();
    }
}

QJSValue QmlValueFlowGraph::runFunction(){
    if ( m_run.isUndefined() ){
        ViewEngine* ve = ViewEngine::grab(this);
        QJSValue v = ve->engine()->evaluate("(function(ob){ return function(input){ return this.evaluate(input); }.bind(ob); })");
        QJSValue thisValue = ve->engine()->newQObject(this);
        m_run = v.call(QJSValueList() << thisValue);
    }
    return m_run;
}

QJSValue QmlValueFlowGraph::apply(const QJSValueList &args){
    return this->evaluate(args.isEmpty() ? QJSValue() : args.first());
}

bool QmlValueFlowGraph::event(QEvent *ev){
    QmlWorkerPool::TaskReadyEvent* tr = dynamic_cast<QmlWorkerPool::TaskReadyEvent*>(ev);
    if (!tr)
        return QObject::event(ev);

    auto ftask = static_cast<QmlValueFlowGraph::WorkerTask*>(tr->task());
    ViewEngine* ve = ViewEngine::grab(this);

    if ( ftask->isErrored() ){
        QmlError qe(ve, ftask->error(), this);
        qe.jsThrow();
        return true;
    }

    m_result = Shared::transfer(ftask->result(), ve->engine());
    emit resultChanged();

    delete m_currentTask;
    m_currentTask = nullptr;

//    if ( m_execAfterCurrent ){
//        m_execAfterCurrent = false;
//        exec();
//    }

    return true;
}

void QmlValueFlowGraph::createGraphFromSource(){
    if ( !m_isComponentComplete )
        return;
    if ( !m_source )
        return;
    if ( m_source->sourceCode().isEmpty() )
        return;

    ViewEngine* ve = ViewEngine::grab(this);

    *m_graphSource = QmlFunctionObjectCall::extractGraphSource(
        m_source->url().toLocalFile(), m_source->importSourceCode(), m_source->sourceCode()
    );

    //TODO: Create m_graph only if needed
    std::vector<QmlFunctionObjectCall*> collectedGraph = QmlFunctionObjectCall::mapSourceToObjects(ve, *m_graphSource);
    *m_graph = QmlFunctionObjectCall::resolveDependencies(collectedGraph);
}

QJSValue QmlValueFlowGraph::evaluate(const QJSValue &input){
    ViewEngine* ve = ViewEngine::grab(this);
    return QmlFunctionObjectCall::runGraph(ve, input, *m_graph);
}

void QmlValueFlowGraph::exec(){
    if ( !m_isComponentComplete )
        return;

    if ( m_worker ){
        ViewEngine* ve = ViewEngine::grab(this);

        try {
            QmlWorkerPool::TransferArguments transferArguments;
            transferArguments.values.append(Shared::transfer(m_input, transferArguments.transfers));
            QString id = qmlContext(this)->nameForObject(this);

            m_currentTask = new QmlValueFlowGraph::WorkerTask(*m_graphSource, transferArguments, this);
            m_worker->start(m_currentTask);

        } catch (lv::Exception& e) {
            QmlError(ve, e, this).jsThrow();
            return;
        }

    } else {
        QJSValue res = evaluate(m_input);

        QmlPromise* p = QmlPromise::fromValue(res);
        if ( p ){
            Shared::ref(p);
            m_waitOn = p;
            p->then(
                [this](QJSValue v){
                    Shared::unref(m_waitOn);
                    m_waitOn = nullptr;
                    m_result = v;
                    emit resultChanged();
                },
                [this](QJSValue v){
                    Shared::unref(m_waitOn);
                    m_waitOn = nullptr;
                    QmlError(ViewEngine::grab(this), v).jsThrow();
                }
            );
        } else {
            m_result = res;
            emit resultChanged();
        }
    }

}

void QmlValueFlowGraph::__sourceComponentChanged(){
    createGraphFromSource();
    exec();
}

}// namespace


