#ifndef QPROJECTQMLSCOPE_P_H
#define QPROJECTQMLSCOPE_P_H

#include <QMutex>
#include <QMutexLocker>
#include "qqmllibraryinfo_p.h"

namespace lcv{

class QProjectQmlScopeContainer{

public:
    QProjectQmlScopeContainer(){}
    ~QProjectQmlScopeContainer(){}

    QQmlLibraryInfo::Ptr libraryInfo(const QString& path);
    bool libraryExists(const QString& path);
    void assignLibrary(const QString& path, QQmlLibraryInfo::Ptr libinfo);
    void assignLibraries(const QHash<QString, QQmlLibraryInfo::Ptr>& libinfos);
    int totalLibraries() const;

    QHash<QString, QQmlLibraryInfo::Ptr> getNoInfoLibraries();
    QHash<QString, QQmlLibraryInfo::Ptr> getNoLinkLibraries();

    void resetLibrariesInPath(const QString& path);
    void resetLibrary(const QString& path);

private:
    QMutex m_libraryMutex;
    QHash<QString, QQmlLibraryInfo::Ptr> m_libraries;
};

inline QQmlLibraryInfo::Ptr QProjectQmlScopeContainer::libraryInfo(const QString &path){
    QQmlLibraryInfo::Ptr libinfo(0);
    m_libraryMutex.lock();
    libinfo = m_libraries.value(path, QQmlLibraryInfo::Ptr(0));
    if ( libinfo.isNull() ){
        libinfo = QQmlLibraryInfo::create();
        m_libraries[path] = libinfo;
    }
    m_libraryMutex.unlock();
    return libinfo;
}

inline bool QProjectQmlScopeContainer::libraryExists(const QString &path){
    m_libraryMutex.lock();
    bool contains = m_libraries.contains(path);
    m_libraryMutex.unlock();
    return contains;
}

inline void QProjectQmlScopeContainer::assignLibrary(const QString &path, QQmlLibraryInfo::Ptr libinfo){
    m_libraryMutex.lock();
    m_libraries[path] = libinfo;
    m_libraryMutex.unlock();
}

inline void QProjectQmlScopeContainer::assignLibraries(const QHash<QString, QQmlLibraryInfo::Ptr> &libinfos){
    m_libraryMutex.lock();
    for( QHash<QString, QQmlLibraryInfo::Ptr>::const_iterator it = libinfos.begin(); it != libinfos.end(); ++it ){
        m_libraries[it.key()] = it.value();
    }
    m_libraryMutex.unlock();
}

inline int QProjectQmlScopeContainer::totalLibraries() const{
    return m_libraries.size();
}

inline QHash<QString, QQmlLibraryInfo::Ptr> QProjectQmlScopeContainer::getNoInfoLibraries(){
    QHash<QString, QQmlLibraryInfo::Ptr> libraries;
    m_libraryMutex.lock();

    for( QHash<QString, QQmlLibraryInfo::Ptr>::const_iterator it = m_libraries.begin(); it != m_libraries.end(); ++it ){
        if ( it.value()->status() == QQmlLibraryInfo::NotScanned ||
             it.value()->status() == QQmlLibraryInfo::RequiresDependency ){

            libraries[it.key()] = it.value();
        }
    }

    m_libraryMutex.unlock();

    return libraries;
}

inline QHash<QString, QQmlLibraryInfo::Ptr> QProjectQmlScopeContainer::getNoLinkLibraries(){
    QHash<QString, QQmlLibraryInfo::Ptr> libraries;
    m_libraryMutex.lock();

    for( QHash<QString, QQmlLibraryInfo::Ptr>::const_iterator it = m_libraries.begin(); it != m_libraries.end(); ++it ){
        if ( it.value()->status() == QQmlLibraryInfo::NoPrototypeLink ){
            libraries[it.key()] = it.value();
        }
    }

    m_libraryMutex.unlock();

    return libraries;
}

inline void QProjectQmlScopeContainer::resetLibrariesInPath(const QString &path){
    m_libraryMutex.lock();
    for( QHash<QString, QQmlLibraryInfo::Ptr>::iterator it = m_libraries.begin(); it != m_libraries.end(); ++it ){
        if ( it.key().startsWith(path) )
            it.value()->setStatus(QQmlLibraryInfo::NotScanned);
    }
    m_libraryMutex.unlock();
}

inline void QProjectQmlScopeContainer::resetLibrary(const QString &path){
    m_libraryMutex.lock();
    if ( m_libraries.contains(path) )
        m_libraries[path]->setStatus(QQmlLibraryInfo::NotScanned);
    m_libraryMutex.unlock();
}

}// namespace

#endif // QPROJECTQMLSCOPE_P_H
