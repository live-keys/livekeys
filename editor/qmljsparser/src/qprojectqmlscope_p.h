#ifndef QPROJECTQMLSCOPE_P_H
#define QPROJECTQMLSCOPE_P_H

#include <QMutex>
#include <QMutexLocker>
#include "qqmllibraryinfo_p.h"

namespace lcv{


class QProjectQmlScopePrivate{

public:
    QProjectQmlScopePrivate(){}
    ~QProjectQmlScopePrivate(){}

    QQmlLibraryInfo::Ptr libraryInfo(const QString& path);
    bool libraryExists(const QString& path);
    void assignLibrary(const QString& path, QQmlLibraryInfo::Ptr libinfo);
    void assignLibraries(const QHash<QString, QQmlLibraryInfo::Ptr>& libinfos);
    int totalLibraries() const;

    QHash<QString, QQmlLibraryInfo::Ptr> getNoInfoLibraries();

private:
    QMutex m_libraryMutex;
    QHash<QString, QQmlLibraryInfo::Ptr> m_libraries;
};

inline QQmlLibraryInfo::Ptr QProjectQmlScopePrivate::libraryInfo(const QString &path){
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

inline bool QProjectQmlScopePrivate::libraryExists(const QString &path){
    m_libraryMutex.lock();
    bool contains = m_libraries.contains(path);
    m_libraryMutex.unlock();
    return contains;
}

inline void QProjectQmlScopePrivate::assignLibrary(const QString &path, QQmlLibraryInfo::Ptr libinfo){
    m_libraryMutex.lock();
    m_libraries[path] = libinfo;
    m_libraryMutex.unlock();
}

inline void QProjectQmlScopePrivate::assignLibraries(const QHash<QString, QQmlLibraryInfo::Ptr> &libinfos){
    m_libraryMutex.lock();
    for( QHash<QString, QQmlLibraryInfo::Ptr>::const_iterator it = libinfos.begin(); it != libinfos.end(); ++it ){
        m_libraries[it.key()] = it.value();
    }
    m_libraryMutex.unlock();
}

inline int QProjectQmlScopePrivate::totalLibraries() const{
    return m_libraries.size();
}

inline QHash<QString, QQmlLibraryInfo::Ptr> QProjectQmlScopePrivate::getNoInfoLibraries(){
    QHash<QString, QQmlLibraryInfo::Ptr> libraries;
    m_libraryMutex.lock();

    for( QHash<QString, QQmlLibraryInfo::Ptr>::const_iterator it = m_libraries.begin(); it != m_libraries.end(); ++it ){
        if ( it.value()->data().pluginTypeInfoStatus() == QmlJS::LibraryInfo::NoTypeInfo )
            libraries[it.key()] = it.value();
    }

    m_libraryMutex.unlock();

    return libraries;
}

}// namespace

#endif // QPROJECTQMLSCOPE_P_H
