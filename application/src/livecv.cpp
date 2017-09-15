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

#include "livecv.h"
#include "livecvarguments.h"
#include "livecvscript.h"
#include "commands.h"
#include "environment.h"

//#include "livecvlog.h"
#include "live/engine.h"
#include "live/errorhandler.h"
#include "live/settings.h"
#include "live/qeditorsettings.h"


#include "live/qlivepalettecontainer.h"

#include "live/qdocumenthandler.h"
#include "live/qproject.h"
#include "live/qprojectentry.h"
#include "live/qprojectfile.h"
#include "live/qprojectfilemodel.h"
#include "live/qprojectnavigationmodel.h"
#include "live/qprojectfilemodel.h"
#include "live/qprojectdocumentmodel.h"
#include "live/qprojectdocument.h"

#include "live/qqmljssettings.h"
#include "live/qcodeqmlhandler.h"
#include "live/qdocumentqmlinfo.h"
#include "live/qplugininfoextractor.h"

#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QGuiApplication>


namespace lcv{

LiveCV::LiveCV(int argc, const char* const argv[], QObject *parent)
    : QObject(parent)
    , m_engine(new Engine(new QQmlApplicationEngine))
    , m_codeInterface(0)
    , m_dir(QGuiApplication::applicationDirPath())
    , m_project(new QProject)
    , m_settings(0)
    , m_script(0)
    , m_commands(new Commands)
{
    solveImportPaths();

    m_arguments = new LiveCVArguments(
        " Live CV v" + versionString() + "\n"
        " --------------------------------------------------- ",
        argc,
        argv
    );
    m_script = new LiveCVScript(m_arguments->scriptArguments());
    QObject::connect(
        m_project, SIGNAL(activeChanged(QProjectDocument*)),
        m_script, SLOT(scriptChanged(QProjectDocument*))
    );

    m_settings = Settings::initialize(dir() + "/config");
    m_settings->setPreviewMode(m_arguments->previewFlag());
}

LiveCV::~LiveCV(){
    delete m_settings;
    delete m_engine;
}

/**
 * Removes the application directory from the import paths. This is to avoid
 * having duplicate possible imports from within the application:
 *
 * \code
 * import live 1.0
 * \endcode
 *
 * \code
 * import plugins.live 1.0
 * \endcode
 */
void LiveCV::solveImportPaths(){

    QStringList importPaths = m_engine->engine()->importPathList();
    importPaths.removeAll(dir());
    m_engine->engine()->setImportPathList(importPaths);

    // Add the plugins directory to the import paths
    m_engine->engine()->addImportPath(dir() + "/plugins");
}

void LiveCV::loadLibrary(const QString &library){
    m_lcvlib.setFileName(library);
    m_lcvlib.load();

}

void LiveCV::loadQml(const QUrl &url){
    lcv::QCodeQmlHandler* qmlHandler = new lcv::QCodeQmlHandler(
        m_engine->engine(),
        m_engine->engineMutex(),
        m_project->lockedFileIO()
    );

    QEditorSettings* editorSettings = new QEditorSettings(m_settings->path() + "/editor.json");
    editorSettings->addSetting("qmljs", qmlHandler->settings());
    editorSettings->syncWithFile();
    m_settings->addConfigFile("editor", editorSettings);

    m_codeInterface = new lcv::QDocumentHandler(
        qmlHandler,
        QLivePaletteContainer::create(m_engine->engine(), dir() + "/plugins")
    );

    QObject::connect(
        m_project, SIGNAL(inFocusChanged(QProjectDocument*)),
        m_codeInterface, SLOT(setDocument(QProjectDocument*))
    );
    QObject::connect(
        m_project, SIGNAL(pathChanged(QString)),
        qmlHandler, SLOT(newProject(QString))
    );
    QObject::connect(
        m_project, SIGNAL(directoryChanged(QString)),
        qmlHandler, SLOT(directoryChanged(QString))
    );
    QObject::connect(
        m_project, SIGNAL(fileChanged(QString)),
        qmlHandler, SLOT(fileChanged(QString))
    );

    //TODO
//    if ( !m_arguments->consoleFlag() )
//        qInstallMessageHandler(&QLiveCVLog::logFunction);
//    if ( m_arguments->fileLogFlag() )
//        QLiveCVLog::instance().enableFileLog();
    if ( m_arguments->script() != "" ){
        m_project->openProject(m_arguments->script());
    } else {
        m_project->newProject();
    }
    if ( !m_arguments->monitoredFiles().isEmpty() ){
        foreach( QString mfile, m_arguments->monitoredFiles() ){
            if ( !mfile.isEmpty() ){
                QFileInfo mfileInfo(mfile);
                if ( mfileInfo.isRelative() ){
                    m_project->openFile(
                        QDir::cleanPath(m_project->path() + QDir::separator() + mfile),
                        QProjectDocument::Monitor
                    );
                } else {
                    m_project->openFile(mfile, QProjectDocument::Monitor);
                }
            }
        }
    }

    //TODO
//    m_engine->engine()->rootContext()->setContextProperty("lcvlog", &QLiveCVLog::instance());
    m_engine->engine()->rootContext()->setContextProperty("project",     m_project);
    m_engine->engine()->rootContext()->setContextProperty("script",      m_script);
    m_engine->engine()->rootContext()->setContextProperty("codeHandler", m_codeInterface);
    m_engine->engine()->rootContext()->setContextProperty("livecv",      this);

    static_cast<QQmlApplicationEngine*>(m_engine->engine())->load(url);
}

void LiveCV::registerTypes(){
    //TODO
//    qmlRegisterUncreatableType<QLiveCVLog>(
//        "Cv", 1, 0, "MessageLog", "Type is singleton.");
    qmlRegisterUncreatableType<lcv::QDocumentHandler>(
        "Cv", 1, 0, "DocumentHandler", "DocumentHandler is singleton.");
    qmlRegisterType<lcv::QDocumentCursorInfo>(
        "Cv", 1, 0, "DocumentCursorInfo");
    qmlRegisterType<lcv::ErrorHandler>(
        "Cv", 1, 0, "ErrorHandler");

    qmlRegisterUncreatableType<lcv::QProjectFileModel>(
        "Cv", 1, 0, "ProjectFileModel", "Cannot create a ProjectFileModel instance.");
    qmlRegisterUncreatableType<lcv::QProjectDocumentModel>(
        "Cv", 1, 0, "ProjectDocumentModel", "Cannot create a ProjectDocumentModel instance.");
    qmlRegisterUncreatableType<lcv::QProjectNavigationModel>(
        "Cv", 1, 0, "ProjectNavigationModel", "Cannot create a ProjectNavigationModel instance.");

    qmlRegisterUncreatableType<lcv::QProjectEntry>(
        "Cv", 1, 0, "ProjectEntry", "ProjectEntry objects are managed by the ProjectFileModel.");
    qmlRegisterUncreatableType<lcv::QProjectFile>(
        "Cv", 1, 0, "ProjectFile", "ProjectFile objects are managed by the ProjectFileModel.");
    qmlRegisterUncreatableType<lcv::QProjectDocument>(
        "Cv", 1, 0, "ProjectDocument", "ProjectDocument objects are managed by the Project class.");

    qmlRegisterUncreatableType<lcv::LiveCV>(
        "Cv", 1, 0, "LiveCV", "LiveCV is available through the \'livecv\' property."
    );
    qmlRegisterUncreatableType<lcv::Engine>(
        "Cv", 1, 0, "LiveEngine", "LiveEngine is available through \'livecv.engine\' property."
    );
    qmlRegisterUncreatableType<lcv::LiveCVScript>(
        "Cv", 1, 0, "LiveScript", "LiveScript is available through the \'script\' property"
    );
    qmlRegisterUncreatableType<lcv::Environment>(
        "Cv", 1, 0, "LiveEnvironment", "LiveEnvironment is available through the \'script.environment\' property."
    );
    qmlRegisterUncreatableType<lcv::Settings>(
        "Cv", 1, 0, "LiveSettings", "LiveSettings is available through the \'livecv.settings\' property."
    );
    qmlRegisterUncreatableType<lcv::Commands>(
        "Cv", 1, 0, "LiveCommands", "LiveCommands is available through the \'livecv.commands\' property."
    );
    qmlRegisterUncreatableType<lcv::QEditorSettings>(
        "Cv", 1, 0, "EditorSettings", "EditorSettings is available through the \'livecv.settings.editor\' property."
    );
}

QByteArray LiveCV::extractPluginInfo(const QString &import) const{
    lcv::QCodeQmlHandler* qmlHandler = new lcv::QCodeQmlHandler(
        m_engine->engine(),
        m_engine->engineMutex(),
        m_project->lockedFileIO()
    );


    lcv::QPluginInfoExtractor* extractor = qmlHandler->getPluginInfoExtractor(import);
    if ( extractor ){
        extractor->waitForResult(10000);
        if (extractor->timedOut() ){
            return "Error: Timed out\n";
        }
    }
    return extractor->result();
}

}// namespace
