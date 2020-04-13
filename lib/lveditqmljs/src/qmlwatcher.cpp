#include "qmlwatcher.h"
#include "qmlwatcherbackground.h"

#include <QQuickItem>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlProperty>
#include <QQmlEngine>

#include <QThread>

#include "live/hookcontainer.h"
#include "live/project.h"
#include "live/projectdocument.h"
#include "live/viewengine.h"
#include "live/visuallogqt.h"

namespace lv{

QmlWatcher::QmlWatcher(QObject *parent)
    : QObject(parent)
    , m_scanner(nullptr)
    , m_worker(nullptr)
    , m_componentComplete(false)
{
}

QmlWatcher::~QmlWatcher(){
    if ( m_worker ){
        m_worker->exit();
        if ( !m_worker->wait(100) ){
            m_worker->terminate();
            m_worker->wait();
        }
        delete m_worker;
    }
}

QObject *QmlWatcher::scanner(){
    if ( !m_scanner ){
        QQmlEngine* engine = qmlEngine(this);
        QQmlComponent component(engine);
        component.setData(
            "import QtQuick 2.3\n"
            "QtObject{ "
            "function capture(){ try{ throw new Error() } catch (e){ return e } return null; }"
            "}",
            QUrl("file:///Watcher.qml")
        );
        m_scanner = component.create();
    }

    return m_scanner;
}

void QmlWatcher::componentComplete(){
    m_componentComplete = true;

    scanner();

    if ( m_fileLine >= 0 && !m_filePath.isEmpty() ){

        //Find document/file, get contents, create capture thread
        QQmlContext* ctx = qmlContext(this);
        if ( ctx ){
            Project* pr = qobject_cast<Project*>(ctx->contextProperty("project").value<QObject*>());
            if ( pr ){
                ProjectDocument* doc = qobject_cast<ProjectDocument*>(pr->isOpened(m_filePath));
                QString docContent;
                if ( doc ){
                    docContent = doc->contentString();
                } else {
                    docContent = QString::fromStdString(pr->lockedFileIO()->readFromFile(m_filePath.toStdString()));
                }

                m_worker = new QmlWatcherBackground(
                    m_filePath, m_fileLine, docContent, this
                );
                connect(m_worker, &QThread::finished, this, &QmlWatcher::__workerResultReady);
                m_worker->start();
            }
        }
    }

    emit ready();
}

const QJSValue& QmlWatcher::position() const{
    return m_position;
}

const QString &QmlWatcher::fileReference() const{
    return m_worker->result();
}

void QmlWatcher::setPosition(QJSValue position){
    QQmlContext* ctx = qmlContext(this);
    ViewEngine* engine = nullptr;
    if ( ctx ){
        QObject* lk = ctx->contextProperty("lk").value<QObject*>();
        if ( lk ){
            engine = qobject_cast<ViewEngine*>(lk->property("engine").value<QObject*>());
        }
    }

    if ( !position.hasProperty("stack") ){
        Exception e = CREATE_EXCEPTION(Exception, "Cannot obtain position through 'stack'.", Exception::toCode("~Stack"));
        engine->throwError(&e, this);
        return;
    }

    QString stack = position.property("stack").toString();
    QString lastStack = stack.mid(stack.lastIndexOf('\n'));
    if ( lastStack.isEmpty() || !lastStack.contains("position") ){
        Exception e = CREATE_EXCEPTION(Exception, "Cannot obtain position through 'stack'.", Exception::toCode("~Stack"));
        engine->throwError(&e, this);
        return;
    }

    QString lastStackContent = lastStack.mid(lastStack.indexOf('@') + 1);
    if ( lastStackContent.isEmpty() || !lastStackContent.contains(':') ){
        Exception e = CREATE_EXCEPTION(Exception, "Cannot obtain position through 'stack'.", Exception::toCode("~Stack"));
        engine->throwError(&e, this);
        return;
    }

    bool ok;
    m_fileLine = lastStackContent.mid(lastStackContent.lastIndexOf(':') + 1).toInt(&ok);
    QUrl file = lastStackContent.mid(0, lastStackContent.lastIndexOf(':'));

    if ( file.scheme() == "memory" || file.scheme() == "file" ){
        m_filePath = file.path();
    }

    if ( m_filePath.isEmpty() || !ok ){
        Exception e = CREATE_EXCEPTION(Exception, "Cannot obtain position through 'stack'.", Exception::toCode("~Stack"));
        engine->throwError(&e, this);
        return;
    }

    m_position = position;
    emit positionChanged();
}

void QmlWatcher::__workerResultReady(){
    if ( m_worker->result().isEmpty() )
        return;

    QQmlContext* ctx = qmlContext(this);
    QString id = ctx->nameForObject(parent());
    if ( id.isEmpty() )
        return;

    HookContainer* hk = qobject_cast<HookContainer*>(ctx->contextProperty("hooks").value<QObject*>());
    hk->insertKey(m_worker->result(), id, this);
}

}// namespace
