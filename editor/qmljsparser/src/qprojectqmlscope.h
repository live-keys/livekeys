#ifndef QLIVECVSCOPE_H
#define QLIVECVSCOPE_H

#include <QSet>
#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QSharedPointer>

#include "qqmljsparserglobal.h"
#include "qqmllibrarydependency.h"

class QQmlEngine;
namespace QmlJS{ class LibraryInfo; }


namespace lcv{

class QProjectQmlScopeContainer;
class Q_QMLJSPARSER_EXPORT QProjectQmlScope{

    Q_DISABLE_COPY(QProjectQmlScope)

public:
    typedef QSharedPointer<const QProjectQmlScope> ConstPtr;
    typedef QSharedPointer<QProjectQmlScope>       Ptr;

public:
    ~QProjectQmlScope();

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

    QProjectQmlScopeContainer* globalLibraries();
    QProjectQmlScopeContainer* implicitLibraries();

    QList<QString> pathsForImport(const QString& importUri);
    QString uriForPath(const QString& path);

    void addDefaultLibraries(const QList<QString>& paths);
    const QList<QString>& defaultLibraries() const;

private:
    QProjectQmlScope(QQmlEngine* engine);

private:
    QScopedPointer<QProjectQmlScopeContainer> d_globalLibraries;
    QScopedPointer<QProjectQmlScopeContainer> d_implicitLibraries;

    QList<QString> m_defaultLibraries;

    QQmlEngine* m_engine;

    QHash<QString, QList<QString> > m_importToPaths;

    QSet<QString> m_monitoredPaths;
    QStringList m_defaultImportPaths;
};

inline QProjectQmlScopeContainer *QProjectQmlScope::globalLibraries(){
    return d_globalLibraries.data();
}

inline QProjectQmlScopeContainer *QProjectQmlScope::implicitLibraries(){
    return d_implicitLibraries.data();
}

inline QList<QString> QProjectQmlScope::pathsForImport(const QString &importUri){
    if ( m_importToPaths.contains(importUri) )
        return m_importToPaths.value(importUri);
    return QList<QString>();
}

inline void QProjectQmlScope::addDefaultLibraries(const QList<QString> &paths){
    foreach( const QString& path, paths ){
        if ( !m_defaultLibraries.contains(path) )
            m_defaultLibraries.append(path);
    }
}

inline const QList<QString> &QProjectQmlScope::defaultLibraries() const{
    return m_defaultLibraries;
}

}// namespace


#endif // QLIVECVSCOPE_H
