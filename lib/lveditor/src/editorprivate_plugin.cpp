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

#include "editorprivate_plugin.h"
#include "live/plugincontext.h"
#include "live/settings.h"
#include "live/documenthandler.h"
#include "live/documentcursorinfo.h"
#include "live/projectfilemodel.h"
#include "live/projectdocumentmodel.h"
#include "live/projectnavigationmodel.h"
#include "live/project.h"
#include "live/projectentry.h"
#include "live/projectfile.h"
#include "live/projectdocument.h"
#include "live/editorsettings.h"
#include "live/livepalettecontainer.h"
#include "live/editorglobalobject.h"
#include "live/plugincontext.h"
#include "live/keymap.h"

#include <QCoreApplication>
#include <QQmlEngine>
#include <QQmlContext>

void EditorPrivatePlugin::registerTypes(const char *uri){
    qmlRegisterType<lv::DocumentHandler>(   uri, 1, 0, "DocumentHandler");
    qmlRegisterType<lv::DocumentCursorInfo>(uri, 1, 0, "DocumentCursorInfo");

    qmlRegisterUncreatableType<lv::ProjectFileModel>(
        uri, 1, 0, "ProjectFileModel", "Cannot create a ProjectFileModel instance.");
    qmlRegisterUncreatableType<lv::ProjectDocumentModel>(
        uri, 1, 0, "ProjectDocumentModel", "Cannot create a ProjectDocumentModel instance.");
    qmlRegisterUncreatableType<lv::ProjectNavigationModel>(
        uri, 1, 0, "ProjectNavigationModel", "Cannot create a ProjectNavigationModel instance.");

    qmlRegisterUncreatableType<lv::ProjectEntry>(
        uri, 1, 0, "ProjectEntry", "ProjectEntry objects are managed by the ProjectFileModel.");
    qmlRegisterUncreatableType<lv::ProjectFile>(
        uri, 1, 0, "ProjectFile", "ProjectFile objects are managed by the ProjectFileModel.");
    qmlRegisterUncreatableType<lv::ProjectDocument>(
        uri, 1, 0, "ProjectDocument", "ProjectDocument objects are managed by the Project class.");
    qmlRegisterUncreatableType<lv::EditorSettings>(
        uri, 1, 0, "EditorSettings", "EditorSettings is available through the \'livecv.settings.editor\' property."
    );
}

void EditorPrivatePlugin::initializeEngine(QQmlEngine *engine, const char *){
    lv::PluginContext::initFromEngine(engine);

    lv::EditorSettings* editorSettings = new lv::EditorSettings(
        lv::PluginContext::settings()->path() + "/editor.json"
    );
    lv::PluginContext::settings()->addConfigFile("editor", editorSettings);

    QObject* prob = engine->rootContext()->contextProperty("project").value<QObject*>();
    lv::Project* pr = qobject_cast<lv::Project*>(prob);

    lv::LivePaletteContainer* lpc = lv::LivePaletteContainer::create(
        engine, lv::PluginContext::pluginPath()
    );

    lv::EditorGlobalObject* editor = new lv::EditorGlobalObject(pr, lpc);

    engine->rootContext()->setContextProperty("editor", editor);

    QObject* livecv    = engine->rootContext()->contextProperty("livecv").value<QObject*>();
    if ( livecv ){
        lv::KeyMap* keymap = static_cast<lv::KeyMap*>(livecv->property("keymap").value<QObject*>());
        keymap->store(0, Qt::Key_O,         lv::KeyMap::CONTROL_OR_COMMAND, "window.project.openFile");
        keymap->store(0, Qt::Key_S,         lv::KeyMap::CONTROL_OR_COMMAND, "window.editor.saveFile");
        keymap->store(0, Qt::Key_S,         lv::KeyMap::CONTROL_OR_COMMAND | lv::KeyMap::Alt, "window.editor.saveFileAs");
        keymap->store(0, Qt::Key_W,         lv::KeyMap::CONTROL_OR_COMMAND, "window.editor.closeFile");

        keymap->store(0, Qt::Key_Backslash, lv::KeyMap::CONTROL_OR_COMMAND, "window.project.toggleVisibility");
        keymap->store(0, Qt::Key_T,         lv::KeyMap::CONTROL_OR_COMMAND, "window.toggleMaximizedRuntime");
        keymap->store(0, Qt::Key_E,         lv::KeyMap::CONTROL_OR_COMMAND, "window.editor.toggleSize");
        keymap->store(0, Qt::Key_K,         lv::KeyMap::CONTROL_OR_COMMAND, "window.toggleNavigation");
        keymap->store(0, Qt::Key_L,         lv::KeyMap::CONTROL_OR_COMMAND, "window.toggleLog");
        keymap->store(0, Qt::Key_BracketLeft,  lv::KeyMap::CONTROL_OR_COMMAND | lv::KeyMap::Shift, "window.addHorizontalEditorView");
        keymap->store(0, Qt::Key_BracketLeft,  lv::KeyMap::CONTROL_OR_COMMAND | lv::KeyMap::Alt,   "window.addHorizontalFragmentEditorView");
        keymap->store(0, Qt::Key_BracketRight, lv::KeyMap::CONTROL_OR_COMMAND | lv::KeyMap::Shift, "window.removeHorizontalEditorView");

        keymap->store(lv::KeyMap::Linux,    Qt::Key_Space, lv::KeyMap::Control, "window.editor.assistCompletion");
        keymap->store(lv::KeyMap::Windows,  Qt::Key_Space, lv::KeyMap::Control, "window.editor.assistCompletion");
        keymap->store(lv::KeyMap::Mac,      Qt::Key_Space, lv::KeyMap::Alt,     "window.editor.assistCompletion");
    }

}
