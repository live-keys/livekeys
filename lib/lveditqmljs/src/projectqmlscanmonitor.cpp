/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#include "projectqmlscanmonitor_p.h"
#include "projectqmlextension.h"
#include "projectqmlscanner_p.h"
#include "projectqmlscopecontainer_p.h"
#include "plugininfoextractor.h"
#include "plugintypesfacade.h"
#include "codeqmlhandler.h"
#include "live/engine.h"
#include "live/project.h"

#include <QFileInfo>
#include <QQmlComponent>

namespace lv{

ProjectQmlScanMonitor::ProjectQmlScanMonitor(
        ProjectQmlExtension *projectHandler,
        Project *project,
        Engine *engine,
        QObject *parent)
    : QObject(parent)
    , m_projectHandler(projectHandler)
    , m_scanner(new ProjectQmlScanner(engine->engine(), engine->engineMutex(), project->lockedFileIO()))
    , m_project(project)
    , m_engine(engine)
    , m_projectScope(0)
{
    connect(project, SIGNAL(pathChanged(QString)),      SLOT(newProject(QString)));
    connect(project, SIGNAL(directoryChanged(QString)), SLOT(directoryChanged(QString)));
    connect(project, SIGNAL(fileChanged(QString)),      SLOT(fileChanged(QString)));

    connect(m_scanner, SIGNAL(projectScopeReady()),       SLOT(newProjectScope()));
    connect(m_scanner, SIGNAL(documentScopeReady(ProjectQmlScanMonitor::DocumentQmlScopeTransport*)),
            SLOT(newDocumentScope(ProjectQmlScanMonitor::DocumentQmlScopeTransport*)));
    connect(m_scanner, SIGNAL(requestObjectLoad(QString)),SLOT(loadImport(QString)));

    newProject(project->rootPath());
}

ProjectQmlScanMonitor::~ProjectQmlScanMonitor(){
    delete m_scanner;
}

void ProjectQmlScanMonitor::scanNewDocumentScope(
        const QString &path,
        const QString &content,
        CodeQmlHandler *codeHandler)
{
    m_scopeListeners.insert(codeHandler);
    m_scanner->scanDocumentScope(path, content, m_projectScope.data(), codeHandler);
}


void ProjectQmlScanMonitor::queueNewDocumentScope(
        const QString &path,
        const QString &content,
        CodeQmlHandler *codeHandler)
{
    m_scanner->queueDocumentScopeScan(path, content, m_projectScope.data(), codeHandler);
}

void ProjectQmlScanMonitor::removeScopeListener(CodeQmlHandler *handler){
    m_scopeListeners.remove(handler);
}

void ProjectQmlScanMonitor::addScopeListener(CodeQmlHandler *handler){
    m_scopeListeners.insert(handler);
}

void ProjectQmlScanMonitor::newDocumentScope(DocumentQmlScopeTransport *dstransport){
    if ( m_scopeListeners.contains(dstransport->codeHandler) ){
        dstransport->codeHandler->newDocumentScopeReady(dstransport->path, dstransport->documentScope);
    }
    delete dstransport;
}

void ProjectQmlScanMonitor::newProjectScope(){
    for (auto it = m_scopeListeners.begin(); it != m_scopeListeners.end(); ++it)
        (*it)->newProjectScopeReady();
}

void ProjectQmlScanMonitor::newProject(const QString &){
    m_projectScope =  ProjectQmlScope::create(m_engine->engine());
    m_scanner->setProjectScope(m_projectScope);
}

void ProjectQmlScanMonitor::directoryChanged(const QString &path){
    vlog_debug("editqmljs-scanmonitor", "Reseting libraries in directory: " + path);

    ProjectQmlScope::Ptr project = m_projectScope;
    project->globalLibraries()->resetLibrariesInPath(path);
    project->implicitLibraries()->resetLibrariesInPath(path);
}

void ProjectQmlScanMonitor::fileChanged(const QString &path){
    QFileInfo finfo(path);
    if ( finfo.fileName() == "" || finfo.suffix() != "qml" || !finfo.fileName().at(0).isUpper() )
        return;
    QString fileDir = finfo.path();

    vlog_debug("editqmljs-scanmonitor", "Reseting library for file: " + path);
    ProjectQmlScope::Ptr project = m_projectScope;
    project->globalLibraries()->resetLibrary(fileDir);
    project->implicitLibraries()->resetLibrary(fileDir);
}

void ProjectQmlScanMonitor::loadImport(const QString &import){
    QQmlComponent component(m_engine->engine());
    QByteArray code = "import " + import.toUtf8() + "\nQtObject{}\n";

    vlog_debug("editqmljs-scanmonitor", "Importing object for plugininfo: " + code);

    component.setData(code, QUrl::fromLocalFile("loading.qml"));
    if ( component.errors().size() > 0 ){
        vlog_debug("editqmljs-scanmonitor", "Importing object error: " + component.errorString());
        m_scanner->updateLoadRequest(import, 0, true);
        return;
    }
    QObject* obj = component.create(m_engine->engine()->rootContext());
    if ( obj == 0 ){
        m_scanner->updateLoadRequest(import, 0, true);
    } else {
        m_scanner->updateLoadRequest(import, obj, false);
    }
}
PluginInfoExtractor* ProjectQmlScanMonitor::getPluginInfoExtractor(const QString &import){
    if ( !PluginTypesFacade::pluginTypesEnabled() ){
        qCritical("Plugin types not available in this build.");
        return 0;
    }

    QmlLibraryDependency parsedImport = QmlLibraryDependency::parse(import);
    if ( !parsedImport.isValid() ){
        qCritical("Invalid import: %s", qPrintable(import));
        return 0;
    }

    m_projectScope = ProjectQmlScope::create(m_engine->engine());
    m_projectHandler->scanner()->setProjectScope(m_projectScope);

    QList<QString> paths;
    m_projectScope->findQmlLibraryInImports(
        parsedImport.uri(),
        parsedImport.versionMajor(),
        parsedImport.versionMinor(),
        paths
    );

    if ( paths.size() == 0 ){
        qCritical("Failed to find import path: %s", qPrintable(import));
        return 0;
    }

    PluginInfoExtractor* extractor = new PluginInfoExtractor(m_projectHandler->scanner(), paths.first(), this);
    connect(m_projectHandler->scanner(), &ProjectQmlScanner::projectScopeReady, extractor, &PluginInfoExtractor::newProjectScope);
    return extractor;
}

}// namespace
