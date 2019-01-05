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
#include "live/editorglobalobject.h"
#include "live/qmljssettings.h"
#include "qmlcursorinfo.h"
#include "qmlcodeconverter.h"
#include "projectqmlscanner_p.h"
#include "projectqmlscanmonitor_p.h"

#include <QQmlEngine>
#include <QQmlContext>

namespace lv{

ProjectQmlExtension::ProjectQmlExtension(QObject *parent)
    : QObject(parent)
    , m_settings(new QmlJsSettings())
    , m_scanMonitor(nullptr)
    , m_paletteContainer(nullptr)
{
}

ProjectQmlExtension::ProjectQmlExtension(Settings *settings, Project *project, ViewEngine* engine, QObject *parent)
    : QObject(parent)
    , m_project(project)
    , m_engine(engine)
    , m_settings(new QmlJsSettings())
    , m_scanMonitor(new ProjectQmlScanMonitor(this, project, engine))
    , m_paletteContainer(nullptr)
{
    lv::EditorSettings* editorSettings = qobject_cast<lv::EditorSettings*>(settings->file("editor"));
    editorSettings->addSetting("qmljs", m_settings);
    editorSettings->syncWithFile();

    engine->addCompileHook(&ProjectQmlExtension::engineHook, this);
}

ProjectQmlExtension::~ProjectQmlExtension(){
    m_engine->removeCompileHook(&ProjectQmlExtension::engineHook, this);
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

        m_engine->addCompileHook(&ProjectQmlExtension::engineHook, this);

        EditorGlobalObject* editor = static_cast<EditorGlobalObject*>(ctx->contextProperty("editor").value<QObject*>());
        if ( !editor ){
            qWarning("Failed to find editor global object.");
            return;
        }

        m_paletteContainer = editor->paletteContainer();
    }
}

ProjectQmlScanner *ProjectQmlExtension::scanner(){
    return m_scanMonitor->m_scanner;
}

PluginInfoExtractor *ProjectQmlExtension::getPluginInfoExtractor(const QString &import){
    return m_scanMonitor->getPluginInfoExtractor(import);
}

void ProjectQmlExtension::engineHook(const QString &, const QUrl &, QObject *result, QObject *, void* data){
    ProjectQmlExtension* that = reinterpret_cast<ProjectQmlExtension*>(data);

    for ( auto it = that->m_codeHandlers.begin(); it != that->m_codeHandlers.end(); ++it ){
        CodeQmlHandler* h = *it;
        h->updateRuntimeBindings(result);
    }
}

void ProjectQmlExtension::addCodeQmlHandler(CodeQmlHandler *handler){
    m_codeHandlers.append(handler);
}

void ProjectQmlExtension::removeCodeQmlHandler(CodeQmlHandler *handler){
    m_codeHandlers.removeAll(handler);
}

void ProjectQmlExtension::registerTypes(const char *uri){
    qmlRegisterType<lv::ProjectQmlExtension>(uri, 1, 0, "ProjectQmlExtension");
    qmlRegisterType<lv::QmlCursorInfo>(      uri, 1, 0, "QmlCursorInfo");

    qmlRegisterUncreatableType<lv::CodeQmlHandler>(
        uri, 1, 0, "CodeQmlHandler", "CodeQmlHandler can only be accessed through the Editor.documentHandler.");
    qmlRegisterUncreatableType<lv::QmlCodeConverter>(
        uri, 1, 0, "QmlCodeConverter", "QmlCodeConverter can only be accessed through the Palette.attachment.");
    qmlRegisterUncreatableType<lv::QmlAddContainer>(
        uri, 1, 0, "QmlAddContainer", "QmlAddContainer can only be accessed through the qmledit extension.");
    qmlRegisterUncreatableType<lv::QmlPropertyModel>(
        uri, 1, 0, "QmlPropertyModel", "QmlPropertyModel can only be accessed through the qmledit extension.");
    qmlRegisterUncreatableType<lv::QmlItemModel>(
        uri, 1, 0, "QmlItemModel", "QmlItemModel can only be accessed through the qmledit extension.");
}

QObject *ProjectQmlExtension::createHandler(ProjectDocument *, DocumentHandler *handler){
    return new CodeQmlHandler(m_engine, m_project, m_settings, this, handler);
}

}// namespace
