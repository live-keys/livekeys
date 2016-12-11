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
class QProjectQmlScope{

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

private:

    QProjectQmlScope(QQmlEngine* engine);

private:
    QScopedPointer<QProjectQmlScopeContainer> d_globalLibraries;
    QScopedPointer<QProjectQmlScopeContainer> d_implicitLibraries;

    QQmlEngine* m_engine;

    QSet<QString> m_monitoredPaths;
    QStringList m_defaultImportPaths;
};

inline QProjectQmlScopeContainer *QProjectQmlScope::globalLibraries(){
    return d_globalLibraries.data();
}

inline QProjectQmlScopeContainer *QProjectQmlScope::implicitLibraries(){
    return d_implicitLibraries.data();
}

}// namespace


#endif // QLIVECVSCOPE_H
