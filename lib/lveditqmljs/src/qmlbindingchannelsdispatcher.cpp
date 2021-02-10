#include "qmlbindingchannelsdispatcher.h"
#include "documentqmlchannels.h"

#include "codeqmlhandler.h"
#include "live/project.h"
#include "live/projectfile.h"
#include "live/projectdocument.h"
#include "live/runnable.h"
#include "live/hookcontainer.h"
#include "live/runnablecontainer.h"
#include "live/qmlbuild.h"

#include "qmlwatcher.h"
#include "qmlbuilder.h"

#include <QQmlContext>

namespace lv{

QmlBindingChannelsDispatcher::QmlBindingChannelsDispatcher(Project *project, QObject *parent)
    : QObject(parent)
    , m_project(project)
{
    connect(m_project->runnables(), &RunnableContainer::qmlBuild, this, &QmlBindingChannelsDispatcher::__newQmlBuild);
}

QmlBindingChannelsDispatcher::~QmlBindingChannelsDispatcher(){

}

void QmlBindingChannelsDispatcher::__newQmlBuild(Runnable *, QmlBuild *build){
    connect(build, &QmlBuild::ready, this, &QmlBindingChannelsDispatcher::__qmlBuildReady);
}

void QmlBindingChannelsDispatcher::__qmlBuildReady(){
    QmlBuild* build = qobject_cast<QmlBuild*>(sender());
    Runnable* run = build->runnable();

    HookContainer* hooks = qobject_cast<HookContainer*>(
        run->viewContext()->contextProperty("hooks").value<QObject*>()
    );
    if ( !hooks )
        THROW_EXCEPTION(Exception, "Hooks object not found in build", Exception::toCode("~Hooks"));

    for ( auto it = m_channeledDocuments.begin(); it != m_channeledDocuments.end(); ++it ){
        DocumentQmlChannels* documentChannels = *it;
        QString filePath = documentChannels->document()->file()->path();

        documentChannels->removeChannels();
        if ( filePath == run->path() ){
            documentChannels->updateChannelForRunnable(run);
        }

        QMap<QString, QList<QObject*> > entries = hooks->entriesForFile(filePath);
        for ( auto entryIt = entries.begin(); entryIt != entries.end(); ++entryIt ){
            QString objectId = entryIt.key();
            QObject* first = entryIt.value().size() ? entryIt.value().first() : nullptr;
            if ( qobject_cast<QmlWatcher*>(first) ){

                QmlWatcher* w = qobject_cast<QmlWatcher*>(first);

                QmlBindingPath::Ptr watcherBp = QmlBindingPath::create();
                watcherBp->appendFile(run->path());
                watcherBp->appendWatcher(w->referencedFile(), objectId);
                QmlBindingChannel::Ptr bpChannel = DocumentQmlChannels::traverseBindingPath(watcherBp, run);
                if ( bpChannel ){
                    documentChannels->updateChannel(bpChannel);
                }

            } else if ( qobject_cast<QmlBuilder*>(first) ){

                QmlBindingPath::Ptr builderBp = QmlBindingPath::create();
                builderBp->appendFile(filePath);
                builderBp->appendComponent(objectId, objectId);
                QmlBindingChannel::Ptr builderC = QmlBindingChannel::create(builderBp, run);
                if ( builderC ){
                    builderC->setIsBuilder(true);
                    builderC->setEnabled(true);
                    documentChannels->updateChannel(builderC);
                }
            }
        }

    }

    //HERE: Add connection to hook container
    // Define scenario first
}

void QmlBindingChannelsDispatcher::removeDocumentChannels(DocumentQmlChannels *documentChannels){
    if ( documentChannels )
        m_channeledDocuments.remove(documentChannels);
}

void QmlBindingChannelsDispatcher::initialize(CodeQmlHandler *codeHandler, DocumentQmlChannels *documentChannels){
    m_channeledDocuments.insert(documentChannels);

    Runnable* r = m_project->runnables()->runnableAt(codeHandler->document()->file()->path());
    if (r && r->type() != Runnable::LvFile ){
        QmlBindingPath::Ptr bp = QmlBindingPath::create();
        bp->appendFile(r->path());
        QmlBindingChannel::Ptr bc = DocumentQmlChannels::traverseBindingPath(bp, r);
        if ( bc )
            documentChannels->addChannel(bc);
    }


    QString fileName = codeHandler->document()->file()->name();
    QString filePath = codeHandler->document()->file()->path();
    if ( fileName.length() && fileName.front().isUpper() ){ // if component
        RunnableContainer* rc = m_project->runnables();
        int totalRunnables = rc->rowCount();
        for ( int i = 0; i < totalRunnables; ++i ){
            Runnable* run = rc->runnableAt(i);
            if ( run == r )
                continue;

            if ( !run->name().isEmpty() && run->type() == Runnable::QmlFile && run->viewContext() ){
                HookContainer* hooks = qobject_cast<HookContainer*>(
                    run->viewContext()->contextProperty("hooks").value<QObject*>()
                );

                if ( hooks ){
                    QMap<QString, QList<QObject*> > entries = hooks->entriesForFile(filePath);
                    for ( auto entryIt = entries.begin(); entryIt != entries.end(); ++entryIt ){
                        QString objectId = entryIt.key();
                        QObject* first = entryIt.value().size() ? entryIt.value().first() : nullptr;
                        if ( qobject_cast<QmlWatcher*>(first) ){
                            QmlWatcher* w = qobject_cast<QmlWatcher*>(first);

                            QmlBindingPath::Ptr watcherBp = QmlBindingPath::create();
                            watcherBp->appendFile(run->path());
                            watcherBp->appendWatcher(w->referencedFile(), objectId);
                            QmlBindingChannel::Ptr bpChannel = DocumentQmlChannels::traverseBindingPath(watcherBp, run);
                            if ( bpChannel ){
                                documentChannels->addChannel(bpChannel);
                            }

                        } else if ( qobject_cast<QmlBuilder*>(first) ){
                            QmlBindingPath::Ptr builderBp = QmlBindingPath::create();
                            builderBp->appendFile(filePath);
                            builderBp->appendComponent(objectId, objectId);
                            QmlBindingChannel::Ptr builderC = QmlBindingChannel::create(builderBp, run);
                            if ( builderC ){
                                builderC->setIsBuilder(true);
                                builderC->setEnabled(true);
                                documentChannels->updateChannel(builderC);
                            }
                        }

                    }
                }
            }
        }
    }
}


}// namespace
