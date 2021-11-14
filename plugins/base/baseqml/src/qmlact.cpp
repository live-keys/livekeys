#include "qmlact.h"
#include "workerthread.h"

#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/shared.h"
#include "live/visuallogqt.h"
#include "live/componentdeclaration.h"
#include "live/documentqmlinfo.h"
#include "live/viewcontext.h"
#include "live/project.h"

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
    , m_worker(nullptr)
    , m_source(nullptr)
    , m_currentTask(nullptr)
    , m_execAfterCurrent(false)
{
}

QmlAct::~QmlAct(){
    delete m_source;
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
             name != "unwrap")
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

}

void QmlAct::extractSource(ViewEngine* ve){
    if ( !m_source ){
        ComponentDeclaration cd = ve->rootDeclaration(this);
        if ( cd.id().isEmpty() ){
            THROW_EXCEPTION(Exception, "Act: Act requires id to be used in workers.", Exception::toCode("~Id"));
        }
        if ( cd.url().isEmpty() ){
            THROW_EXCEPTION(Exception, "Act: Failed ot find path for act " + cd.id().toStdString() + ".", Exception::toCode("~Id"));
        }

        m_source = new QmlAct::RunSource(cd);

        QString path = m_source->declarationLocation.url().toLocalFile();

        DocumentQmlInfo::Ptr dqi = DocumentQmlInfo::create(path);
        QString code = QString::fromStdString(ve->fileIO()->readFromFile(path.toStdString()));
        dqi->parse(code);
        dqi->createRanges();

        m_source->source = dqi->propertySourceFromObjectId(m_source->declarationLocation.id(), "run").toUtf8();
        m_source->imports = DocumentQmlInfo::Import::join(dqi->imports()).toUtf8();
    }
}

void QmlAct::exec(){
    if ( m_worker ){

        if ( m_currentTask ){ // is working
            m_execAfterCurrent = true;
            return;
        }

        ViewEngine* ve = ViewEngine::grab(this);

        try {
            extractSource(ve);

            QmlWorkerPool::TransferArguments transferArguments;

            for ( auto it = m_argList.begin(); it != m_argList.end(); ++it ){
                QJSValue& a = *it;
                transferArguments.values.append(Shared::transfer(a, transferArguments.transfers));
            }

            for ( auto it = m_argBindings.begin(); it != m_argBindings.end(); ++it ){
                QVariant v = it->second.read();
                transferArguments.values[it->first] = Shared::transfer(v, transferArguments.transfers);
            }

            m_currentTask = new QmlWorkerPool::QmlFunctionTask(
                m_source->declarationLocation.url().toLocalFile(),
                m_source->declarationLocation.id(),
                m_source->imports,
                m_source->source,
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

bool QmlAct::event(QEvent *ev){
    QmlWorkerPool::TaskReadyEvent* tr = dynamic_cast<QmlWorkerPool::TaskReadyEvent*>(ev);
    if (!tr)
        return QObject::event(ev);

    auto ftask = static_cast<QmlWorkerPool::QmlFunctionTask*>(tr->task());

    if ( ftask->isErrored() ){
        QmlError qe(ViewContext::instance().engine(), ftask->error(), this);
        qe.jsThrow();
        return true;
    }

    ViewEngine* ve = ViewEngine::grab(this);

    setResult(Shared::transfer(ftask->result(), ve->engine()));

    delete m_currentTask;
    m_currentTask = nullptr;

    if ( m_execAfterCurrent ){
        m_execAfterCurrent = false;
        exec();
    }

    return true;
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
        Exception e = CREATE_EXCEPTION(lv::Exception, "ActFn: Cannot set run method after component is complete.", Exception::toCode("~ActFnConfig"));
        ViewContext::instance().engine()->throwError(&e, this);
        return;
    }
    if (m_worker == worker)
        return;

    m_worker = worker;
    emit workerChanged();
}

}// namespace
