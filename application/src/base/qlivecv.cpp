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

#include "qlivecv.h"
#include "qlivecvlog.h"
#include "qlivecvengine.h"
#include "qlivecvsettings.h"
#include "qeditorsettings.h"

#include "qlivecvarguments.h"
#include "qstaticcontainer.h"
#include "qlicensecontainer.h"
#include "qlivepalettecontainer.h"

#include "qdocumenthandler.h"
#include "qproject.h"
#include "qprojectentry.h"
#include "qprojectfile.h"
#include "qprojectfilemodel.h"
#include "qprojectnavigationmodel.h"
#include "qprojectdocumentmodel.h"
#include "qprojectdocument.h"

#include "qqmljssettings.h"
#include "qcodeqmlhandler.h"
#include "qdocumentqmlinfo.h"
#include "qplugininfoextractor.h"

#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QGuiApplication>


namespace lcv{

QLiveCV::QLiveCV(int argc, const char* const argv[])
    : m_engine(new QLiveCVEngine(new QQmlApplicationEngine))
    , m_staticContainer(new QStaticContainer(m_engine))
    , m_codeInterface(0)
    , m_dir(QGuiApplication::applicationDirPath())
    , m_project(new QProject)
    , m_settings(0)
{
    solveImportPaths();

    m_arguments = new QLiveCVArguments(
        " Live CV v" + versionString() + "\n"
        " --------------------------------------------------- ",
        argc,
        argv
    );

    m_settings = QLiveCVSettings::initialize(dir() + "/config");
}

QLiveCV::~QLiveCV(){
    delete m_settings;
    delete m_staticContainer;
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
void QLiveCV::solveImportPaths(){

    // Remove the application base dir from the import paths
    // TODO: document why
    QStringList importPaths = m_engine->engine()->importPathList();
    importPaths.removeAll(dir());
    m_engine->engine()->setImportPathList(importPaths);

    // Add the plugins directory to the import paths
    m_engine->engine()->addImportPath(dir() + "/plugins");
}

void QLiveCV::loadLibrary(const QString &library){

    m_lcvlib.setFileName(library);
    m_lcvlib.load();

}

void QLiveCV::loadQml(const QUrl &url){
    lcv::QCodeQmlHandler* qmlHandler = new lcv::QCodeQmlHandler(
        m_engine->engine(),
        m_engine->engineMutex(),
        m_project->lockedFileIO()
    );

    m_settings->editor()->addSetting("qmljs", qmlHandler->settings());
    m_settings->editor()->syncWithFile();

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

    if ( !m_arguments->consoleFlag() )
        qInstallMessageHandler(&QLiveCVLog::logFunction);
    if ( m_arguments->fileLogFlag() )
        QLiveCVLog::instance().enableFileLog();
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

    m_engine->engine()->rootContext()->setContextProperty("staticContainer", m_staticContainer);
    m_engine->engine()->rootContext()->setContextProperty("project", m_project);
    m_engine->engine()->rootContext()->setContextProperty("lcvlog", &QLiveCVLog::instance());
    m_engine->engine()->rootContext()->setContextProperty("args", m_arguments);
    m_engine->engine()->rootContext()->setContextProperty("engine", m_engine);
    m_engine->engine()->rootContext()->setContextProperty("codeHandler", m_codeInterface);
    m_engine->engine()->rootContext()->setContextProperty("settings", m_settings);
#ifdef Q_OS_LINUX
    m_engine->engine()->rootContext()->setContextProperty("isLinux", QVariant::fromValue(true));
#else
    m_engine->engine()->rootContext()->setContextProperty("isLinux", QVariant::fromValue(false));
#endif

    static_cast<QQmlApplicationEngine*>(m_engine->engine())->load(url);

    QList<QObject*> objects = static_cast<QQmlApplicationEngine*>(m_engine->engine())->rootObjects();
    if ( objects.size() > 0 && qobject_cast<QQuickWindow*>(objects.first())){
        m_staticContainer->setWindow(qobject_cast<QQuickWindow*>(objects.first()));
    }
}

void QLiveCV::registerTypes(){
    qmlRegisterUncreatableType<QLiveCVLog>(
        "Cv", 1, 0, "MessageLog", "Type is singleton.");
    qmlRegisterUncreatableType<lcv::QDocumentHandler>(
        "Cv", 1, 0, "DocumentHandler", "DocumentHandler is singleton.");
    qmlRegisterType<lcv::QDocumentCursorInfo>(
        "Cv", 1, 0, "DocumentCursorInfo");

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
    qmlRegisterUncreatableType<lcv::QLiveCVEngine>(
        "Cv", 1, 0, "LiveEngine", "LiveEngine is available through engine property."
    );
    qmlRegisterUncreatableType<lcv::QLiveCVSettings>(
        "Cv", 1, 0, "LiveSettings", "LiveSettings is available through the settings property."
    );
    qmlRegisterUncreatableType<lcv::QEditorSettings>(
        "Cv", 1, 0, "EditorSettings", "EditorSettings is available through the settings.editor property."
    );
    qmlRegisterUncreatableType<lcv::QLicenseContainer>(
        "Cv", 1, 0, "LicenseContainer", "LicenseContainer is available through the settings.license property."
    );
}

QByteArray QLiveCV::extractPluginInfo(const QString &import) const{
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
