#include "qmlstreamoperator.h"
#include "live/viewengine.h"
#include "live/qmlstreamprovider.h"
#include "live/qmlact.h"

namespace lv{

QmlStreamOperator::QmlStreamOperator(ViewEngine *ve, OperationType ot, QmlStream *input, const QJSValue &options, const QJSValue &step, QObject *parent)
    : QObject(parent)
    , m_engine(ve)
    , m_operationType(ot)
    , m_input(input)
    , m_output(new QmlStream(this, nullptr))
    , m_step(step)
    , m_wait(0)
    , m_isRunning(false)
{
    Shared::ref(m_output);

    if ( m_input ){
        if ( m_operationType == QmlStreamOperator::StreamFunction ){
            m_args = QJSValueList() << QJSValue() << ve->engine()->newQObject(this) << options;
            m_input->forward(this, &QmlStreamOperator::streamFunctionHandler);
            Shared::ref(m_input);
        } else if ( m_operationType == QmlStreamOperator::ValueFunction ){
            m_args = QJSValueList() << QJSValue() << options;
            m_input->forward(this, &QmlStreamOperator::valueFunctionHandler);
            Shared::ref(m_input);
        } else if ( m_operationType == QmlStreamOperator::Act ){
            m_args = QJSValueList() << QJSValue();
            m_input->forward(this, &QmlStreamOperator::actHandler);
            Shared::ref(m_input);
        }
    }
}

QmlStreamOperator::~QmlStreamOperator(){
    Shared::unref(m_output);
    if ( m_input )
        Shared::unref(m_input);
}

void QmlStreamOperator::streamFunctionHandler(QObject *that, const QJSValue &val){
    QmlStreamOperator* streamOperator = static_cast<QmlStreamOperator*>(that);
    streamOperator->applyStreamFunction(val);
}

void QmlStreamOperator::applyStreamFunction(const QJSValue &val){
    if ( m_isRunning ){
        return;
    }

    if ( m_step.isCallable() ){
        m_args[0] = val;
        QJSValue res = m_step.call(m_args);
        if ( res.isError() ){
            ViewEngine* ve = ViewEngine::grab(this);
            if ( ve ){
                ve->throwError(res, this);
                return;
            }
        //TODO: This will switch to promises
        } else if ( res.isString() && res.toString() == "async" ){
            m_isRunning = true;

            if ( m_input->provider() )
                m_input->provider()->wait();
        }
    }
}

void QmlStreamOperator::valueFunctionHandler(QObject *that, const QJSValue &val){
    QmlStreamOperator* streamOperator = static_cast<QmlStreamOperator*>(that);
    streamOperator->applyValueFunction(val);
}

void QmlStreamOperator::applyValueFunction(const QJSValue &val){
    if ( m_isRunning ){
        return;
    }

    if ( m_step.isCallable() ){
        m_args[0] = val;
        QJSValue res = m_step.call(m_args);
        if ( res.isError() ){
            m_engine->throwError(res, this);
            return;
        //TODO: This will switch to promises
        } else if ( res.isString() && res.toString() == "async" ){
            m_isRunning = true;

            if ( m_input->provider() )
                m_input->provider()->wait();
        } else {
            push(res);
        }
    }
}

void QmlStreamOperator::actHandler(QObject *that, const QJSValue &val){
    QmlStreamOperator* streamOperator = static_cast<QmlStreamOperator*>(that);
    streamOperator->applyAct(val);
}

void QmlStreamOperator::applyAct(const QJSValue &val){
    if ( m_isRunning ){
        return;
    }

    if ( m_step.isQObject() ){
        QmlAct* act = qobject_cast<QmlAct*>(m_step.toQObject());
        m_args[0] = val;

        QJSValue res = act->apply(m_args);
        if ( res.isError() ){
            m_engine->throwError(res, this);
            return;
//        //TODO: This will switch to promises
        } else if ( res.isString() && res.toString() == "async" ){
            m_isRunning = true;

            if ( m_input->provider() )
                m_input->provider()->wait();
        } else {
            push(res);
        }
    }
}

void QmlStreamOperator::wait(){
    ++m_wait;
    if ( m_input && m_input->provider() ){
        m_input->provider()->wait();
    }
}

void QmlStreamOperator::resume(){
    if ( m_wait > 0 ){
        --m_wait;
    }
    if ( m_wait == 0 ){
        if ( m_input && m_input->provider() ){
            m_input->provider()->resume();
        }
    }
}

void QmlStreamOperator::ready(){
    m_isRunning = false;
    if ( m_input->provider() )
        m_input->provider()->resume();
}

void QmlStreamOperator::push(const QJSValue &val){
    m_output->push(val);
}

}// namespace
