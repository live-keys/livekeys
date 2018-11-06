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

namespace lv{

ProjectQmlExtension::ProjectQmlExtension(Settings *settings, Project *project, ViewEngine* engine)
    : m_settings(new QmlJsSettings())
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
        QObject *parent)
{
    QString filePath = document->file()->path();
    if ( filePath.toLower().endsWith(".js") || filePath.toLower().endsWith(".qml") || filePath.isEmpty() ){
        CodeQmlHandler* ch = new CodeQmlHandler(engine, project, m_settings, this, parent);
        return ch;
    }
    return 0;
}

ProjectQmlScanner *ProjectQmlExtension::scanner(){ return m_scanMonitor->m_scanner; }

PluginInfoExtractor *ProjectQmlExtension::getPluginInfoExtractor(const QString &import){
    return m_scanMonitor->getPluginInfoExtractor(import);
}

void ProjectQmlExtension::engineHook(const QString &code, const QUrl &, QObject *result, QObject *document){
    ProjectDocument* doc = static_cast<ProjectDocument*>(document);
    DocumentQmlInfo::syncBindings(code, doc, result);
}

}// namespace
