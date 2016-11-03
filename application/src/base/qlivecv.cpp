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
#include "qcodehandler.h"

#include "qproject.h"
#include "qprojectentry.h"
#include "qprojectfile.h"
#include "qprojectfilemodel.h"

#include <QUrl>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QGuiApplication>

QLiveCV::QLiveCV(int argc, const char* const argv[])
    : m_engine(new QQmlApplicationEngine)
    , m_document(new QCodeDocument)
    , m_dir(QGuiApplication::applicationDirPath())
    , m_project(new lcv::QProject)
{
    m_arguments = new QLiveCVArguments(
        " Live CV v" + versionString() + "\n"
        " --------------------------------------------------- ",
        argc,
        argv
    );

    if ( !m_arguments->consoleFlag() )
        qInstallMessageHandler(&QLiveCVLog::logFunction);
    if ( m_arguments->fileLogFlag() )
        QLiveCVLog::instance().enableFileLog();
}

QLiveCV::~QLiveCV(){
    delete m_engine;
    delete m_document;
}

void QLiveCV::solveImportPaths(){
    QStringList importPaths = m_engine->importPathList();
    m_engine->setImportPathList(QStringList());
    for ( QStringList::iterator it = importPaths.begin(); it != importPaths.end(); ++it ){
        if ( *it != dir() )
            m_engine->addImportPath(*it);
    }
    m_engine->addImportPath(dir() + "/plugins");
}

void QLiveCV::loadLibrary(const QString &library){
    m_lcvlib.setFileName(library);
    m_lcvlib.load();
}

void QLiveCV::loadQml(const QUrl &url){
    solveImportPaths();

    m_engine->rootContext()->setContextProperty("projectFileModel", m_project->fileModel());
    m_engine->rootContext()->setContextProperty("codeDocument", m_document);
    m_engine->rootContext()->setContextProperty("lcvlog", &QLiveCVLog::instance());
    m_engine->rootContext()->setContextProperty("arguments", m_arguments);
#ifdef Q_OS_LINUX
    m_engine->rootContext()->setContextProperty("isLinux", true);
#else
    m_engine->rootContext()->setContextProperty("isLinux", false);
#endif

    m_engine->load(url);
}

void QLiveCV::registerTypes(){
    qmlRegisterUncreatableType<QCodeDocument>(
        "Cv", 1, 0, "Document", "Only access to the document object is allowed.");
    qmlRegisterUncreatableType<QLiveCVLog>(
        "Cv", 1, 0, "MessageLog", "Type is singleton.");
    qmlRegisterType<QCodeHandler>(
        "Cv", 1, 0, "CodeHandler");

    qmlRegisterUncreatableType<lcv::QProjectFileModel>(
        "Cv", 1, 0, "ProjectFileModel", "Cannot create a FileSystemModel instance.");
    qmlRegisterUncreatableType<lcv::QProjectEntry>(
        "Cv", 1, 0, "ProjectEntry", "ProjectEntry objects are managed by the ProjectFileModel.");
    qmlRegisterUncreatableType<lcv::QProjectFile>(
        "Cv", 1, 0, "ProjectFile", "ProjectFile objects are managed by the ProjectFileModel.");
}
