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

#include "editorprivate_plugin.h"
#include "live/applicationcontext.h"
#include "live/settings.h"
#include "live/documenthandler.h"
#include "projectfilemodel.h"
#include "live/projectdocumentmodel.h"
#include "projectnavigationmodel.h"
#include "runnablecontainer.h"
#include "live/project.h"
#include "live/projectentry.h"
#include "live/projectfile.h"
#include "live/projectdocument.h"
#include "live/editorsettings.h"
#include "live/palettecontainer.h"
#include "live/editorglobalobject.h"
#include "live/applicationcontext.h"
#include "live/keymap.h"
#include "live/viewengine.h"
#include "live/viewcontext.h"
#include "linesurface.h"
#include "linemanager.h"
#include "editorlayer.h"
#include "workspacelayer.h"
#include "themecontainer.h"
#include "live/theme.h"

#include <QCoreApplication>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlPropertyMap>

void EditorPrivatePlugin::registerTypes(const char *uri){
    qmlRegisterType<lv::TextEdit>(      uri, 1, 0, "NewTextEdit");
    qmlRegisterType<lv::LineSurface>(   uri, 1, 0, "LineSurface");
    qmlRegisterType<lv::LineManager>(   uri, 1, 0, "LineManager");
    qmlRegisterType<lv::EditorLayer>(   uri, 1, 0, "EditorLayer");
    qmlRegisterType<lv::WorkspaceLayer>(uri, 1, 0, "WorkspaceLayer");

    qmlRegisterUncreatableType<lv::Project>(
        uri, 1, 0, "Project", "Cannot create Project instance.");
    qmlRegisterUncreatableType<lv::ProjectFileModel>(
        uri, 1, 0, "ProjectFileModel", "Cannot create a ProjectFileModel instance.");
    qmlRegisterUncreatableType<lv::ProjectDocumentModel>(
        uri, 1, 0, "ProjectDocumentModel", "Cannot create a ProjectDocumentModel instance.");
    qmlRegisterUncreatableType<lv::ProjectNavigationModel>(
        uri, 1, 0, "ProjectNavigationModel", "Cannot create a ProjectNavigationModel instance.");
    qmlRegisterUncreatableType<lv::Runnable>(
        uri, 1, 0, "Runnable", "Cannot create Runnable instance. Use subproject instead.");
    qmlRegisterUncreatableType<lv::RunnableContainer>(
        uri, 1, 0, "RunnablesContainer", "Cannot create a RunnablesContainer instance.");

    qmlRegisterUncreatableType<lv::ProjectEntry>(
        uri, 1, 0, "ProjectEntry", "ProjectEntry objects are managed by the ProjectFileModel.");
    qmlRegisterUncreatableType<lv::ProjectFile>(
        uri, 1, 0, "ProjectFile", "ProjectFile objects are managed by the ProjectFileModel.");
    qmlRegisterUncreatableType<lv::ProjectDocument>(
        uri, 1, 0, "ProjectDocument", "ProjectDocument objects are managed by the Project class.");
    qmlRegisterUncreatableType<lv::EditorSettings>(
        uri, 1, 0, "EditorSettings", "EditorSettings is available through the \'livecv.settings.editor\' property."
    );

    qmlRegisterUncreatableType<lv::Commands>(
        uri, 1, 0, "LiveCommands", "LiveCommands is available through the \'livecv.layers.workspace.commands\' property.");
    qmlRegisterUncreatableType<lv::KeyMap>(
        uri, 1, 0, "KeyMap", "KeyMap is available through the \'livecv.layers.workspace.keymap.\' property.");
    qmlRegisterUncreatableType<lv::ThemeContainer>(
        uri, 1, 0, "ThemeContainer", "ThemeContainer is available through the \'livecv.layers.workspace.themes.\' property.");
}

void EditorPrivatePlugin::initializeEngine(QQmlEngine *engine, const char *){
    lv::ViewContext::initFromEngine(engine);

    lv::EditorSettings* editorSettings = new lv::EditorSettings(
        lv::ViewContext::instance().settings()->path() + "/editor.json"
    );
    lv::ViewContext::instance().settings()->addConfigFile("editor", editorSettings);

    QObject* prob = engine->rootContext()->contextProperty("project").value<QObject*>();
    lv::Project* pr = qobject_cast<lv::Project*>(prob);

    lv::PaletteContainer* lpc = lv::PaletteContainer::create(
        engine, QString::fromStdString(lv::ApplicationContext::instance().pluginPath())
    );

    lv::EditorGlobalObject* editor = new lv::EditorGlobalObject(pr, lpc);

    engine->rootContext()->setContextProperty("editor", editor);
}
