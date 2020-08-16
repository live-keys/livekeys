#include "qmlact.h"
#include "workerthread.h"

#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/shared.h"
#include "live/visuallogqt.h"

#include <QMetaObject>
#include <QMetaProperty>
#include <QQmlProperty>
#include <QJSValue>
#include <QJSValueIterator>
#include <QtDebug>

namespace lv{

QmlAct::QmlAct(QObject *parent)
    : QObject(parent)
    , m_isComponentComplete(false)
    , m_workerThread(nullptr)
{
}

QmlAct::~QmlAct(){
}

void QmlAct::componentComplete(){
    m_isComponentComplete = true;

    const QMetaObject* meta = metaObject();

    // capture args
    if ( m_args.isArray() ){

        QJSValueIterator it(m_args);
        int index = 0;
        while ( it.next() ){
            if ( it.name() == "length" )
                continue;

            QJSValue v = it.value();
            if ( v.isString() ){
                QString vs = v.toString();
                if ( vs.startsWith("$") ){
                    QQmlProperty pp(this, vs.mid(1));
                    if ( pp.isValid() ){
                        m_argBindings.append(qMakePair(index, pp));
                    }
                }
            }
            m_argList.append(v);
            ++index;
        }
    }

    for ( int i = 0; i < meta->propertyCount(); i++ ){
        QMetaProperty property = meta->property(i);
        QByteArray name = property.name();
        if ( name != "objectName" && name != "result" ){
            QQmlProperty pp(this, name);
            pp.connectNotifySignal(this, SLOT(exec()));
        }
    }

    emit complete();
}

void QmlAct::setArgs(QJSValue args){
    if ( m_isComponentComplete ){
        Exception e = CREATE_EXCEPTION(lv::Exception, "ActFn: Cannot configure arguments after component is complete.", Exception::toCode("~ActFnConfig"));
        ViewContext::instance().engine()->throwError(&e, this);
        return;
    }

    m_args = args;
    emit argsChanged();
}

void QmlAct::setResult(const QVariant &result){
    QJSEngine* engine = ViewContext::instance().engine()->engine();
    if ( engine ){
        setResult(Shared::transfer(result, engine));
    }
}

void QmlAct::setResult(const QJSValue &result){
    if ( result.isError() ){
        Exception e = CREATE_EXCEPTION(lv::Exception, "Act Error: " + result.toString().toStdString(), Exception::toCode("~ActFnRun"));
        ViewContext::instance().engine()->throwError(&e, this);
        return;
    }
    m_result = result;
    emit resultChanged();
}

void QmlAct::exec(){
    if ( m_workerThread ){

        if ( m_workerThread->isWorking() ){
            m_workerThread->postWork(this, QVariantList(), QList<Shared*>());
            return;
        }

        QVariantList args;

        QList<Shared*> objectTransfer;

        for ( auto it = m_argList.begin(); it != m_argList.end(); ++it ){
            QJSValue& a = *it;
            args.append(Shared::transfer(a, objectTransfer));
        }

        for ( auto it = m_argBindings.begin(); it != m_argBindings.end(); ++it ){
            QVariant v = it->second.read();
            args[it->first] = Shared::transfer(v, objectTransfer);
        }

        m_workerThread->postWork(this, args, objectTransfer);
    } else {
        ViewEngine* ve = ViewEngine::grab(this);
        if ( !ve ){
            qWarning("Act: Failed to capture view engine for %s.", metaObject()->className());
            return;
        }
        if ( m_run.isCallable() ){
            QJSEngine* engine = ve->engine();
            QJSValueList currentArgs = m_argList;
            for ( auto it = m_argBindings.begin(); it != m_argBindings.end(); ++it ){
                currentArgs[it->first] = engine->toScriptValue(it->second.read());
            }

            QJSValue r = m_run.call(currentArgs);
            setResult(r);
        } else if ( m_run.isArray() ){
            QJSEngine* engine = ve->engine();
            QJSValueList currentArgs = m_argList;
            for ( auto it = m_argBindings.begin(); it != m_argBindings.end(); ++it ){
                currentArgs[it->first] = engine->toScriptValue(it->second.read());
            }

            QJSValue ob = m_run.property(0);
            QJSValue prop = m_run.property(1);
            QJSValue run = ob.property(prop.toString());
            QJSValue r = run.call(currentArgs);
            setResult(r);
        }
    }
}

void QmlAct::setRun(QJSValue run){
    if ( m_isComponentComplete ){
        Exception e = CREATE_EXCEPTION(lv::Exception, "ActFn: Cannot set run method after component is complete.", Exception::toCode("~ActFnConfig"));
        ViewContext::instance().engine()->throwError(&e, this);
        return;
    }
    if ( !run.isCallable() && !run.isArray() ){
        Exception e = CREATE_EXCEPTION(lv::Exception, "ActFn: Run property needs to be a function.", Exception::toCode("~Function"));
        ViewContext::instance().engine()->throwError(&e, this);
        return;
    }

    m_run = run;
    emit runChanged();
}

}// namespace
