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

#include "editorprivate_plugin.h"
#include "live/applicationcontext.h"
#include "live/settings.h"
#include "live/codehandler.h"
#include "projectfilemodel.h"
#include "live/projectdocumentmodel.h"
#include "projectfileindexer.h"
#include "runnablecontainer.h"
#include "live/project.h"
#include "live/projectentry.h"
#include "live/projectfile.h"
#include "live/projectdocument.h"
#include "live/editorsettings.h"
#include "live/paletteloader.h"
#include "live/applicationcontext.h"
#include "live/keymap.h"
#include "live/viewengine.h"
#include "live/theme.h"
#include "linesurface.h"
#include "editorlayer.h"
#include "workspacelayer.h"
#include "fileformattypes.h"
#include "themecontainer.h"
#include "documentation.h"

#include <QCoreApplication>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlPropertyMap>

void EditorPrivatePlugin::registerTypes(const char *uri){
    qmlRegisterType<lv::TextEdit>(      uri, 1, 0, "NewTextEdit");
    qmlRegisterType<lv::LineSurface>(   uri, 1, 0, "LineSurface");

    qmlRegisterUncreatableType<lv::Project>(
        uri, 1, 0, "Project", "Cannot create Project instance.");
    qmlRegisterUncreatableType<lv::ProjectFileModel>(
        uri, 1, 0, "ProjectFileModel", "Cannot create a ProjectFileModel instance.");
    qmlRegisterUncreatableType<lv::ProjectDocumentModel>(
        uri, 1, 0, "ProjectDocumentModel", "Cannot create a ProjectDocumentModel instance.");
    qmlRegisterUncreatableType<lv::ProjectFileIndexer>(
        uri, 1, 0, "ProjectFileIndexer", "Cannot create a ProjectFileIndexer instance.");
    qmlRegisterUncreatableType<lv::Runnable>(
        uri, 1, 0, "Runnable", "Cannot create Runnable instance. Use subproject instead.");
    qmlRegisterUncreatableType<lv::RunnableContainer>(
        uri, 1, 0, "RunnablesContainer", "Cannot create a RunnablesContainer instance.");

    qmlRegisterUncreatableType<lv::ProjectEntry>(
        uri, 1, 0, "ProjectEntry", "ProjectEntry objects are managed by the ProjectFileModel.");
    qmlRegisterUncreatableType<lv::ProjectFile>(
        uri, 1, 0, "ProjectFile", "ProjectFile objects are managed by the ProjectFileModel.");
    qmlRegisterUncreatableType<lv::Document>(
        uri, 1, 0, "Document", "Document objects are managed by the Project class.");
    qmlRegisterUncreatableType<lv::ProjectDocument>(
        uri, 1, 0, "ProjectDocument", "ProjectDocument objects are managed by the Project class.");
    qmlRegisterUncreatableType<lv::EditorSettings>(
        uri, 1, 0, "EditorSettings", "EditorSettings is available through the \'lk.settings.editor\' property.");

    qmlRegisterUncreatableType<lv::Documentation>(
        uri, 1, 0, "Documentation", "Documentation is available through the \'lk.layers.workspace.documentation\' property.");
    qmlRegisterUncreatableType<lv::Commands>(
        uri, 1, 0, "LiveCommands", "LiveCommands is available through the \'lk.layers.workspace.commands\' property.");
    qmlRegisterUncreatableType<lv::KeyMap>(
        uri, 1, 0, "KeyMap", "KeyMap is available through the \'lk.layers.workspace.keymap.\' property.");
    qmlRegisterUncreatableType<lv::FileFormatTypes>(
        uri, 1, 0, "FileFormatTypes", "FileFormatTypes is available through the \'lk.layers.workspace.fileFormats.\' property.");
    qmlRegisterUncreatableType<lv::ThemeContainer>(
        uri, 1, 0, "ThemeContainer", "ThemeContainer is available through the \'lk.layers.workspace.themes.\' property.");
    qmlRegisterUncreatableType<lv::StartupModel>(
        uri, 1, 0, "StartupModel", "StartupModel objects are available through \'lk.layers.workspace.(recents/tutorials/samples)\'.");

}

void EditorPrivatePlugin::initializeEngine(QQmlEngine *, const char *){

}
