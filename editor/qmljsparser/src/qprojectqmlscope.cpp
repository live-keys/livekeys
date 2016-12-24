#include "qprojectqmlscope.h"
#include "qprojectqmlscopecontainer_p.h"
#include "qqmllibraryinfo_p.h"

#include "qmljs/qmljsdocument.h"
#include "qmljs/qmljsinterpreter.h"
#include "qmljs/qmljsbind.h"
#include "qmljs/qmljstypedescriptionreader.h"

#include "qmljs/qmljsdescribevalue.h"

#include <QDir>
#include <QQmlEngine>

namespace lcv{


QProjectQmlScope::QProjectQmlScope(QQmlEngine *engine)
    : d_globalLibraries(new QProjectQmlScopeContainer)
    , d_implicitLibraries(new QProjectQmlScopeContainer)
    , m_engine(engine)
    , m_defaultImportPaths(engine->importPathList())
{
}

QProjectQmlScope::~QProjectQmlScope(){
}

QProjectQmlScope::Ptr QProjectQmlScope::create(QQmlEngine *engine){
    return QProjectQmlScope::Ptr(new QProjectQmlScope(engine));
}

void QProjectQmlScope::findQmlLibraryInImports(
        const QString &path,
        int versionMajor,
        int versionMinor,
        QList<QString> &paths)
{
    foreach( const QString& importPath, m_defaultImportPaths ){
        findQmlLibrary(
            QDir::cleanPath(importPath + QDir::separator() + path),
            versionMajor,
            versionMinor,
            paths
        );
    }
}

void QProjectQmlScope::findQmlLibrary(
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

void QProjectQmlScope::findQmlLibraryInPath(const QString &path, bool requiresQmlDir, QList<QString>& paths){
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
            d_globalLibraries->assignLibrary(path, QQmlLibraryInfo::create());
            paths.append(path);
        }
        return;
    }

    dirFile.open(QFile::ReadOnly);

    QmlDirParser dirParser;
    dirParser.parse(QString::fromUtf8(dirFile.readAll()));

    d_globalLibraries->assignLibrary(path, QQmlLibraryInfo::create(dirParser));
    paths.append(path);
}

void QProjectQmlScope::addImplicitLibrary(const QString &path){
    if ( !d_implicitLibraries->libraryExists(path) )
        d_implicitLibraries->assignLibrary(path, QQmlLibraryInfo::create());
}

int QProjectQmlScope::totalLibraries() const{
    return d_globalLibraries->totalLibraries();
}

int QProjectQmlScope::totalImplicitLibraries() const{
    return d_implicitLibraries->totalLibraries();
}

}// namespace




