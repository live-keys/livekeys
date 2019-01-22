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

#include "live/projectqmlscope.h"
#include "projectqmlscopecontainer_p.h"
#include "qmllibrarydependency.h"
#include "qmllibraryinfo_p.h"

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

ProjectQmlScope::ProjectQmlScope(QQmlEngine *engine)
    : d_globalLibraries(new ProjectQmlScopeContainer)
    , d_implicitLibraries(new ProjectQmlScopeContainer)
    , m_defaultImportPaths(engine->importPathList())
{
    QStringList versionSegments = QString(QML_VERSION_STR).split(".");
    int versionMajor = versionSegments.length() > 0 ? versionSegments[0].toInt() : 2;
    int versionMinor = versionSegments.length() > 1 ? versionSegments[1].toInt() : 0;

    QList<QString> paths;
    findQmlLibraryInImports("QtQml", versionMajor, versionMinor, paths);
    findQmlLibraryInImports("QtQml/Models", versionMajor, versionMinor, paths);

    addDefaultLibraries(paths);
}

/**
 * \brief ProjectQmlScope destructor
 */
ProjectQmlScope::~ProjectQmlScope(){
}

/**
 * \brief Creates a new ProjectQmlScope object
 */
ProjectQmlScope::Ptr ProjectQmlScope::create(QQmlEngine *engine){
    return ProjectQmlScope::Ptr(new ProjectQmlScope(engine));
}

/**
 * \brief Finds a libraries paths given it's uri
 */
void ProjectQmlScope::findQmlLibraryInImports(
        const QString &path,
        int versionMajor,
        int versionMinor,
        QList<QString> &paths)
{
    QList<QString> newPaths;
    QString importUri = path;
    importUri.replace('/', '.') += " " + QString::number(versionMajor) + "." + QString::number(versionMinor);
    if ( m_importToPaths.contains(importUri) ){
        paths.append(m_importToPaths.value(importUri));
        return;
    }

    foreach( const QString& importPath, m_defaultImportPaths ){
        findQmlLibrary(
            QDir::cleanPath(importPath + QDir::separator() + path),
            versionMajor,
            versionMinor,
            newPaths
        );
        if ( !newPaths.isEmpty() ) // avoid duplicates in multiple import paths
            break;
    }

    if ( !newPaths.isEmpty() )
        m_importToPaths[importUri] = newPaths;

    paths << newPaths;
}

/**
 * \brief Finds a qml library in a given path
 */
void ProjectQmlScope::findQmlLibrary(
        const QString &path,
        int versionMajor,
        int versionMinor,
        QList<QString>& paths)
{
    QString pathMajorMinor = QString::fromLatin1("%1.%2.%3").arg(
        path, QString::number(versionMajor), QString::number(versionMinor)
    );
    QString pathMajor = QString::fromLatin1("%1.%2").arg(
        path, QString::number(versionMajor)
    );

    findQmlLibraryInPath(pathMajorMinor, true, paths);
    findQmlLibraryInPath(pathMajor, true, paths);
    findQmlLibraryInPath(path, true, paths);
}

/**
 * \brief Finds a qml library in a given path.
 */
void ProjectQmlScope::findQmlLibraryInPath(const QString &path, bool requiresQmlDir, QList<QString>& paths){
    if ( d_globalLibraries->libraryExists(path) ){
        paths.append(path);
        return;
    }

    const QDir dir(path);
    if ( !dir.exists() )
        return;

    QFile dirFile(dir.filePath("qmldir"));
    if( !dirFile.exists() ){
        if ( !requiresQmlDir ){
            d_globalLibraries->assignLibrary(path, QmlLibraryInfo::create());
            paths.append(path);
        }
        return;
    }

    dirFile.open(QFile::ReadOnly);

    QmlDirParser dirParser;
    dirParser.parse(QString::fromUtf8(dirFile.readAll()));

    d_globalLibraries->assignLibrary(path, QmlLibraryInfo::create(dirParser));
    paths.append(path);
}

/**
 * \brief Adds a library as implicit
 */
void ProjectQmlScope::addImplicitLibrary(const QString &path){
    if ( !d_implicitLibraries->libraryExists(path) )
        d_implicitLibraries->assignLibrary(path, QmlLibraryInfo::create());
}

/**
 * \brief Returns the total number of global libraries
 */
int ProjectQmlScope::totalLibraries() const{
    return d_globalLibraries->totalLibraries();
}

/**
 * \brief Returns the total number of implicit libraries
 */
int ProjectQmlScope::totalImplicitLibraries() const{
    return d_implicitLibraries->totalLibraries();
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

}// namespace




