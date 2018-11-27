/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#include "projectqmlextension.h"
#include "live/project.h"
#include "live/projectfile.h"
#include "live/settings.h"
#include "live/viewengine.h"
#include "live/codeqmlhandler.h"
#include "live/editorsettings.h"
#include "live/qmljssettings.h"
#include "projectqmlscanner_p.h"
#include "projectqmlscanmonitor_p.h"

#include <QQmlEngine>
#include <QQmlContext>

namespace lv{

ProjectQmlExtension::ProjectQmlExtension(QObject *parent)
    : ProjectExtension(parent)
    , m_settings(new QmlJsSettings())
    , m_scanMonitor(nullptr)
{

}

ProjectQmlExtension::ProjectQmlExtension(Settings *settings, Project *project, ViewEngine* engine, QObject *parent)
    : ProjectExtension(parent)
    , m_project(project)
    , m_engine(engine)
    , m_settings(new QmlJsSettings())
    , m_scanMonitor(new ProjectQmlScanMonitor(this, project, engine))
{
    lv::EditorSettings* editorSettings = qobject_cast<lv::EditorSettings*>(settings->file("editor"));
    editorSettings->addSetting("qmljs", m_settings);
    editorSettings->syncWithFile();

    engine->setBindHook(&engineHook);
}

ProjectQmlExtension::~ProjectQmlExtension(){
}

AbstractCodeHandler *ProjectQmlExtension::createHandler(
        ProjectDocument *document,
        Project *project,
        ViewEngine *engine,
        DocumentHandler *handler)
{
    QString filePath = document->file()->path();
    if ( filePath.toLower().endsWith(".js") || filePath.toLower().endsWith(".qml") || filePath.isEmpty() ){
        CodeQmlHandler* ch = new CodeQmlHandler(engine, project, m_settings, this, handler);
        return ch;
    }
    return 0;
}

void ProjectQmlExtension::componentComplete(){
    if ( !m_scanMonitor ){
        QQmlContext* ctx = qmlEngine(this)->rootContext();
        QObject* lg = ctx->contextProperty("livecv").value<QObject*>();
        if ( !lg ){
            qWarning("Failed to find live global object.");
            return;
        }

        m_engine = static_cast<ViewEngine*>(lg->property("engine").value<QObject*>());
        if ( !m_engine ){ qWarning("Failed to find engine object."); return; }

        Settings* settings = static_cast<Settings*>(lg->property("settings").value<QObject*>());
        if ( !settings ){ qWarning("Failed to find settings object."); return; }
        m_project = static_cast<Project*>(ctx->contextProperty("project").value<QObject*>());
        if ( !m_project ){ qWarning("Failed to find project object."); return; }

        m_scanMonitor = new ProjectQmlScanMonitor(this, m_project, m_engine);

        lv::EditorSettings* editorSettings = qobject_cast<lv::EditorSettings*>(settings->file("editor"));
        editorSettings->addSetting("qmljs", m_settings);
        editorSettings->syncWithFile();

        m_engine->setBindHook(&engineHook);
    }
}

ProjectQmlScanner *ProjectQmlExtension::scanner(){ return m_scanMonitor->m_scanner; }

PluginInfoExtractor *ProjectQmlExtension::getPluginInfoExtractor(const QString &import){
    return m_scanMonitor->getPluginInfoExtractor(import);
}

void ProjectQmlExtension::engineHook(const QString &code, const QUrl &, QObject *result, QObject *document){
    ProjectDocument* doc = static_cast<ProjectDocument*>(document);
    DocumentQmlInfo::syncBindings(code, doc, result);
}

QObject *ProjectQmlExtension::createHandler(ProjectDocument *, DocumentHandler *handler){
    return new CodeQmlHandler(m_engine, m_project, m_settings, this, handler);
}

}// namespace
