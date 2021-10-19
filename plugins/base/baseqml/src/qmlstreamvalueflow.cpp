#include "qmlstreamvalueflow.h"
#include "live/viewengine.h"
#include "live/visuallogqt.h"

namespace lv{

QmlStreamValueFlow::QmlStreamValueFlow(QObject *parent)
    : QObject(parent)
    , m_isComponentComplete(false)
    , m_next(nullptr)
    , m_input(nullptr)
    , m_output(nullptr)
    , m_isRunning(false)
    , m_inStreamValue(false)
    , m_wait(0)
{
}

QmlStreamValueFlow::~QmlStreamValueFlow(){
    Shared::unref(m_input);
    Shared::unref(m_output);
}

void QmlStreamValueFlow::setValueType(const QString &valueType){
    if (m_valueType == valueType)
        return;

    if ( m_isComponentComplete ){
        THROW_QMLERROR(Exception, "QmlStreamValueFlow: Cannot set valueType after component is complete.", Exception::toCode("~Assignment"), this);
        return;
    }

    m_value = ViewEngine::typeDefaultValue(valueType, ViewEngine::grab(this));

    m_valueType = valueType;
    emit valueTypeChanged();
}

void QmlStreamValueFlow::streamHandler(QObject *that, const QJSValue &val){
    QmlStreamValueFlow* svf = static_cast<QmlStreamValueFlow*>(that);
    svf->onStreamValue(val);
}

void QmlStreamValueFlow::onStreamValue(const QJSValue &val){
    if ( m_isRunning ){
        return;
    }

    if ( m_inStreamValue){ // avoid recursive signal callbacks (binding loops) and return to event loop
        m_next = new QJSValue(val);
        if ( m_input->provider() ){
            m_input->provider()->wait();
        }
        return;
    }

    m_inStreamValue = true;
    m_isRunning = true;
    if ( m_input->provider() )
        m_input->provider()->wait();

    m_value = val;
    emit valueChanged();

    while ( m_next ) { // iterate as long as new values come along during recursive calls from valueChanged()
        m_isRunning = true;
        m_value = *m_next;
        delete m_next;
        m_next = nullptr;
        emit valueChanged();
    }

    m_inStreamValue = false;
}

void QmlStreamValueFlow::setResult(const QJSValue& result){
    if ( !m_isRunning )
        return;

    m_result = result;
    emit resultChanged();

    if ( m_output )
        m_output->push(result);

    m_isRunning = false;
    if ( m_input->provider() )
        m_input->provider()->resume();
}

void QmlStreamValueFlow::setInput(QmlStream *stream){
    if (m_input == stream)
        return;

    if ( m_input ){
        m_input->unsubscribeObject(this);
        Shared::unref(m_input);

        Shared::unref(m_output);
        m_output = nullptr;
    }

    m_input = stream;
    if ( m_input ){
        m_input->forward(this, &QmlStreamValueFlow::streamHandler);
        Shared::ref(m_input);

        m_output = new QmlStream(this, this);
        Shared::ref(m_output);
    }

    emit inputChanged();
    emit outputChanged();
}

void QmlStreamValueFlow::wait(){
    ++m_wait;
    if ( m_input && m_input->provider() ){
        m_input->provider()->wait();
    }
}

void QmlStreamValueFlow::resume(){
    if ( m_wait > 0 ){
        --m_wait;
    }
    if ( m_wait == 0 ){
        if ( m_input && m_input->provider() ){
            m_input->provider()->resume();
        }
    }
}

void QmlStreamValueFlow::componentComplete(){
    m_isComponentComplete = true;
}

void QmlStreamValueFlow::appendChildObject(QObject *obj){
    m_childObjects.append(obj);
}

int QmlStreamValueFlow::childObjectCount() const{
    return m_childObjects.size();
}

QObject *QmlStreamValueFlow::childObject(int index) const{
    return m_childObjects.at(index);
}

void QmlStreamValueFlow::clearChildObjects(){
    m_childObjects.clear();
}

QQmlListProperty<QObject> QmlStreamValueFlow::childObjects(){
    return QQmlListProperty<QObject>(this, this,
             &QmlStreamValueFlow::appendChildObject,
             &QmlStreamValueFlow::childObjectCount,
             &QmlStreamValueFlow::childObject,
             &QmlStreamValueFlow::clearChildObjects);
}

void QmlStreamValueFlow::appendChildObject(QQmlListProperty<QObject> *list, QObject *o){
    reinterpret_cast<QmlStreamValueFlow*>(list->data)->appendChildObject(o);
}

int QmlStreamValueFlow::childObjectCount(QQmlListProperty<QObject> *list){
    return reinterpret_cast<QmlStreamValueFlow*>(list->data)->childObjectCount();
}

QObject *QmlStreamValueFlow::childObject(QQmlListProperty<QObject> *list, int index){
    return reinterpret_cast<QmlStreamValueFlow*>(list->data)->childObject(index);
}

void QmlStreamValueFlow::clearChildObjects(QQmlListProperty<QObject> *list){
    reinterpret_cast<QmlStreamValueFlow*>(list->data)->clearChildObjects();
}

}// namespace
