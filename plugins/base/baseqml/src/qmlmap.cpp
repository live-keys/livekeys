#include "qmlmap.h"
#include "live/exception.h"
#include "live/visuallogqt.h"
#include "qmlvalueflow.h"
#include "qmlvalueflowgraph.h"

#include <QJSValueIterator>
#include <QQmlComponent>
#include <QQmlContext>

namespace lv{

QmlMap::QmlMap(QObject *parent)
    : QObject(parent)
    , m_isRunning(false)
    , m_isComponentComplete(false)
    , m_engine(nullptr)
    , m_flow(nullptr)
{
}

QmlMap::~QmlMap(){
}

void QmlMap::setInput(const QJSValue &input){
    if ( m_isRunning )
        return;

    m_input = input;
    emit inputChanged();

    if ( m_isComponentComplete )
        exec();
}

void QmlMap::exec(){
    if ( m_isRunning || !m_flow )
        return;
    if ( !m_input.isArray() )
        return;

    m_isRunning = true;

    m_result = evaluate(m_input);
    emit resultChanged();
}

//TODO: Unwrap promises, return single promise
QJSValue QmlMap::evaluate(const QJSValue &input){
    quint32 len = input.property("length").toUInt();
    QJSValue result = m_engine->engine()->newArray(len);

    quint32 index = 0;

    QJSValueIterator it(input);
    while ( it.hasNext() ){
        it.next();
        if ( it.name() != "length" ){
            QJSValue val = it.value();
            QJSValue res = m_flow->evaluate(val).toString();
            result.setProperty(index, res);
            ++index;
        }
    }

    return result;
}

void QmlMap::setF(QmlValueFlowGraph *flow){
    if (m_flow == flow)
        return;
    if ( m_isRunning )
        return;

    m_flow = flow;
    emit flowChanged();

    if ( m_isComponentComplete )
        exec();
}

void QmlMap::componentComplete(){
    m_engine = ViewEngine::grab(this);
    m_isComponentComplete = true;

    if ( m_input.isArray() && m_flow )
        exec();
}

}// namespace
