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
#include <QMutex>
#include <QTimer>

#include "live/lveditqmljsglobal.h"
#include "live/qmllanguageinfo.h"
#include "live/lockedfileiosession.h"

class QQmlEngine;
namespace QQmlJS{ class LibraryInfo; }

namespace lv{

class QmlLanguageScanMonitor;
class QmlLanguageScanner;
class LanguageQmlHandler;

class LV_EDITQMLJS_EXPORT ProjectQmlScope : public QObject{

    Q_OBJECT
    Q_DISABLE_COPY(ProjectQmlScope)

public:
    /// Constant SharedPointer to ProjectQmlScope
    typedef QSharedPointer<const ProjectQmlScope> ConstPtr;

    /// SharedPointer to ProjectQmlScope
    typedef QSharedPointer<ProjectQmlScope>       Ptr;

public:
    ~ProjectQmlScope();

    static Ptr create(LockedFileIOSession::Ptr ioSession, QQmlEngine* engine);

    static QmlLibraryInfo::Ptr findQmlLibraryInImports(
        const QStringList& importPaths,
        QString uri,
        int versionMajor,
        int versionMinor
    );
    static QmlLibraryInfo::Ptr findQmlLibraryInImports(
        const QString& uri,
        const QString fullPath,
        int versionMajor,
        int versionMinor
    );
    static QmlLibraryInfo::Ptr findQmlLibraryInPath(
        const QString& uri,
        const QString &fullPath,
        bool requiresQmlDir
    );

    QmlLibraryInfo::Ptr addQmlGlobalLibrary(const QString& uri, int major, int minor);
    QmlLibraryInfo::Ptr addQmlProjectLibrary(const QString& uri);

    QList<QString> pathsForImport(const QString& importUri);
    QString uriForPath(const QString& path);

    void addDefaultLibraries(const QList<QString>& paths);
    const QList<QString>& defaultLibraries() const;

    QmlLibraryInfo::Ptr libraryInfo(const QString& path);
    bool libraryExists(const QString& path);
    void addLibrary(const QString& path);
    void assignLibrary(const QString& path, QmlLibraryInfo::Ptr libinfo);
    void assignLibraries(const QHash<QString, QmlLibraryInfo::Ptr> &libinfos);
    int totalLibraries() const;

    QList<QmlLibraryInfo::Ptr> getLibrariesInPath(const QString& path);

    void resetLibrariesInPath(const QString& path);
    void resetLibrary(const QString& path);

    QString toString();

    QmlLanguageScanMonitor* languageScanMonitor();
    QmlLanguageScanner* languageScanner();

public slots:
    void __libraryUpdates();

signals:
    void __processQueue();
    void libraryScanQueueCleared();

private:
    ProjectQmlScope(LockedFileIOSession::Ptr ioSession, QQmlEngine* engine, QObject* parent = nullptr);
    void queueLibrary(const QmlLibraryInfo::Ptr& lib);

private:
    QList<QString>                  m_defaultLibraries;
    QHash<QString, QList<QString> > m_importToPaths;
    QStringList                     m_defaultImportPaths;

    QmlLanguageScanMonitor* m_scanMonitor;
    QThread*                m_monitorThread;
    QTimer*                 m_scanTimer;

    QMutex m_libraryMutex;
    QHash<QString, QmlLibraryInfo::Ptr> m_libraries;
};

/// \brief Returns the stored paths for an import uri.
inline QList<QString> ProjectQmlScope::pathsForImport(const QString &importUri){
    if ( m_importToPaths.contains(importUri) )
        return m_importToPaths.value(importUri);
    return QList<QString>();
}

/// \brief Adds a set of paths to the default libraries.
inline void ProjectQmlScope::addDefaultLibraries(const QList<QString> &paths){
    foreach( const QString& path, paths ){
        if ( !m_defaultLibraries.contains(path) ){
            m_defaultLibraries.append(path);
        }
    }
}

/// \brief Returns the default libraries
inline const QList<QString> &ProjectQmlScope::defaultLibraries() const{
    return m_defaultLibraries;
}

inline QmlLanguageScanMonitor *ProjectQmlScope::languageScanMonitor(){
    return m_scanMonitor;
}

}// namespace


#endif // LVPROJECTQMLSCOPE_H
