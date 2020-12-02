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

void QmlAct::extractSource(ViewEngine* ve){
    if ( !m_source ){
        ComponentDeclaration cd = ve->rootDeclaration(this);
        m_source = new QmlAct::RunSource(cd);

        Project* project = qobject_cast<lv::Project*>(
            lv::ViewContext::instance().engine()->engine()->rootContext()->contextProperty("project").value<QObject*>()
        );
        if ( !project ){
            lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Failed to load 'project' property from context", 0);
            lv::ViewContext::instance().engine()->throwError(&e);
            return;
        }

        QString path = m_source->declarationLocation.url().toLocalFile();

        DocumentQmlInfo::Ptr dqi = DocumentQmlInfo::create(path);
        QString code = QString::fromStdString(project->lockedFileIO()->readFromFile(path.toStdString()));
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

void QmlAct::setReturns(QString returns){
    if (m_returns == returns)
        return;

    if ( m_isComponentComplete ){
        Exception e = CREATE_EXCEPTION(lv::Exception, "ActFn: Cannot set run method after component is complete.", Exception::toCode("~ActFnConfig"));
        ViewContext::instance().engine()->throwError(&e, this);
        return;
    }

    if ( returns == "qml/object" ){
        m_result = QJSValue(QJSValue::NullValue);
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
