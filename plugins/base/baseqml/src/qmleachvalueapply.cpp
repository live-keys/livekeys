#include "qmleachvalueapply.h"
#include "live/exception.h"
#include "live/visuallogqt.h"
#include "qmlvalueflow.h"

#include <QJSValueIterator>
#include <QQmlComponent>
#include <QQmlContext>

namespace lv{

class QmlEachValueApplyPrivate{

public:
    class FlowEntry{
    public:
        QmlValueFlow* flow;
        QQmlContext*  context;
        quint32       index;
    };

    quint32 currentIndex;
    quint32 inputSize;

    bool    insideIteration;

    int queuedFlowEntry;

    QList<FlowEntry> runningFlows;
};


QmlEachValueApply::QmlEachValueApply(QObject *parent)
    : QObject(parent)
    , d_ptr(new QmlEachValueApplyPrivate)
    , m_isRunning(false)
    , m_isComponentComplete(false)
    , m_engine(nullptr)
    , m_flow(nullptr)
{
}

QmlEachValueApply::~QmlEachValueApply(){
}

void QmlEachValueApply::setInput(const QJSValue &input){
    if ( m_isRunning )
        return;

    m_input = input;
    emit inputChanged();

    if ( m_isComponentComplete )
        exec();
}

void QmlEachValueApply::exec(){
    if ( m_isRunning || !m_flow )
        return;
    Q_D(QmlEachValueApply);

    m_isRunning = true;
    d->insideIteration = true;
    d->queuedFlowEntry = -1;

    if ( m_input.isArray() ){
        d->inputSize = m_input.property("length").toUInt();
        m_newResult = m_engine->engine()->newArray(d->inputSize);
        if ( d->inputSize == 0 ){
            m_result = m_newResult;
            emit resultChanged();
            return;
        }

        d->currentIndex = 0;

        if ( d->currentIndex < d->inputSize ){
            QmlEachValueApplyPrivate::FlowEntry entry;
            entry.context = new QQmlContext(m_flow->creationContext(), this);
            entry.flow    = qobject_cast<QmlValueFlow*>(m_flow->create(entry.context));
            if ( entry.flow == nullptr ){
                qCritical("MapArray: Failed to create item: %s", qPrintable(m_flow->errorString()));
                delete entry.context;
                m_isRunning = false;
                return;
            }
            connect(entry.flow, &QmlValueFlow::resultChanged, this, &QmlEachValueApply::__flowResultReady);

            entry.index = d->currentIndex;
            d->runningFlows.append(entry);

            entry.flow->setValue(m_input.property(d->currentIndex));
            while( true ){
                if ( d->queuedFlowEntry > -1 ){
                    int queuedIndex = d->queuedFlowEntry;
                    d->queuedFlowEntry = -1;
                    d->runningFlows[queuedIndex].flow->setValue(m_input.property(d->runningFlows[queuedIndex].index));
                } else {
                    break;
                }
            }
        }
    }

    if ( d->runningFlows.isEmpty() ){
        m_result = m_newResult;
        m_newResult = QJSValue();
        emit resultChanged();

        m_isRunning = false;
    }

    d->insideIteration = false;
}

void QmlEachValueApply::__flowResultReady(){
    Q_D(QmlEachValueApply);
    QmlValueFlow* flow = qobject_cast<QmlValueFlow*>(sender());
    d->currentIndex++;

    for ( int i = 0; i < d->runningFlows.length(); ++i ){
        if ( d->runningFlows[i].flow == flow ){
            m_newResult.setProperty(d->runningFlows[i].index, flow->result());
            if ( d->currentIndex < d->inputSize){
                d->runningFlows[i].index = d->currentIndex;
                if ( d->insideIteration ){
                    d->queuedFlowEntry = i;
                    return;
                } else {
                    d->insideIteration = true;
                    while( true ){
                        if ( d->queuedFlowEntry > -1 ){
                            int queuedIndex = d->queuedFlowEntry;
                            d->queuedFlowEntry = -1;
                            d->runningFlows[queuedIndex].flow->setValue(m_input.property(d->runningFlows[queuedIndex].index));
                        } else {
                            break;
                        }
                    }
                    d->insideIteration = false;
                }
            } else {
                d->runningFlows[i].flow->deleteLater();
                d->runningFlows[i].context->deleteLater();
                d->runningFlows.removeAt(i);
                break;
            }
            break;
        }
    }

    if ( d->runningFlows.isEmpty() && !d->insideIteration ){
        m_result = m_newResult;
        m_newResult = QJSValue();
        emit resultChanged();

        m_isRunning = false;
    }
}

void QmlEachValueApply::setFlow(QQmlComponent *flow){
    if (m_flow == flow)
        return;
    if ( m_isRunning )
        return;

    m_flow = flow;
    emit flowChanged();

    if ( m_isComponentComplete )
        exec();
}

void QmlEachValueApply::componentComplete(){
    m_engine = ViewEngine::grab(this);
    m_isComponentComplete = true;

    if ( m_input.isArray() && m_flow )
        exec();
}

}// namespace
