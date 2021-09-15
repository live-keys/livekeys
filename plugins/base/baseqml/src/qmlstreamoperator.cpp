#include "qmlstreamoperator.h"
#include "live/viewengine.h"
#include "live/qmlstreamprovider.h"

namespace lv{

QmlStreamOperator::QmlStreamOperator(QObject *parent)
    : QObject(parent)
    , m_input(nullptr)
    , m_output(nullptr)
    , m_isRunning(false)
    , m_wait(0)
{
}

QmlStreamOperator::~QmlStreamOperator(){
    delete m_output;
}

void QmlStreamOperator::streamHandler(QObject *that, const QJSValue &val){
    QmlStreamOperator* streamOperator = static_cast<QmlStreamOperator*>(that);
    streamOperator->onStreamValue(val);
}

void QmlStreamOperator::onStreamValue(const QJSValue &val){
    if ( m_isRunning ){
        return;
    }

    if ( m_next.isCallable() ){
        QJSValue res = m_next.call(QJSValueList() << val);
        if ( res.isError() ){
            ViewEngine* ve = ViewEngine::grab(this);
            if ( ve ){
                ve->throwError(res, this);
                return;
            }
        } else if ( res.isString() && res.toString() == "async" ){
            m_isRunning = true;

            if ( m_input->provider() )
                m_input->provider()->wait();
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

void QmlStreamOperator::writeNext(QJSValue val){
    if ( m_output ){
        m_output->push(val);
    }
}

void QmlStreamOperator::ready(){
    m_isRunning = false;
    if ( m_input->provider() )
        m_input->provider()->resume();
}

void QmlStreamOperator::setInput(QmlStream *stream){
    if (m_input == stream)
        return;

    if ( m_input ){
        m_input->unsubscribeObject(this);
        Shared::unref(m_output);
        m_output = nullptr;
    }

    m_input = stream;
    if ( m_input ){
        m_input->forward(this, &QmlStreamOperator::streamHandler);
        m_output = new QmlStream(this, this);
        Shared::ref(m_output);
    }

    emit inputChanged();
    emit outputChanged();
}

void QmlStreamOperator::setNext(QJSValue next){
    m_next = next;
    emit nextChanged();
}

}// namespace
