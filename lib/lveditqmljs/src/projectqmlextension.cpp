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

#include "projectqmlextension.h"
#include "live/project.h"
#include "live/projectfile.h"
#include "live/settings.h"
#include "live/viewengine.h"
#include "live/codeqmlhandler.h"
#include "live/editorsettings.h"
#include "live/editorglobalobject.h"
#include "qmljssettings.h"
#include "qmlcursorinfo.h"
#include "qmlcodeconverter.h"
#include "projectqmlscanner_p.h"
#include "projectqmlscanmonitor_p.h"
#include "qmlpropertymodel.h"
#include "qmlitemmodel.h"
#include "qmladdcontainer.h"
#include "qmleditfragment.h"

#include <QQmlEngine>
#include <QQmlContext>

namespace lv{

/**
 * \class lv::ProjectQmlExtension
 * \ingroup lveditqmljs
 * \brief Main object used throughout qml extension plugin.
 *
 * To capture the object, you can use the workspace layer:
 *
 * \code
 * var projectQmlExtension = lk.layers.workspace.extensions["editqml"].globals
 * \endcode
 */


/**
 * \brief ProjectQmlExtension constructor with a \p parent parameter
 */
ProjectQmlExtension::ProjectQmlExtension(QObject *parent)
    : QObject(parent)
    , m_settings(new QmlJsSettings())
    , m_scanMonitor(nullptr)
    , m_paletteContainer(nullptr)
{
}

/**
 * @brief ProjectQmlExtension destructor
 */
ProjectQmlExtension::~ProjectQmlExtension(){
    m_engine->removeCompileHook(&ProjectQmlExtension::engineHook, this);
    for ( auto it = m_codeHandlers.begin(); it != m_codeHandlers.end(); ++it ){
        CodeQmlHandler* cqh = *it;
        cqh->resetProjectQmlExtension();
    }
    m_codeHandlers.clear();
}

/**
 * @brief Override of QQmlParserStatus::classBegin
 */
void ProjectQmlExtension::classBegin(){
}


/**
 * @brief Override of QQmlParserStatus::componentComplete
 */
void ProjectQmlExtension::componentComplete(){
    if ( !m_scanMonitor ){
        QQmlContext* ctx = qmlEngine(this)->rootContext();
        QObject* lg = ctx->contextProperty("lk").value<QObject*>();
        if ( !lg ){
            qWarning("Failed to find live global object.");
            return;
        }

        ViewEngine* engine = static_cast<ViewEngine*>(lg->property("engine").value<QObject*>());
        if ( !engine ){ qWarning("Failed to find engine object."); return; }

        Settings* settings = static_cast<Settings*>(lg->property("settings").value<QObject*>());
        if ( !settings ){ qWarning("Failed to find settings object."); return; }

        Project* project = static_cast<Project*>(ctx->contextProperty("project").value<QObject*>());
        if ( !project ){ qWarning("Failed to find project object."); return; }

        setParams(settings, project, engine);
    }
}

/**
 * \brief Returns the the qml scanner associated with this object.
 */
ProjectQmlScanner *ProjectQmlExtension::scanner(){
    return m_scanMonitor->m_scanner;
}

/**
 * \brief Returns the lv::PluginInfoExtractor associated with this object.
 */
PluginInfoExtractor *ProjectQmlExtension::getPluginInfoExtractor(const QString &import){
    return m_scanMonitor->getPluginInfoExtractor(import);
}

/**
 * \brief Hook that get's executed for each engine recompile, notifying all codeHandlers assigned to this object.
 */
void ProjectQmlExtension::engineHook(const QString &, const QUrl &, QObject *result, void* data){
    ProjectQmlExtension* that = reinterpret_cast<ProjectQmlExtension*>(data);

    for ( auto it = that->m_codeHandlers.begin(); it != that->m_codeHandlers.end(); ++it ){
        CodeQmlHandler* h = *it;
        h->updateRuntimeBindings(result);
    }
}

/**
 * \brief Adds a codeHandler to this object.
 *
 * CodeHandlers are updated each time the engine recompiles the code.
 */
void ProjectQmlExtension::addCodeQmlHandler(CodeQmlHandler *handler){
    m_codeHandlers.append(handler);
}

/**
 * \brief Removes the \p handler from this object.
 *
 * Note that this does not destroy the object.
 */
void ProjectQmlExtension::removeCodeQmlHandler(CodeQmlHandler *handler){
    m_codeHandlers.removeAll(handler);
}

/**
 * \brief Registers the ProjectQmlExtension qml types to the \p uri.
 */
void ProjectQmlExtension::registerTypes(const char *uri){
    qmlRegisterType<lv::ProjectQmlExtension>(uri, 1, 0, "ProjectQmlExtension");
    qmlRegisterType<lv::QmlCursorInfo>(      uri, 1, 0, "QmlCursorInfo");

    qmlRegisterUncreatableType<lv::QmlEditFragment>(
        uri, 1, 0, "QmlEditFragment", "QmlEditFragment can be created through the Editor.documentHandler.codeQmlHandler.");
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

/**
 * \brief Assign initialization params for this object
 */
void ProjectQmlExtension::setParams(Settings *settings, Project *project, ViewEngine *engine){
    m_project = project;
    m_engine = engine;

    m_scanMonitor = new ProjectQmlScanMonitor(this, m_project, m_engine);

    lv::EditorSettings* editorSettings = qobject_cast<lv::EditorSettings*>(settings->file("editor"));
    editorSettings->addSetting("qmljs", m_settings);
    editorSettings->syncWithFile();

    m_engine->addCompileHook(&ProjectQmlExtension::engineHook, this);

    EditorGlobalObject* editor = static_cast<EditorGlobalObject*>(engine->engine()->rootContext()->contextProperty("editor").value<QObject*>());
    if ( !editor ){
        qWarning("Failed to find editor global object.");
        return;
    }

    m_paletteContainer = editor->paletteContainer();
}


/**
 * \brief Creates a CodeQmlHandler for the given \p document
 */
QObject *ProjectQmlExtension::createHandler(ProjectDocument *, DocumentHandler *handler){
    return new CodeQmlHandler(m_engine, m_project, m_settings, this, handler);
}

}// namespace
