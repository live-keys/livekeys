/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#include "qmlprojectmonitor_p.h"
#include "projectqmlextension.h"
#include "plugintypesfacade.h"
#include "codeqmlhandler.h"
#include "qmllibrarydependency.h"

#include "live/viewengine.h"
#include "live/project.h"
#include "live/visuallogqt.h"

#include <QFileInfo>
#include <QQmlComponent>

namespace lv{

QmlProjectMonitor::QmlProjectMonitor(
        ProjectQmlExtension *projectHandler,
        Project *project,
        ViewEngine *engine,
        QObject *parent)
    : QObject(parent)
    , m_projectHandler(projectHandler)
    , m_project(project)
    , m_engine(engine)
    , m_projectScope(nullptr)
{
    connect(project, SIGNAL(pathChanged(QString)),      SLOT(newProject(QString)));
    connect(project, SIGNAL(directoryChanged(QString)), SLOT(directoryChanged(QString)));
    connect(project, SIGNAL(fileChanged(QString)),      SLOT(fileChanged(QString)));

    newProject(project->rootPath());
}

QmlProjectMonitor::~QmlProjectMonitor(){
//    delete m_scanner;
}

void QmlProjectMonitor::queueDocumentScan(
        const QString &path,
        const QString &content,
        CodeQmlHandler *codeHandler)
{
    m_scopeListeners.insert(codeHandler);
    emit requestDocumentScan(path, content, codeHandler);
}

void QmlProjectMonitor::removeScopeListener(CodeQmlHandler *handler){
    m_scopeListeners.remove(handler);
}

void QmlProjectMonitor::addScopeListener(CodeQmlHandler *handler){
    m_scopeListeners.insert(handler);
}

void QmlProjectMonitor::newDocumentScan(QmlLanguageScanner::DocumentTransport *dstransport){
    if ( m_scopeListeners.contains(dstransport->codeHandler) ){
        dstransport->codeHandler->newDocumentScanReady(dstransport->documentInfo);
    }
    delete dstransport;
}

void QmlProjectMonitor::newProject(const QString &){
    if ( m_projectScope ){
        disconnect(
            this, &QmlProjectMonitor::requestDocumentScan,
            m_projectScope->languageScanner(), &QmlLanguageScanner::scanDocument);
        disconnect(
            m_projectScope->languageScanner(), &QmlLanguageScanner::documentScanReady,
            this, &QmlProjectMonitor::newDocumentScan);
    }

    m_projectScope = ProjectQmlScope::create(m_project->lockedFileIO(), m_engine->engine());
    connect(
        this, &QmlProjectMonitor::requestDocumentScan,
        m_projectScope->languageScanner(), &QmlLanguageScanner::scanDocument);
    connect(
        m_projectScope->languageScanner(), &QmlLanguageScanner::documentScanReady,
        this, &QmlProjectMonitor::newDocumentScan);
}

void QmlProjectMonitor::directoryChanged(const QString &path){
    vlog_debug("editqmljs-scanmonitor", "Reseting libraries in directory: " + path);

    ProjectQmlScope::Ptr project = m_projectScope;
    project->resetLibrariesInPath(path);
}

void QmlProjectMonitor::fileChanged(const QString &path){
    QFileInfo finfo(path);
    if ( finfo.fileName() == "" || finfo.suffix() != "qml" || !finfo.fileName().at(0).isUpper() )
        return;
    QString fileDir = finfo.path();

    vlog_debug("editqmljs-scanmonitor", "Reseting library for file: " + path);
    ProjectQmlScope::Ptr project = m_projectScope;
    project->resetLibrary(fileDir);
}

}// namespace
