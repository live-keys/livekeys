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

#include <QUrl>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QGuiApplication>

QLiveCV::QLiveCV(const QStringList& arguments)
    : m_engine(new QQmlApplicationEngine)
    , m_document(new QCodeDocument)
    , m_dir(QGuiApplication::applicationDirPath())
{
    parseArguments(arguments);
}

QLiveCV::~QLiveCV(){
    delete m_engine;
    delete m_document;
}

void QLiveCV::parseArguments(const QStringList &arguments){
    if ( !arguments.contains("-c" ) )
        qInstallMessageHandler(&QLiveCVLog::logFunction);
    if ( arguments.contains("-l") )
        QLiveCVLog::instance().enableFileLog();
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

    m_engine->rootContext()->setContextProperty("codeDocument", m_document);
    m_engine->rootContext()->setContextProperty("lcvlog", &QLiveCVLog::instance());
#ifdef Q_OS_LINUX
    m_engine->rootContext()->setContextProperty("isLinux", true);
#else
    m_engine->rootContext()->setContextProperty("isLinux", false);
#endif

    m_engine->load(url);
}
