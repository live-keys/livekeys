#include "qmlvalueflowsink.h"
#include "qmlvalueflow.h"
#include "qmlstreamvalueflow.h"

#include "live/viewengine.h"
#include "live/exception.h"
#include "live/visuallogqt.h"
#include <QtDebug>
namespace lv{

QmlValueFlowSink::QmlValueFlowSink(QObject *parent)
    : QObject(parent)
    , m_isComponentComplete(false)
    , m_valueFlow(nullptr)
    , m_streamValueFlow(nullptr)
{
    initializeParent();
}

QmlValueFlowSink::~QmlValueFlowSink(){
}

QJSValue QmlValueFlowSink::value() const{
    if ( m_valueFlow )
        return m_valueFlow->result();
    else if ( m_streamValueFlow )
        return m_streamValueFlow->result();
    return QJSValue();
}

void QmlValueFlowSink::setValue(const QJSValue &value){
    if ( m_valueFlow ){
        m_valueFlow->setResult(value);
        emit valueChanged();
    } else if ( m_streamValueFlow ){
        m_streamValueFlow->setResult(value);
        emit valueChanged();
    }
}

void QmlValueFlowSink::componentComplete(){
    m_isComponentComplete = true;
    initializeParent();

    if ( !m_valueFlow && !m_streamValueFlow ){
        ViewEngine* ve = ViewEngine::grab(this);
        if ( !ve ){
            vlog("base").w() << "QmlValueFlowInput: Failed to capture engine. Errors will not report";
            return;
        }

        THROW_QMLERROR(Exception, "QmlValueFlowInput: Parent needs to be of ValueFlow or StreamValueFlow.", Exception::toCode("~ParentType"), this);
        return;
    }
}

void QmlValueFlowSink::initializeParent(){
    if ( m_valueFlow || m_streamValueFlow )
        return;

    m_valueFlow = qobject_cast<QmlValueFlow*>(parent());
    if ( !m_valueFlow ){
        m_streamValueFlow = qobject_cast<QmlStreamValueFlow*>(parent());
    }
}

}// namespace
