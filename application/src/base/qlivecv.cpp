/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
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
#include "qcodedocument.h"
#include "qlivecvarguments.h"
#include "qlivecvmain.h"

#include "qdocumentcodeinterface.h"
#include "qproject.h"
#include "qprojectentry.h"
#include "qprojectfile.h"
#include "qprojectfilemodel.h"
#include "qprojectnavigationmodel.h"
#include "qprojectdocumentmodel.h"
#include "qprojectdocument.h"
#include "qlivecvengine.h"

#include "qdocumentqmlhandler.h"
#include "qdocumentqmlinfo.h"
#include "qplugininfoextractor.h"

#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QGuiApplication>

namespace lcv{

QLiveCV::QLiveCV(int argc, const char* const argv[])
    : m_engine(new QLiveCVEngine(new QQmlApplicationEngine))
    , m_document(new QCodeDocument)
    , m_codeInterface(0)
    , m_dir(QGuiApplication::applicationDirPath())
    , m_project(new lcv::QProject)
{
    solveImportPaths();

    m_arguments = new QLiveCVArguments(
        " Live CV v" + versionString() + "\n"
        " --------------------------------------------------- ",
        argc,
        argv
    );
}

QLiveCV::~QLiveCV(){
    delete m_engine;
    delete m_document;
}

void QLiveCV::solveImportPaths(){
    QStringList importPaths = m_engine->engine()->importPathList();
    m_engine->engine()->setImportPathList(QStringList());
    for ( QStringList::iterator it = importPaths.begin(); it != importPaths.end(); ++it ){
        if ( *it != dir() )
            m_engine->engine()->addImportPath(*it);
    }
    m_engine->engine()->addImportPath(dir() + "/plugins");
}

void QLiveCV::loadLibrary(const QString &library){
    m_lcvlib.setFileName(library);
    m_lcvlib.load();
}

void QLiveCV::loadQml(const QUrl &url){

    lcv::QDocumentQmlHandler* qmlHandler = new lcv::QDocumentQmlHandler(
        m_engine->engine(),
        m_engine->engineMutex(),
        m_project->lockedFileIO()
    );
    m_codeInterface = new lcv::QDocumentCodeInterface(qmlHandler);
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
    if ( m_arguments->script() != "" )
        m_project->openProject(m_arguments->script());
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

    m_engine->engine()->rootContext()->setContextProperty("project", m_project);
    m_engine->engine()->rootContext()->setContextProperty("codeDocument", m_document);
    m_engine->engine()->rootContext()->setContextProperty("lcvlog", &QLiveCVLog::instance());
    m_engine->engine()->rootContext()->setContextProperty("args", m_arguments);
    m_engine->engine()->rootContext()->setContextProperty("engine", m_engine);
    m_engine->engine()->rootContext()->setContextProperty("codeHandler", m_codeInterface);
#ifdef Q_OS_LINUX
    m_engine->engine()->rootContext()->setContextProperty("isLinux", true);
#else
    m_engine->engine()->rootContext()->setContextProperty("isLinux", false);
#endif

    static_cast<QQmlApplicationEngine*>(m_engine->engine())->load(url);
}

void QLiveCV::registerTypes(){
    qmlRegisterUncreatableType<QCodeDocument>(
        "Cv", 1, 0, "Document", "Only access to the document object is allowed.");
    qmlRegisterUncreatableType<QLiveCVLog>(
        "Cv", 1, 0, "MessageLog", "Type is singleton.");
    qmlRegisterUncreatableType<lcv::QDocumentCodeInterface>(
        "Cv", 1, 0, "DocumentCodeInterface", "DocumentCodeInterface is singleton.");

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
        "live", 1, 0, "LiveEngine", "LiveEngine is available through engine property."
    );
    qmlRegisterUncreatableType<lcv::QLiveCVArguments>(
        "live", 1, 0, "LiveArguments", "LiveArguments is available through the arguments property."
    );
    qmlRegisterType<lcv::QLiveCVMain>("live", 1, 0, "Main");
}

QByteArray QLiveCV::extractPluginInfo(const QString &import) const{
    lcv::QDocumentQmlHandler* qmlHandler = new lcv::QDocumentQmlHandler(
        m_engine->engine(),
        m_engine->engineMutex(),
        m_project->lockedFileIO()
    );


    lcv::QPluginInfoExtractor* extractor = qmlHandler->getPluginInfoExtractor(import);
    if ( extractor ){
        extractor->waitForResult(10000);
        if (extractor->timedOut() ){
            return "Timed out\n";
        }
    }
    return extractor->result();
}

}// namespace
