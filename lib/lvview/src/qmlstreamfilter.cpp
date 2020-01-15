#include "qmlstreamfilter.h"

#include "live/visuallogqt.h"
#include "live/viewengine.h"
#include "live/applicationcontext.h"
#include "live/workerthread.h"

namespace lv{

QmlStreamFilter::QmlStreamFilter(QObject* parent)
    : QObject(parent)
    , m_pull(nullptr)
    , m_result(new QmlStream)
    , m_workerThread(nullptr)
{
}

QmlStreamFilter::~QmlStreamFilter(){
}

void QmlStreamFilter::streamHandler(QObject *that, const QJSValue &val){
    QmlStreamFilter* sf = static_cast<QmlStreamFilter*>(that);
    sf->triggerRun(val);
}

void QmlStreamFilter::setPull(QmlStream *pull){
    if (m_pull == pull)
        return;

    m_pull = pull;

    if ( m_run.isCallable() )
        m_pull->forward(this, &QmlStreamFilter::streamHandler);

    emit pullChanged();
}

void QmlStreamFilter::setRun(const QJSValue &run){
    m_run = run;

    if ( m_run.isCallable() && m_pull )
        m_pull->forward(this, &QmlStreamFilter::streamHandler);

    emit runChanged();
}

void QmlStreamFilter::triggerRun(){
    triggerRun(m_lastValue);
    m_lastValue = QJSValue();
}

void QmlStreamFilter::triggerRun(const QJSValue &arg){
    if ( !m_workerThread ){
        QJSValue r = m_run.call(QJSValueList() << arg);
        m_result->push(r);
    } else {
        if ( m_workerThread->isWorking() ){
            m_lastValue = arg;
            m_workerThread->postWork(this, QVariantList(), QList<Shared*>());
            return;
        }

        QList<Shared*> objectTransfer;

        QVariant varg = Shared::transfer(arg, objectTransfer);

        m_workerThread->postWork(this, varg, objectTransfer);
    }
}

void QmlStreamFilter::pushResult(const QVariant &v){
    m_result->push(m_run.engine()->toScriptValue(v));
}

}// namespace
