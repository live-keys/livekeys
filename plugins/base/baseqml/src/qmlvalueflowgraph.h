#ifndef LVQMLVALUEFLOWGRAPH_H
#define LVQMLVALUEFLOWGRAPH_H

#include <QJSValue>
#include <QObject>
#include <QQmlComponent>
#include <QQmlParserStatus>

#include "live/viewengine.h"
#include "live/qmlsourcelocation.h"
#include "live/qmlcomponentsource.h"
#include "live/qmlpromise.h"
#include "live/qmlworkerpool.h"
#include "live/qmlworkerinterface.h"

#include "qmlfunctionobjectcall.h"

namespace lv{

class QmlFunctionObjectCall;
class QmlValueFlowGraph : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(lv::QmlComponentSource* source READ source  WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QJSValue input                 READ input   WRITE setInput NOTIFY inputChanged)
    Q_PROPERTY(QJSValue returns               READ returns WRITE setReturns NOTIFY returnsChanged)
    Q_PROPERTY(QJSValue result                READ result  NOTIFY resultChanged)
    Q_PROPERTY(lv::QmlWorkerInterface* worker READ worker  WRITE setWorker NOTIFY workerChanged)
    Q_CLASSINFO("DefaultProperty", "source");

public:
    class WorkerTask : public QmlWorkerPool::Task{

    public:
        WorkerTask(
            const QmlFunctionObjectCall::GraphSource& source,
            const QmlWorkerPool::TransferArguments& arguments = QmlWorkerPool::TransferArguments(),
            QObject* resultListener = nullptr);
        ~WorkerTask() override;
        void run(QmlWorkerPool::Thread* thread) override;

        const QVariant& result() const;

        bool isErrored() const;
        Exception error() const;

    private:
        QmlFunctionObjectCall::GraphSource m_source;
        QVariant   m_result;
        Exception* m_error;
        QObject*   m_resultListener;

        QmlWorkerPool::TransferArguments m_arguments;
    };


public:
    explicit QmlValueFlowGraph(QObject *parent = nullptr);
    ~QmlValueFlowGraph();

    lv::QmlComponentSource* source() const;
    void setSource(lv::QmlComponentSource* source);

    const QJSValue& input() const;
    void setInput(const QJSValue& input);

    const QJSValue& result() const;

    QJSValue returns() const;
    void setReturns(QJSValue returns);

    QJSValue runFunction();

    QJSValue apply(const QJSValueList& args);

    lv::QmlWorkerInterface* worker() const;
    void setWorker(lv::QmlWorkerInterface* worker);

    bool event(QEvent *event) override;

public slots:
    QJSValue evaluate(const QJSValue& input);
    void exec();

    void __sourceComponentChanged();

protected:
    virtual void classBegin() override {}
    virtual void componentComplete () override;

signals:
    void sourceChanged();
    void inputChanged();
    void resultChanged();
    void returnsChanged();

    void workerChanged(lv::QmlWorkerInterface* worker);

private:
    void createGraphFromSource();

    lv::QmlComponentSource*              m_source;
    bool                                 m_isComponentComplete;
    QJSValue                             m_input;
    QJSValue                             m_run;
    QmlFunctionObjectCall::GraphSource*  m_graphSource;
    std::vector<QmlFunctionObjectCall*>* m_graph;
    QJSValue                             m_result;
    QJSValue                             m_returns;
    QmlPromise*                          m_waitOn;
    QmlWorkerInterface*                  m_worker;
    WorkerTask*                          m_currentTask;
};

inline lv::QmlComponentSource *QmlValueFlowGraph::source() const{
    return m_source;
}

inline void QmlValueFlowGraph::setSource(lv::QmlComponentSource *source){
    if (m_source == source)
        return;

    if ( m_source ) {
         if ( m_source->parent() == this )
            m_source->setParent(nullptr);
         disconnect(m_source, &QmlComponentSource::componentChanged,
                    this, &QmlValueFlowGraph::__sourceComponentChanged);
    }

    m_source = source;
    emit sourceChanged();

    if ( m_source ){
        m_source->setParent(this);
        connect(m_source, &QmlComponentSource::componentChanged,
                   this, &QmlValueFlowGraph::__sourceComponentChanged);
    }

    createGraphFromSource();
    exec();
}

inline const QJSValue &QmlValueFlowGraph::input() const{
    return m_input;
}

inline void QmlValueFlowGraph::setInput(const QJSValue &input){
    m_input = input;
    emit inputChanged();

    exec();
}

inline const QJSValue &QmlValueFlowGraph::result() const{
    return m_result;
}

inline QJSValue QmlValueFlowGraph::returns() const{
    return m_returns;
}

inline QmlWorkerInterface *QmlValueFlowGraph::worker() const{
    return m_worker;
}

inline void QmlValueFlowGraph::setWorker(QmlWorkerInterface *worker){
    if (m_worker == worker)
        return;

    m_worker = worker;
    emit workerChanged(m_worker);
}

}// namespace

#endif // QMLVALUEFLOWGRAPH_H
