#include "qmlstreamoperations.h"
#include "qmlstreamproviderscript.h"
#include "live/viewengine.h"

namespace lv{

QmlStreamOperations::QmlStreamOperations(QObject *parent)
    : QObject(parent)
{
}

QmlStream *QmlStreamOperations::createProvider(const QJSValue &opt, const QJSValue &stepCb){
    ViewEngine* ve = ViewEngine::grabFromQmlEngine(qobject_cast<QQmlEngine*>(parent()));
    auto provider = new QmlStreamProviderScript(ve, opt, stepCb);
    QQmlEngine::setObjectOwnership(provider, QQmlEngine::CppOwnership);
    return provider->stream();
}

QmlStream *QmlStreamOperations::createOperator(QmlStream* input, const QJSValue &opt, const QJSValue &stepCb){
    ViewEngine* ve = ViewEngine::grabFromQmlEngine(qobject_cast<QQmlEngine*>(parent()));
    auto streamOperator = new QmlStreamOperator(ve, QmlStreamOperator::StreamFunction, input, opt, stepCb);
    QQmlEngine::setObjectOwnership(streamOperator, QQmlEngine::CppOwnership);
    return streamOperator->output();
}

QmlStream *QmlStreamOperations::createValueOperator(QmlStream *input, const QJSValue &opt, const QJSValue &stepCb){
    ViewEngine* ve = ViewEngine::grabFromQmlEngine(qobject_cast<QQmlEngine*>(parent()));
    auto streamOperator = new QmlStreamOperator(ve, stepCb.isQObject() ? QmlStreamOperator::Act : QmlStreamOperator::ValueFunction, input, opt, stepCb);
    QQmlEngine::setObjectOwnership(streamOperator, QQmlEngine::CppOwnership);
    return streamOperator->output();
}

}// namespace
