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

#include "live/projectqmlscope.h"
#include "live/visuallogqt.h"
#include "qmllibrarydependency.h"
#include "qmllanguagescanner.h"
#include "qmllanguagescanmonitor.h"

#include "qmljs/qmljsdocument.h"
#include "qmljs/qmljsinterpreter.h"
#include "qmljs/qmljsbind.h"
#include "qmljs/qmljstypedescriptionreader.h"
#include "qmljs/qmljsdescribevalue.h"

#include "QtQml/qqml.h"
#include <QDir>
#include <QQmlEngine>

namespace lv{

/**
 * \class ProjectQmlScope
 * \ingroup lveditqmljs
 * \brief Contains all the loaded plugins of a project
 */

/**
 * \brief ProjectQmlScope constructor
 *
 * This constructor is private, if you want to create a new ProjectQmlScope, you need to use the
 * lv::ProjectQmlScope::create function.
 */

ProjectQmlScope::ProjectQmlScope(LockedFileIOSession::Ptr ioSession, QQmlEngine *engine, QObject *parent)
    : QObject(parent)
    , m_defaultImportPaths(engine->importPathList())
    , m_scanMonitor(new QmlLanguageScanMonitor(new QmlLanguageScanner(ioSession, engine->importPathList())))
    , m_monitorThread(new QThread)
    , m_scanTimer(new QTimer)
{
    m_scanTimer->setInterval(5000);
    m_scanTimer->setSingleShot(false);

    m_scanMonitor->moveToThread(m_monitorThread);

    connect(this, &ProjectQmlScope::__processQueue, m_scanMonitor, &QmlLanguageScanMonitor::processQueue);
    connect(m_scanTimer, &QTimer::timeout,          m_scanMonitor, &QmlLanguageScanMonitor::processQueue);
    connect(m_scanMonitor, &QmlLanguageScanMonitor::libraryUpdates, this, &ProjectQmlScope::__libraryUpdates);

    m_scanTimer->start();
    m_monitorThread->start();

    QStringList versionSegments = QString(QML_VERSION_STR).split(".");
    int versionMajor = versionSegments.length() > 0 ? versionSegments[0].toInt() : 2;
    int versionMinor = versionSegments.length() > 1 ? versionSegments[1].toInt() : 0;

    QmlLibraryInfo::Ptr qtqml = findQmlLibraryInImports(m_defaultImportPaths, "QtQml", versionMajor, versionMinor);
    QmlLibraryInfo::Ptr qtqmlModels = findQmlLibraryInImports(m_defaultImportPaths, "QtQml.Models", versionMajor, versionMinor);

    if ( qtqml ){
        if ( !m_defaultLibraries.contains(qtqml->uri()) ){
            m_defaultLibraries.append(qtqml->uri());
        }
        queueLibrary(qtqml);
    }
    if ( qtqmlModels ){
        if ( !m_defaultLibraries.contains(qtqmlModels->uri()) ){
            m_defaultLibraries.append(qtqmlModels->uri());
        }
        queueLibrary(qtqmlModels);
    }

}

void ProjectQmlScope::queueLibrary(const QmlLibraryInfo::Ptr &lib){
    m_scanMonitor->queueLibrary(lib);
    if ( !m_scanMonitor->isProcessing() ){
        emit __processQueue();
        m_scanTimer->start();
    }
}

/**
 * \brief ProjectQmlScope destructor
 */
ProjectQmlScope::~ProjectQmlScope(){
    m_scanMonitor->requestStop();
    if ( !m_monitorThread->wait(100) ){
        m_monitorThread->terminate();
        m_monitorThread->wait();
    }
    delete m_scanTimer;
    delete m_scanMonitor;
    delete m_monitorThread;
}

/**
 * \brief Creates a new ProjectQmlScope object
 */
ProjectQmlScope::Ptr ProjectQmlScope::create(LockedFileIOSession::Ptr ioSession, QQmlEngine *engine, QObject *parent){
    return ProjectQmlScope::Ptr(new ProjectQmlScope(ioSession, engine));
}

QmlLibraryInfo::Ptr ProjectQmlScope::findQmlLibraryInImports(
        const QStringList &importPaths,
        QString uri,
        int versionMajor,
        int versionMinor)
{
    QString path = uri.replace('.', '/');

    for( const QString& importPath: importPaths ){
        QmlLibraryInfo::Ptr lib = findQmlLibraryInImports(
            uri,
            QDir::cleanPath(importPath + QDir::separator() + path),
            versionMajor,
            versionMinor
        );
        if ( lib )
            return lib;
    }

    return nullptr;
}

QmlLibraryInfo::Ptr ProjectQmlScope::findQmlLibraryInImports(
        const QString &uri,
        const QString fullPath,
        int versionMajor,
        int versionMinor)
{
    QString pathMajorMinor = QString::fromLatin1("%1.%2.%3").arg(
        fullPath, QString::number(versionMajor), QString::number(versionMinor)
    );
    QString pathMajor = QString::fromLatin1("%1.%2").arg(
        fullPath, QString::number(versionMajor)
    );

    QmlLibraryInfo::Ptr lib;
    lib = findQmlLibraryInPath(uri, pathMajorMinor, true);
    if ( !lib )
        lib = findQmlLibraryInPath(uri, pathMajor, true);

    if ( !lib )
        lib = findQmlLibraryInPath(uri, fullPath, true);

    if ( lib )
        lib->updateImportInfo(versionMajor, versionMinor);

    return lib;
}

QmlLibraryInfo::Ptr ProjectQmlScope::findQmlLibraryInPath(
        const QString &uri,
        const QString& fullPath,
        bool requiresQmlDir)
{
    const QDir dir(fullPath);
    if ( !dir.exists() )
        return nullptr;

    QFile dirFile(dir.filePath("qmldir"));
    if( !dirFile.exists() ){
        if ( !requiresQmlDir ){
            QmlLibraryInfo::Ptr li = QmlLibraryInfo::create(uri);
            li->setPath(fullPath);
            return li;
        }
        return nullptr;
    }

    dirFile.open(QFile::ReadOnly);

    QmlDirParser dirParser;
    dirParser.parse(QString::fromUtf8(dirFile.readAll()));

    QmlLibraryInfo::Ptr linfo = QmlLibraryInfo::create(dirParser);
    linfo->setPath(fullPath);
    return linfo;
}

QmlLibraryInfo::Ptr ProjectQmlScope::addQmlGlobalLibrary(const QString &uri, int major, int minor){
    auto fit = m_libraries.find(uri);
    if ( fit != m_libraries.end() ){
        return fit.value();
    }

    QmlLibraryInfo::Ptr lib = findQmlLibraryInImports(m_defaultImportPaths, uri, major, minor);
    if ( lib ){
        m_libraries[lib->uri()] = lib;
        queueLibrary(lib);
        return lib;
    }
    return nullptr;
}

QmlLibraryInfo::Ptr ProjectQmlScope::addQmlProjectLibrary(const QString &uri){
    auto fit = m_libraries.find(uri);
    if ( fit != m_libraries.end() ){
        return fit.value();
    }

    QmlLibraryInfo::Ptr lib = findQmlLibraryInPath(uri, uri, false);
    if ( lib ){
        m_libraries[lib->uri()] = lib;
        queueLibrary(lib);
        return lib;
    }
    return nullptr;
}

/**
 * \brief Returns the total number of global libraries
 */
int ProjectQmlScope::totalLibraries() const{
    return m_libraries.size();
}

QList<QmlLibraryInfo::Ptr> ProjectQmlScope::getLibrariesInPath(const QString &path){
    QList<QmlLibraryInfo::Ptr> libraries;
    m_libraryMutex.lock();
    for( auto it = m_libraries.begin(); it != m_libraries.end(); ++it ){
        if ( it.key().startsWith(path) ){
            libraries.append(it.value());
        }
    }
    m_libraryMutex.unlock();
    return libraries;
}

void ProjectQmlScope::resetLibrariesInPath(const QString &path){
    m_libraryMutex.lock();
    for( auto it = m_libraries.begin(); it != m_libraries.end(); ++it ){
        if ( it.key().startsWith(path) ){
            it.value()->setStatus(QmlLibraryInfo::NotScanned);
            //HERE
        }
    }
    m_libraryMutex.unlock();
}

void ProjectQmlScope::resetLibrary(const QString &path){
    m_libraryMutex.lock();
    if ( m_libraries.contains(path) ) //HERE
        m_libraries[path]->setStatus(QmlLibraryInfo::NotScanned);
    m_libraryMutex.unlock();
}

QString ProjectQmlScope::toString(){
    QString result;
    m_libraryMutex.lock();
    for( auto it = m_libraries.begin(); it != m_libraries.end(); ++it ){
        QmlLibraryInfo::ConstPtr linfo = it.value();
        result += linfo->uri() + "[" + linfo->statusString() + "]\n";
    }
    m_libraryMutex.unlock();
    return result;
}

void ProjectQmlScope::__libraryUpdates(){
    auto libs = m_scanMonitor->detachLibraries();
    for ( auto lib : libs ){
        m_libraries[lib->uri()] = lib;
    }
}

/**
 * \brief Returns the uri of a library from it's path
 */
QString ProjectQmlScope::uriForPath(const QString &path){
    QString bestmatch;
    for ( QHash<QString, QList<QString> >::iterator it = m_importToPaths.begin(); it != m_importToPaths.end(); ++it ){
        for ( QList<QString>::iterator iit = it.value().begin(); iit != it.value().end(); ++iit ){
            if ( *iit == path && it.key() > bestmatch )
                bestmatch = it.key();
        }
    }
    return bestmatch;
}

QmlLibraryInfo::Ptr ProjectQmlScope::libraryInfo(const QString &path){
    QmlLibraryInfo::Ptr libinfo(nullptr);
    m_libraryMutex.lock();
    libinfo = m_libraries.value(path, QmlLibraryInfo::Ptr(nullptr));
    if ( libinfo.isNull() ){
        libinfo = QmlLibraryInfo::create(path);
        m_libraries[path] = libinfo;
    }
    m_libraryMutex.unlock();
    return libinfo;
}

bool ProjectQmlScope::libraryExists(const QString &path){
    m_libraryMutex.lock();
    bool contains = m_libraries.contains(path);
    m_libraryMutex.unlock();
    return contains;
}

void ProjectQmlScope::addLibrary(const QString &path){
    assignLibrary(path, QmlLibraryInfo::create(path));
}

void ProjectQmlScope::assignLibrary(const QString &path, lv::QmlLibraryInfo::Ptr libinfo){
    m_libraryMutex.lock();
    m_libraries[path] = libinfo;
    m_libraryMutex.unlock();
}

void ProjectQmlScope::assignLibraries(const QHash<QString, QmlLibraryInfo::Ptr> &libinfos){
    m_libraryMutex.lock();
    for( auto it = libinfos.begin(); it != libinfos.end(); ++it ){
        m_libraries[it.key()] = it.value();
    }
    m_libraryMutex.unlock();
}

QmlLanguageScanner *ProjectQmlScope::languageScanner(){
    return m_scanMonitor->languageScanner();
}

}// namespace




