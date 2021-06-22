#include "jsmodule.h"

#include <QJSValueIterator>
#include <QQmlEngine>
#include <QFile>
#include <QTextStream>
#include <QtDebug>

JsModule::JsModule(QObject *parent)
    : QObject(parent)
{
    m_engine = qobject_cast<QQmlEngine*>(parent);
}

void JsModule::setEngine(QQmlEngine *engine){
    m_engine = engine;
}

QObject* JsModule::provider(QQmlEngine *engine, QJSEngine *){
    QObject* ob = new JsModule(engine);
    QQmlEngine::setObjectOwnership(ob, QQmlEngine::CppOwnership);
    return ob;
}

QJSValue JsModule::require(const QString &path, const QJSValue &context, bool reload){
    if ( !m_engine ){
        qWarning("Engine not set in JsModule. This object is disabled.");
        return QJSValue();
    }

    auto it = m_loadedModules.find(path);
    if ( it != m_loadedModules.end() ){
        if ( !reload )
            return it.value();
    }

    QFile scriptFile(path);

    if (!scriptFile.exists()){
        m_engine->throwError("Js file doesn't exist:" + path);
        return QJSValue();
    }

    if (!scriptFile.open(QIODevice::ReadOnly)){
        m_engine->throwError("Failed to open js file:" + path);
        return QJSValue();
    }

    QTextStream stream(&scriptFile);
    QString contents = stream.readAll();
    scriptFile.close();

    // expand context

    QJSValueList argValues;
    QStringList argNames;

    if ( context.isObject() ){
        QJSValueIterator vit(context);
        while ( vit.next() ){
            argValues << vit.value();
            argNames << vit.name();
        }
    }

    QString moduleWrap = "(function(" + argNames.join(',') + "){" + contents + "})";

    QJSValue moduleFn = m_engine->evaluate(moduleWrap, path);
    if ( moduleFn.isError() ){
        m_engine->throwError(path + ":" + moduleFn.toString());
        return QJSValue();
    }

    QJSValue fnResult = moduleFn.call(argValues);
    if ( fnResult.isError() ){
        m_engine->throwError(path + "[" + fnResult.property("lineNumber").toInt() + "]:" + fnResult.toString());
        return QJSValue();
    }

    m_loadedModules[path] = context;

    return context;
}
