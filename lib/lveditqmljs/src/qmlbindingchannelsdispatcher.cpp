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

#include "qmlbindingchannelsdispatcher.h"
#include "documentqmlchannels.h"

#include "languageqmlhandler.h"
#include "live/project.h"
#include "live/projectfile.h"
#include "live/projectdocument.h"
#include "live/runnable.h"
#include "live/hookcontainer.h"
#include "live/runnablecontainer.h"
#include "live/qmlbuild.h"

#include "qmlcomponentsource.h"
#include "qmlwatcher.h"

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
        QString filePath = documentChannels->document()->path();

        documentChannels->removeIndirectChannels();
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

            } else if ( qobject_cast<QmlComponentSource*>(first) ){
                QmlComponentSource* cs = qobject_cast<QmlComponentSource*>(first);

                QmlBindingPath::Ptr builderBp = QmlBindingPath::create();
                builderBp->appendFile(run->path());
                builderBp->appendComponent(objectId, cs->url().toLocalFile());
                QmlBindingChannel::Ptr builderC = QmlBindingChannel::create(builderBp, run);
                if ( builderC ){
                    builderC->updateConnection(first);
                    builderC->setIsBuilder(true);
                    builderC->setEnabled(true);
                    documentChannels->updateChannel(builderC);
                }
            }
        }

        documentChannels->runnableBuildReady(run);
    }

    // For watchers that initialize after the build is ready, connect them later
    connect(hooks, &HookContainer::entryAdded, this, &QmlBindingChannelsDispatcher::__hookEntryAdded);
}

void QmlBindingChannelsDispatcher::__hookEntryAdded(Runnable *runnable, const QString &file, const QString &id, QObject *object){
    for ( auto it = m_channeledDocuments.begin(); it != m_channeledDocuments.end(); ++it ){
        DocumentQmlChannels* documentChannels = *it;
        QString filePath = documentChannels->document()->path();
        if ( filePath == file ){
            if ( qobject_cast<QmlWatcher*>(object) ){

                QmlWatcher* w = qobject_cast<QmlWatcher*>(object);

                QmlBindingPath::Ptr watcherBp = QmlBindingPath::create();
                watcherBp->appendFile(runnable->path());
                watcherBp->appendWatcher(w->referencedFile(), id);
                QmlBindingChannel::Ptr bpChannel = DocumentQmlChannels::traverseBindingPath(watcherBp, runnable);
                if ( bpChannel ){
                    documentChannels->updateChannel(bpChannel);
                }

            } else if ( qobject_cast<QmlComponentSource*>(object) ){
                QmlComponentSource* cs = qobject_cast<QmlComponentSource*>(object);

                QmlBindingPath::Ptr builderBp = QmlBindingPath::create();
                builderBp->appendFile(filePath);
                builderBp->appendComponent(id, cs->url().toLocalFile());
                QmlBindingChannel::Ptr builderC = QmlBindingChannel::create(builderBp, runnable);
                if ( builderC ){
                    builderC->updateConnection(object);
                    builderC->setIsBuilder(true);
                    builderC->setEnabled(true);
                    documentChannels->updateChannel(builderC);
                }
            }
        }
    }
}

void QmlBindingChannelsDispatcher::removeDocumentChannels(DocumentQmlChannels *documentChannels){
    if ( documentChannels )
        m_channeledDocuments.remove(documentChannels);
}

void QmlBindingChannelsDispatcher::initialize(LanguageQmlHandler *languageHandler, DocumentQmlChannels *documentChannels){
    m_channeledDocuments.insert(documentChannels);

    Runnable* r = m_project->runnables()->runnableAt(languageHandler->document()->path());
    if (r && r->type() != Runnable::LvFile ){
        QmlBindingPath::Ptr bp = QmlBindingPath::create();
        bp->appendFile(r->path());
        QmlBindingChannel::Ptr bc = DocumentQmlChannels::traverseBindingPath(bp, r);
        if ( bc )
            documentChannels->addChannel(bc);
    }


    QString fileName = languageHandler->document()->fileName();
    QString filePath = languageHandler->document()->path();
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

                        } else if ( qobject_cast<QmlComponentSource*>(first) ){
                            QmlComponentSource* cs = qobject_cast<QmlComponentSource*>(first);

                            QmlBindingPath::Ptr builderBp = QmlBindingPath::create();
                            builderBp->appendFile(filePath);
                            builderBp->appendComponent(objectId, cs->url().toLocalFile());
                            QmlBindingChannel::Ptr builderC = QmlBindingChannel::create(builderBp, run);
                            if ( builderC ){
                                builderC->updateConnection(first);
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
