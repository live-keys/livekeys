/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#include "runnable.h"
#include "runnablecontainer.h"

#include "live/hookcontainer.h"
#include "live/exception.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/project.h"
#include "live/projectfile.h"
#include "live/projectdocumentmodel.h"
#include "live/qmlbuild.h"
#include "live/qmlprogram.h"
#include "live/visuallogqt.h"
#include "live/programholder.h"

#include <QQmlProperty>
#include <QQmlListReference>

#include <QFileInfo>
#include <QQmlComponent>
#include <QQuickItem>
#include <QQmlContext>
#include <QQmlEngine>
#include <QTimer>

#ifdef BUILD_ELEMENTS
#include"live/elements/engine.h"
#endif

namespace lv{

Runnable::Runnable(ViewEngine* viwEngine,
        el::Engine* e,
        const QString &path,
        RunnableContainer* parent,
        const QString &name)
    : QObject(parent)
    , m_name(name)
    , m_path(path)
    , m_program(nullptr)
    , m_component(nullptr)
    , m_runSpace(nullptr)
    , m_viewEngine(viwEngine)
    , m_viewRoot(nullptr)
    , m_viewContext(nullptr)
    , m_type(Runnable::QmlFile)
    , m_scheduleTimer(nullptr)
    , m_runTrigger(Document::RunOnChange)
    , m_engine(e)
{
    QString ext = QFileInfo(path).suffix();
    if ( ext == "lv" && m_engine )
        m_type = Runnable::LvFile;

    m_project = qobject_cast<Project*>(parent->parent());
    m_program = QmlProgram::create(m_viewEngine, m_project->dir().toStdString(), m_path.toStdString());

    initialize();
}

Runnable::Runnable(Program *program, RunnableContainer *parent)
    : QObject(parent)
    , m_name("")
    , m_path("")
    , m_program(nullptr)
    , m_component(nullptr)
    , m_runSpace(nullptr)
    , m_viewEngine(nullptr)
    , m_viewRoot(nullptr)
    , m_viewContext(nullptr)
    , m_type(Runnable::QmlFile)
    , m_scheduleTimer(nullptr)
    , m_runTrigger(Document::RunOnChange)
    , m_engine(nullptr)
{
    m_project = qobject_cast<Project*>(parent->parent());

    QmlProgram* qmlProgram = static_cast<QmlProgram*>(program);

    if ( qmlProgram ){
        if ( qmlProgram->mainPath().find(':') == std::string::npos ){
            QFileInfo mainPath(QString::fromStdString(qmlProgram->mainPath().data()));
            m_path = mainPath.absoluteFilePath();
            m_name = mainPath.fileName();
        } else {
            m_path = QString::fromStdString(qmlProgram->mainPath().data());
            m_name = QString::fromStdString(qmlProgram->mainPath().data());
        }
        m_viewEngine = qmlProgram->viewEngine();
        m_program = qmlProgram;

        qmlProgram->onExit([](int code){
            vlog("program").i() << "Program exited with code: " << code;
        });

        initialize();
    }
}

Runnable::Runnable(ViewEngine* engine, QQmlComponent *component, RunnableContainer *parent, const QString &name)
    : QObject(parent)
    , m_name(name)
    , m_component(component)
    , m_runSpace(nullptr)
    , m_viewEngine(engine)
    , m_viewRoot(nullptr)
    , m_type(Runnable::QmlComponent)
    , m_scheduleTimer(nullptr)
    , m_runTrigger(Document::RunOnChange)
{
    m_project = qobject_cast<Project*>(parent->parent());
    m_program = QmlProgram::create(m_viewEngine, m_project->dir().toStdString(), m_path.toStdString());
    initialize();
}

Runnable::~Runnable(){
    if ( m_viewRoot ){
        disconnect(m_viewRoot, &QObject::destroyed, this, &Runnable::clearRoot);

        m_viewRoot->setParent(nullptr);
        auto item = qobject_cast<QQuickItem*>(m_viewRoot);
        if (item)
            item->setParentItem(nullptr);
        m_viewRoot->deleteLater();
    }

    m_program->setFileReader(nullptr);
    if ( m_project->m_programHolder->main() != m_program ){
        delete m_program;
    }
}

QUrl Runnable::url() const{
    return QUrl::fromLocalFile(m_path);
}

void Runnable::run(){
    if ( m_type == Runnable::QmlFile ){

        ProjectDocument* document = ProjectDocument::castFrom(m_project->isOpened(m_path));
        if ( document ){
            auto documentList = m_project->documentModel()->listUnsavedDocuments();
            if ( !(documentList.size() == 1 && documentList[0] == document->path()) ){
                m_program->clearCache();
            }
        }

        m_program->run(nullptr, m_project->active() == this);

    } else if ( m_type == Runnable::QmlComponent ){

        QObject* obj = qobject_cast<QObject*>(m_component->create(m_component->creationContext()));
        if (!obj){
            THROW_EXCEPTION(Exception, "Failed to create item: " + m_component->errorString().toStdString(), Exception::toCode("~Component") );
        }

        emptyRunSpace();

        m_viewRoot = obj;
        connect(m_viewRoot, &QObject::destroyed, this, &Runnable::clearRoot);

        obj->setParent(m_runSpace);

        QQuickItem* appRootItem = qobject_cast<QQuickItem*>(obj);
        QQuickItem* runspaceItem = qobject_cast<QQuickItem*>(m_runSpace);

        if ( appRootItem && runspaceItem ){
            appRootItem->setParentItem(runspaceItem);
        }

        emit objectReady(obj);
    } else if ( m_type == Runnable::LvFile ){
        ProjectDocument* document = ProjectDocument::castFrom(m_project->isOpened(m_path));

        std::string content;

        if ( document ){
            content = document->content().toStdString();
        } else {
            content = m_viewEngine->fileIO()->readFromFile(m_path.toStdString());
        }

#ifdef BUILD_ELEMENTS
        try{
//            el::Object o = m_engine->loadFile(m_path.toStdString(), content);
//            std::string componentName = QFileInfo(m_path).baseName().toStdString();
//            el::Object::Accessor lo(o);
//            el::ScopedValue lval = lo.get(m_engine, componentName);

//            el::Element* e = lval.toElement(m_engine);
//            m_runtimeRoot = e;
        } catch ( lv::Exception& e ){
            vlog("main").e() << e.message();
        }
#endif
    }
}

void Runnable::__activationContentsChanged(int, int, int){
    m_scheduleTimer->start();
}

void Runnable::setRunTrigger(int runTrigger){
    if (m_runTrigger == runTrigger)
        return;

    m_runTrigger = runTrigger;
    emit runTriggerChanged();

    for ( auto it = m_project->documentModel()->openedFiles().begin(); it != m_project->documentModel()->openedFiles().end(); ++it ){
        Document* doc = it.value();
        if ( doc->runTrigger() && doc->runTrigger()->triggerRunnable() == this ){
            if ( doc->runTrigger()->triggerType() != m_runTrigger ){
                doc->runTrigger()->setTriggerType(static_cast<Document::RunTriggerType>(m_runTrigger));
                doc->notifyRunTriggerChanged();
            }
        }
    }

}

void Runnable::clearRoot(){
    if (m_viewRoot != sender()) return;
    m_viewRoot = nullptr;
    m_viewContext->deleteLater();
    m_viewContext = nullptr;
}

void Runnable::initialize(){
    connect(m_program, &QmlProgram::objectAcquired,      this, &Runnable::__objectAcquired);
    connect(m_program, &QmlProgram::objectReady,         this, &Runnable::__objectReady);
    connect(m_program, &QmlProgram::objectCreationError, this, &Runnable::__objectCreationError);

    m_program->setContextProvider([this](QObject*){
        QQmlContext* ctx = createContext();
        QmlBuild* build = static_cast<QmlBuild*>(ctx->contextProperty("build").value<QObject*>());
        RunnableContainer* runnableContainer = static_cast<RunnableContainer*>(parent());
        runnableContainer->announceQmlBuild(this, build);
        build->setState(QmlBuild::Compiling);
        return ctx;
    });

    m_program->setFileReader([this](QUrl url, QObject*){
        ProjectDocument* document = ProjectDocument::castFrom(m_project->isOpened(m_path));
        if ( document ){
            return document->content();
        } else {
            std::string ctx = m_viewEngine->fileIO()->readFromFile(url.toLocalFile().toStdString());
            return QByteArray::fromStdString(ctx);
        }
    });
}

void Runnable::runLv(){
    //TODO
}

QObject* Runnable::createObject(const QByteArray &code, const QUrl &file, QQmlContext *context){
    m_viewEngine->engine()->clearComponentCache();

    QQmlComponent component(m_viewEngine->engine());
    component.setData(code, file);

    QList<QQmlError> errors = component.errors();
    if ( errors.size() ){
        emit runError(m_viewEngine->toJSErrors(errors));
        return nullptr;
    }

    QObject* obj = component.create(context);
    m_viewEngine->engine()->setObjectOwnership(obj, QQmlEngine::JavaScriptOwnership);

    errors = component.errors();
    if ( errors.size() ){
        emit runError(m_viewEngine->toJSErrors(errors));
        return nullptr;
    }

    return obj;
}


QQmlContext *Runnable::createContext(){
    QQmlContext* ctx = new QQmlContext(m_viewEngine->engine()->rootContext());

    HookContainer* hooks = new HookContainer(m_project->dir(), this, ctx);
    QmlBuild* build      = new QmlBuild(this, ctx);
    ctx->setContextProperty("hooks", hooks);
    ctx->setContextProperty("build", build);
    return ctx;
}

void Runnable::swapViewRoot(QObject *newViewRoot){
    newViewRoot->setParent(m_runSpace);
    QQuickItem* newRootItem = qobject_cast<QQuickItem*>(newViewRoot);
    QQuickItem* runSpaceItem = qobject_cast<QQuickItem*>(m_runSpace);
    if ( newRootItem && runSpaceItem ){
        newRootItem->setParentItem(runSpaceItem);
    }

    if (m_viewRoot)
        disconnect(m_viewRoot, &QObject::destroyed, this, &Runnable::clearRoot);
    m_viewRoot = newViewRoot;

    QQuickItem* rootItem = qobject_cast<QQuickItem*>(m_runSpace);
    if ( rootItem ){
        QQmlProperty pp(rootItem);
        QQmlListReference ppref = qvariant_cast<QQmlListReference>(pp.read());
        if ( ppref.canAppend() ){
            ppref.append(newViewRoot);
        }
    }

    connect(m_viewRoot, &QObject::destroyed, this, &Runnable::clearRoot);
}

void Runnable::__objectAcquired(){
    emptyRunSpace();
}

void Runnable::__objectReady(QObject *object, QQmlContext* context){
    if (m_viewRoot)
        disconnect(m_viewRoot, &QObject::destroyed, this, &Runnable::clearRoot);

    m_viewRoot    = object;

    QQuickItem* rootItem = qobject_cast<QQuickItem*>(m_runSpace);
    if ( rootItem ){
        QQmlProperty pp(rootItem);
        QQmlListReference ppref = qvariant_cast<QQmlListReference>(pp.read());
        if ( ppref.canAppend() ){
            ppref.append(object);
        }
    }

    m_viewContext = context;

    QmlBuild* build = static_cast<QmlBuild*>(context->contextProperty("build").value<QObject*>());
    build->setState(QmlBuild::Ready);

    connect(m_viewRoot, &QObject::destroyed, this, &Runnable::clearRoot);
    emit objectReady(object);
}

void Runnable::__objectCreationError(QJSValue errors, QQmlContext *context){
    delete context;
    emit runError(errors);
}

void Runnable::emptyRunSpace(){
    if ( m_viewRoot ){
        QQuickItem* appRootItem = qobject_cast<QQuickItem*>(m_viewRoot);
        if ( appRootItem ){
            appRootItem->setParentItem(nullptr);
        }
        disconnect(m_viewRoot, &QObject::destroyed, this, &Runnable::clearRoot);
        m_viewRoot->setParent(nullptr);
        m_viewRoot->deleteLater();
        m_viewRoot = nullptr;
        m_viewContext->deleteLater();
        m_viewContext = nullptr;
    }
}

void Runnable::setRunSpace(QObject *runspace){
    if ( m_runSpace == runspace )
        return;

    if ( m_runSpace ){
        emptyRunSpace();
    }

    m_runSpace = runspace;
    m_program->setRunSpace(runspace);
}

Program *Runnable::program() const{
    return m_program;
}

void Runnable::scheduleRun(){
    if ( !m_scheduleTimer ){
        m_scheduleTimer = new QTimer(this);
        m_scheduleTimer->setInterval(1000);
        m_scheduleTimer->setSingleShot(true);
        connect(m_scheduleTimer, &QTimer::timeout, this, &Runnable::run);
    }
    m_scheduleTimer->start();
}

}// namespace
