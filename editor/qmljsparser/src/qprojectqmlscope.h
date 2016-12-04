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

class QProjectQmlScopePrivate;
class QProjectQmlScope{

    Q_DISABLE_COPY(QProjectQmlScope)

public:
    typedef QSharedPointer<const QProjectQmlScope> ConstPtr;
    typedef QSharedPointer<QProjectQmlScope>       Ptr;

public:
    ~QProjectQmlScope();

    void scan(const QString& file, const QString& source);

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

    void updateScope();
//    LibraryInfoSnapshot* updateLibrary(
//        const QString& path,
//        const QmlJS::LibraryInfo& libInfo,
//        QList<QQmlLibraryDependency>& depdendencies
//    );


    int totalLibraries() const;

    QProjectQmlScopePrivate* data();

private:

    QProjectQmlScope(QQmlEngine* engine);

private:
    QScopedPointer<QProjectQmlScopePrivate> d_ptr;

    QQmlEngine* m_engine;

    QSet<QString> m_monitoredPaths;
    QStringList m_defaultImportPaths;
};

inline QProjectQmlScopePrivate *QProjectQmlScope::data(){
    return d_ptr.data();
}

}// namespace


#endif // QLIVECVSCOPE_H
