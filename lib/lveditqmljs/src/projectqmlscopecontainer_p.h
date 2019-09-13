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

#ifndef LVPROJECTQMLSCOPECONTAINER_P_H
#define LVPROJECTQMLSCOPECONTAINER_P_H

#include <QMutex>
#include <QMutexLocker>
#include "qmllibraryinfo_p.h"

namespace lv{

/// \private
class ProjectQmlScopeContainer{

public:
    ProjectQmlScopeContainer(){}
    ~ProjectQmlScopeContainer(){}

    QmlLibraryInfo::Ptr libraryInfo(const QString& path);
    bool libraryExists(const QString& path);
    void assignLibrary(const QString& path, QmlLibraryInfo::Ptr libinfo);
    void assignLibraries(const QHash<QString, QmlLibraryInfo::Ptr>& libinfos);
    int totalLibraries() const;

    QHash<QString, QmlLibraryInfo::Ptr> getNoInfoLibraries();
    QHash<QString, QmlLibraryInfo::Ptr> getNoLinkLibraries();

    void resetLibrariesInPath(const QString& path);
    void resetLibrary(const QString& path);

private:
    QMutex m_libraryMutex;
    QHash<QString, QmlLibraryInfo::Ptr> m_libraries;
};

inline QmlLibraryInfo::Ptr ProjectQmlScopeContainer::libraryInfo(const QString &path){
    QmlLibraryInfo::Ptr libinfo(0);
    m_libraryMutex.lock();
    libinfo = m_libraries.value(path, QmlLibraryInfo::Ptr(0));
    if ( libinfo.isNull() ){
        libinfo = QmlLibraryInfo::create();
        m_libraries[path] = libinfo;
    }
    m_libraryMutex.unlock();
    return libinfo;
}

inline bool ProjectQmlScopeContainer::libraryExists(const QString &path){
    m_libraryMutex.lock();
    bool contains = m_libraries.contains(path);
    m_libraryMutex.unlock();
    return contains;
}

inline void ProjectQmlScopeContainer::assignLibrary(const QString &path, QmlLibraryInfo::Ptr libinfo){
    m_libraryMutex.lock();
    m_libraries[path] = libinfo;
    m_libraryMutex.unlock();
}

inline void ProjectQmlScopeContainer::assignLibraries(const QHash<QString, QmlLibraryInfo::Ptr> &libinfos){
    m_libraryMutex.lock();
    for( QHash<QString, QmlLibraryInfo::Ptr>::const_iterator it = libinfos.begin(); it != libinfos.end(); ++it ){
        m_libraries[it.key()] = it.value();
    }
    m_libraryMutex.unlock();
}

inline int ProjectQmlScopeContainer::totalLibraries() const{
    return m_libraries.size();
}

inline QHash<QString, QmlLibraryInfo::Ptr> ProjectQmlScopeContainer::getNoInfoLibraries(){
    QHash<QString, QmlLibraryInfo::Ptr> libraries;
    m_libraryMutex.lock();

    for( QHash<QString, QmlLibraryInfo::Ptr>::const_iterator it = m_libraries.begin(); it != m_libraries.end(); ++it ){
        if ( it.value()->status() == QmlLibraryInfo::NotScanned ||
             it.value()->status() == QmlLibraryInfo::RequiresDependency ){

            libraries[it.key()] = it.value();
        }
    }

    m_libraryMutex.unlock();

    return libraries;
}

inline QHash<QString, QmlLibraryInfo::Ptr> ProjectQmlScopeContainer::getNoLinkLibraries(){
    QHash<QString, QmlLibraryInfo::Ptr> libraries;
    m_libraryMutex.lock();

    for( QHash<QString, QmlLibraryInfo::Ptr>::const_iterator it = m_libraries.begin(); it != m_libraries.end(); ++it ){
        if ( it.value()->status() == QmlLibraryInfo::NoPrototypeLink ){
            libraries[it.key()] = it.value();
        }
    }

    m_libraryMutex.unlock();

    return libraries;
}

inline void ProjectQmlScopeContainer::resetLibrariesInPath(const QString &path){
    m_libraryMutex.lock();
    for( QHash<QString, QmlLibraryInfo::Ptr>::iterator it = m_libraries.begin(); it != m_libraries.end(); ++it ){
        if ( it.key().startsWith(path) )
            it.value()->setStatus(QmlLibraryInfo::NotScanned);
    }
    m_libraryMutex.unlock();
}

inline void ProjectQmlScopeContainer::resetLibrary(const QString &path){
    m_libraryMutex.lock();
    if ( m_libraries.contains(path) )
        m_libraries[path]->setStatus(QmlLibraryInfo::NotScanned);
    m_libraryMutex.unlock();
}

}// namespace

#endif // LVPROJECTQMLSCOPECONTAINER_P_H
