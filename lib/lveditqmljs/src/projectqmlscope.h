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

#ifndef LVPROJECTQMLSCOPE_H
#define LVPROJECTQMLSCOPE_H

#include <QSet>
#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QSharedPointer>

#include "live/lveditqmljsglobal.h"

class QQmlEngine;
namespace QmlJS{ class LibraryInfo; }


namespace lv{

class ProjectQmlScopeContainer;
class LV_EDITQMLJS_EXPORT ProjectQmlScope{

    Q_DISABLE_COPY(ProjectQmlScope)

public:
    /// Constant SharedPointer to ProjectQmlScope
    typedef QSharedPointer<const ProjectQmlScope> ConstPtr;

    /// SharedPointer to ProjectQmlScope
    typedef QSharedPointer<ProjectQmlScope>       Ptr;

public:
    ~ProjectQmlScope();

    static Ptr create(QQmlEngine* engine);

    void findQmlLibraryInImports(
        const QString& path,
        int versionMajor,
        int versionMinor,
        QList<QString>& paths
    );
    void findQmlLibrary(
        const QString& path,
        int versionMajor,
        int versionMinor,
        QList<QString>& paths
    );
    void findQmlLibraryInPath(
        const QString &path,
        bool requiresQmlDir,
        QList<QString>& paths
    );

    void addImplicitLibrary(const QString& path);

    int totalLibraries() const;
    int totalImplicitLibraries() const;

    ProjectQmlScopeContainer* globalLibraries();
    ProjectQmlScopeContainer* implicitLibraries();

    QList<QString> pathsForImport(const QString& importUri);
    QString uriForPath(const QString& path);

    void addDefaultLibraries(const QList<QString>& paths);
    const QList<QString>& defaultLibraries() const;

private:
    ProjectQmlScope(QQmlEngine* engine);

private:
    QScopedPointer<ProjectQmlScopeContainer> d_globalLibraries;
    QScopedPointer<ProjectQmlScopeContainer> d_implicitLibraries;

    QList<QString> m_defaultLibraries;

    QHash<QString, QList<QString> > m_importToPaths;

    QSet<QString> m_monitoredPaths;
    QStringList m_defaultImportPaths;
};

/// \brief Returns the global libraries container
inline ProjectQmlScopeContainer *ProjectQmlScope::globalLibraries(){
    return d_globalLibraries.data();
}

/// \brief Returns the implicit libraries container
inline ProjectQmlScopeContainer *ProjectQmlScope::implicitLibraries(){
    return d_implicitLibraries.data();
}

/// \brief Returns the stored paths for an import uri.
inline QList<QString> ProjectQmlScope::pathsForImport(const QString &importUri){
    if ( m_importToPaths.contains(importUri) )
        return m_importToPaths.value(importUri);
    return QList<QString>();
}

/// \brief Adds a set of paths to the default libraries.
inline void ProjectQmlScope::addDefaultLibraries(const QList<QString> &paths){
    foreach( const QString& path, paths ){
        if ( !m_defaultLibraries.contains(path) )
            m_defaultLibraries.append(path);
    }
}

/// \brief Returns the default libraries
inline const QList<QString> &ProjectQmlScope::defaultLibraries() const{
    return m_defaultLibraries;
}

}// namespace


#endif // LVPROJECTQMLSCOPE_H
