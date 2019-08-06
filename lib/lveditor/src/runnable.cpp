#include "runnable.h"
#include "runnablecontainer.h"

#include "live/exception.h"
#include "live/viewengine.h"
#include "live/project.h"
#include "live/projectfile.h"
#include "live/projectdocumentmodel.h"

#include <QQmlComponent>
#include <QQuickItem>
#include <QQmlEngine>

namespace lv{

Runnable::Runnable(ViewEngine* engine, const QString &path, const QString &name, QObject* parent)
    : QObject(parent)
    , m_name(name)
    , m_path(path)
    , m_component(nullptr)
    , m_runSpace(nullptr)
    , m_engine(engine)
    , m_appRoot(nullptr)
    , m_type(Runnable::File)
{
    connect(engine, &ViewEngine::objectAcquired,      this, &Runnable::engineObjectAcquired);
    connect(engine, &ViewEngine::objectReady,         this, &Runnable::engineObjectReady);
    connect(engine, &ViewEngine::objectCreationError, this, &Runnable::engineObjectCreationError);
}

Runnable::Runnable(ViewEngine* engine, QQmlComponent *component, const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_component(component)
    , m_runSpace(nullptr)
    , m_engine(engine)
    , m_appRoot(nullptr)
    , m_type(Runnable::Component)
{
}

Runnable::~Runnable(){
    if ( m_appRoot ){
        m_appRoot->deleteLater();
    }
}

void Runnable::run(){
    if ( !m_runSpace ){
        THROW_EXCEPTION(lv::Exception, "Attempting to trigger a runnable with a null runspace.", Exception::toCode("~runspace"));
    }

    RunnableContainer* container = qobject_cast<RunnableContainer*>(parent());
    Project* project = qobject_cast<Project*>(container->parent());

    if ( m_type == Runnable::File ){

        ProjectDocument* document = project->isOpened(m_path);
        if ( document ){
            auto documentList = project->documentModel()->listUnsavedDocuments();
            m_engine->createObjectAsync(
                document->content(),
                m_runSpace,
                document->file()->path(),
                this,
                !(documentList.size() == 1 && documentList[0] == document->file()->path())
            );
        } else {
            QFile f(m_path);
            if ( !f.open(QFile::ReadOnly) )
                THROW_EXCEPTION(Exception, "Failed to read file for running:" + m_path.toStdString(), Exception::toCode("~File"));

            QByteArray contentBytes = f.readAll();

            m_engine->createObjectAsync(
                contentBytes,
                m_runSpace,
                m_path,
                this,
                true
            );
        }
    } else if ( m_type == Runnable::Component ){
        //TODO: Empty run space, create new component
    }
}

void Runnable::engineObjectAcquired(const QUrl &, QObject *ref){
    if ( ref == this ){
        emptyRunSpace();
    }
}

void Runnable::engineObjectReady(QObject *object, const QUrl &, QObject *ref){
    if ( ref == this ){
        m_appRoot = object;
    }
}

void Runnable::engineObjectCreationError(QJSValue errors, const QUrl &, QObject *ref){
    if ( ref == this ){
        emit runError(errors);
    }
}

void Runnable::emptyRunSpace(){
    if ( m_appRoot ){
        QQuickItem* appRootItem = qobject_cast<QQuickItem*>(m_appRoot);
        if ( appRootItem ){
            appRootItem->setParentItem(nullptr);
        }
        m_appRoot->setParent(nullptr);
        m_appRoot->deleteLater();
        m_appRoot = nullptr;
    }
}

}// namespace
