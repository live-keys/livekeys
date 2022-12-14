#include "qmlvalueflowinput.h"
#include "qmlvalueflow.h"
#include "qmlstreamvalueflow.h"

#include "live/viewengine.h"
#include "live/exception.h"
#include "live/visuallogqt.h"

namespace lv{

QmlValueFlowInput::QmlValueFlowInput(QObject *parent)
    : QObject(parent)
    , m_isComponentComplete(false)
    , m_valueFlow(nullptr)
    , m_streamValueFlow(nullptr)
{
    initializeParent();
}

QmlValueFlowInput::~QmlValueFlowInput(){
}

QJSValue QmlValueFlowInput::value(){
    if ( !m_isComponentComplete )
        initializeVars();

    if ( m_valueFlow )
        return m_valueFlow->value();
    else if ( m_streamValueFlow )
        return m_streamValueFlow->value();
    return QJSValue();
}

void QmlValueFlowInput::classBegin(){}

void QmlValueFlowInput::componentComplete(){
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

void QmlValueFlowInput::initializeVars(){
    if ( m_valueFlow || m_streamValueFlow )
        return;
    m_valueFlow = qobject_cast<QmlValueFlow*>(parent());
    if ( !m_valueFlow )
        m_streamValueFlow = qobject_cast<QmlStreamValueFlow*>(parent());
}

void QmlValueFlowInput::initializeParent(){
    initializeVars();
    if ( m_valueFlow ){
        connect(m_valueFlow, &QmlValueFlow::valueChanged, this, &QmlValueFlowInput::valueChanged);
    } else if ( m_streamValueFlow ){
        connect(m_streamValueFlow, &QmlStreamValueFlow::valueChanged, this, &QmlValueFlowInput::valueChanged);
    }
}

}// namespace
