#include "qmlact.h"
#include "workerthread.h"

#include "live/viewengine.h"
#include "live/exception.h"
#include "live/shared.h"
#include "live/visuallogqt.h"
#include "live/viewcontext.h"
#include "live/qmlstreamprovider.h"
#include "live/qmlpromise.h"

#include <QQmlContext>
#include <QQmlEngine>
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
    , m_trigger(QmlAct::PropertyChange)
    , m_runSource(nullptr)
    , m_promiseResult(nullptr)
    , m_worker(nullptr)
    , m_currentTask(nullptr)
    , m_execAfterCurrent(false)
{
}

QmlAct::~QmlAct(){
    if ( m_promiseResult ){
        m_promiseResult->clearInternalCallbacks();
        Shared::unref(m_promiseResult);
    }
    delete m_runSource;
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
        if ( name != "objectName" &&
             name != "run" &&
             name != "args" &&
             name != "returns" &&
             name != "result" &&
             name != "trigger" &&
             name != "worker" &&
             name != "unwrap" &&
             !name.startsWith("__" ))
        {
            QQmlProperty pp(this, name);
            pp.connectNotifySignal(this, SLOT(__propertyChange()));
        }
    }

    emit complete();
}

void QmlAct::setArgs(QJSValue args){
    if ( m_isComponentComplete ){
        Exception e = CREATE_EXCEPTION(lv::Exception, "Act: Cannot configure arguments after component is complete.", Exception::toCode("~ActFnConfig"));
        QmlError(e, this).jsThrow();
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
        ViewEngine* ve = ViewEngine::grab(this);
        if ( !ve ){
            QmlError::warnNoEngineCaptured(this);
            return;
        }
        ve->throwError(result, this);
        return;
    }

    QmlStream* localStream = QmlStream::fromValue(m_result);
    if ( localStream ){
        QmlStreamProvider* sp = localStream->provider(); // This stream provider is owned by this Act
        delete sp;
    }

    QmlPromise* promise = QmlPromise::fromValue(result);
    if ( promise ){
        m_promiseResult = promise;
        Shared::ref(m_promiseResult);
        m_promiseResult->then(
            [this](QJSValue v){
                Shared::unref(m_promiseResult);
                m_promiseResult = nullptr;
                setResult(v);
                if ( m_execAfterCurrent ){
                    m_execAfterCurrent = false;
                    exec();
                }
            },
            [this](QJSValue v){
                Shared::unref(m_promiseResult);
                m_promiseResult = nullptr;
                m_execAfterCurrent = false;
                QmlError(ViewEngine::grab(this), v).jsThrow();
            }
        );
        return;
    }

    if ( m_returns.isArray() ){
        ViewEngine* engine = ViewContext::instance().engine();
        QJSValue resultWrap = engine->engine()->newObject();
        resultWrap.setProperty("result", result);

        QJSValueIterator it(m_returns);
        while ( it.hasNext() ){
            it.next();
            if ( it.name() != "length" ){
                QString propertyName = it.value().toString();
                if ( propertyName != "result" ){
                    QQmlProperty qp(this, propertyName);
                    if ( qp.isValid() ){
                        resultWrap.setProperty(propertyName, Shared::transfer(qp.read(), engine->engine()));
                    }
                }
            }
        }

        m_result = resultWrap;
        emit resultChanged();

    } else {
        m_result = result;
        emit resultChanged();
    }

    QmlStream* stream = QmlStream::fromValue(result);
    if ( stream ){
        QmlStreamProvider* sp = stream->provider();
        sp->resume();
    }
}

void QmlAct::exec(){
    if ( !m_runSource )
        return;

    if ( m_promiseResult ){
        m_execAfterCurrent = true;
        return;
    }

    if ( m_worker ){
        if ( m_currentTask ){ // is working
            m_execAfterCurrent = true;
            return;
        }

        ViewEngine* ve = ViewEngine::grab(this);

        try {
            QmlWorkerPool::TransferArguments transferArguments;

            for ( auto it = m_argList.begin(); it != m_argList.end(); ++it ){
                QJSValue& a = *it;
                transferArguments.values.append(Shared::transfer(a, transferArguments.transfers));
            }

            for ( auto it = m_argBindings.begin(); it != m_argBindings.end(); ++it ){
                QVariant v = it->second.read();
                transferArguments.values[it->first] = Shared::transfer(v, transferArguments.transfers);
            }

            QString id = qmlContext(this)->nameForObject(this);

            m_currentTask = new QmlWorkerPool::QmlFunctionTask(
                m_runSource->path(),
                id,
                m_runSource->imports(),
                m_runSource->source().toUtf8(),
                transferArguments,
                this
            );
            m_worker->start(m_currentTask);

        } catch (lv::Exception& e) {

            QmlError(ve, e, this).jsThrow();
            return;

        }
    } else {
        ViewEngine* ve = ViewEngine::grab(this);
        if ( !ve ){
            QmlError::warnNoEngineCaptured(this, "Act");
            return;
        }
        if ( m_runSource->fn().isCallable() ){
            QJSEngine* engine = ve->engine();
            QJSValueList currentArgs = m_argList;
            for ( auto it = m_argBindings.begin(); it != m_argBindings.end(); ++it ){
                currentArgs[it->first] = engine->toScriptValue(it->second.read());
            }

            QJSValue r = m_runSource->fn().call(currentArgs);
            setResult(r);
        } else if ( m_runSource->fn().isArray() ){
            QJSEngine* engine = ve->engine();
            QJSValueList currentArgs = m_argList;
            for ( auto it = m_argBindings.begin(); it != m_argBindings.end(); ++it ){
                currentArgs[it->first] = engine->toScriptValue(it->second.read());
            }

            QJSValue ob = m_runSource->fn().property(0);
            QJSValue prop = m_runSource->fn().property(1);
            QJSValue run = ob.property(prop.toString());
            QJSValue r = run.call(currentArgs);
            setResult(r);
        }
    }
}

void QmlAct::__propertyChange(){
    if ( m_trigger == QmlAct::PropertyChange ){
        exec();
    }
}

void QmlAct::setUnwrap(QJSValue unwrap){
    if ( unwrap.isObject() ){

        auto trigger = m_trigger;
        m_trigger = QmlAct::Manual;

        QJSValueIterator it(unwrap);
        while ( it.hasNext() ){
            it.next();
            QQmlProperty pp(this, it.name());
            if ( pp.isValid() && pp.isWritable() ){
                pp.write(it.value().toVariant());
            }
        }

        m_trigger = trigger;
        if ( m_trigger == QmlAct::PropertyChange ){
            exec();
        }
    }

    emit unwrapChanged();
}

QJSValue QmlAct::apply(const QJSValueList &args){
    return m_runSource->fn().call(args);
}

bool QmlAct::parserPropertyValidateHook(QmlSourceLocation, QmlSourceLocation, const QString &name){
    if ( name != "run" && !name.startsWith("on") ){
        return false;
    }
    return true;
}


QJSValue QmlAct::runFunction() const{
    if ( m_runSource )
        return m_runSource->fn();
    return QJSValue();
}

void QmlAct::parserPropertyHook(
        ViewEngine *ve,
        QObject *target,
        const QByteArray &imports,
        QmlSourceLocation,
        QmlSourceLocation valueLocation,
        const QString &name,
        const QString &source)
{
    if ( name != "run" ){
        THROW_EXCEPTION(lv::Exception, "Invalid property: " + name.toStdString() + ". Note: Signal handlers are also not supported directly on Act.", Exception::toCode("~Property"));
    }

    QmlFunctionSource* fns = QmlFunctionSource::createCompiled(ve, imports, source, valueLocation);

    QmlAct* act = qobject_cast<QmlAct*>(target);
    if ( act->m_runSource )
        delete act->m_runSource;
    act->m_runSource = fns;
}

bool QmlAct::event(QEvent *ev){
    QmlWorkerPool::TaskReadyEvent* tr = dynamic_cast<QmlWorkerPool::TaskReadyEvent*>(ev);
    if (!tr)
        return QObject::event(ev);

    auto ftask = static_cast<QmlWorkerPool::QmlFunctionTask*>(tr->task());
    ViewEngine* ve = ViewEngine::grab(this);

    if ( ftask->isErrored() ){
        QmlError qe(ve, ftask->error(), this);
        qe.jsThrow();
        return true;
    }

    setResult(Shared::transfer(ftask->result(), ve->engine()));

    delete m_currentTask;
    m_currentTask = nullptr;

    if ( m_execAfterCurrent ){
        m_execAfterCurrent = false;
        exec();
    }

    return true;
}

void QmlAct::setReturns(QJSValue returns){
    if ( !m_isComponentComplete ){
        if ( returns.isString() ){
            m_result = ViewEngine::typeDefaultValue(returns.toString(), ViewEngine::grab(this));
        }
    }

    m_returns = returns;
    emit returnsChanged();
}


void lv::QmlAct::setWorker(QmlWorkerInterface *worker){
    if ( m_isComponentComplete ){
        Exception e = CREATE_EXCEPTION(lv::Exception, "Act: Cannot set worker after component is complete.", Exception::toCode("~ActFnConfig"));
        QmlError(e, this).jsThrow();
        return;
    }
    if (m_worker == worker)
        return;

    m_worker = worker;
    emit workerChanged();
}

}// namespace
