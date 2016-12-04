#include "qprojectqmlscope.h"
#include "qprojectqmlscope_p.h"
#include "qqmllibraryinfo_p.h"

#include "qmljs/qmljsdocument.h"
#include "qmljs/qmljsinterpreter.h"
#include "qmljs/qmljsbind.h"
#include "qmljs/qmljstypedescriptionreader.h"

#include "qmljs/qmljsdescribevalue.h"

#include <QDir>
#include <QQmlEngine>

namespace lcv{


class LibraryInfoSnapshot{
public:
    QStringList dependencies;
    QList<QQmlLibraryDependency> parsedDependencies;
    QList<LanguageUtils::FakeMetaObject::ConstPtr> objects;
    QList<QmlJS::ModuleApiInfo> moduleApis;
};

namespace projectqml_helpers{

QQmlLibraryInfo::Ptr updateLibrary(
        const QString& path,
        const QmlJS::LibraryInfo &libInfo,
        QList<QQmlLibraryDependency>& dependencies)
{
    QQmlLibraryInfo::Ptr base = QQmlLibraryInfo::create();
    LibraryInfoSnapshot snapshot;

    const QDir dir(path);
    QFile dirFile(dir.filePath("qmldir"));
    if( !dirFile.exists() ){
//        qDebug() << "Dir file exit:" << dir.path();
        //TODO: Parse files manually
        return base;
    }

//    qDebug() << "DirFile:" << dirFile.fileName();

    dirFile.open(QFile::ReadOnly);

    QmlDirParser dirParser;
    dirParser.parse(QString::fromUtf8(dirFile.readAll()));

    QStringList typeInfoPaths;

    const QLatin1String defaultQmltypesFileName("plugins.qmltypes");
    const QString defaultQmltypesPath = QDir::cleanPath(path + QDir::separator() + defaultQmltypesFileName);
    if ( QFile::exists(defaultQmltypesPath) && !typeInfoPaths.contains(defaultQmltypesPath) )
        typeInfoPaths += defaultQmltypesPath;

    foreach (const QmlDirParser::TypeInfo &typeInfo, libInfo.typeInfos()) {
        QString fullPath = QDir::cleanPath(path + QDir::separator() + typeInfo.fileName);
        if (!typeInfoPaths.contains(fullPath) && QFile::exists(fullPath))
            typeInfoPaths += fullPath;
    }

    //TODO: Block engine and create dump if necessary

    foreach( const QString& typeInfoPath, typeInfoPaths ){

        QFile typeInfoFile(typeInfoPath);
        if ( !typeInfoFile.open(QIODevice::ReadOnly) ){
            qDebug() << "Cannot read plugin file:" << typeInfoFile.fileName();
            continue;
        }

        QHash<QString, LanguageUtils::FakeMetaObject::ConstPtr> newObjects;
        QmlJS::TypeDescriptionReader reader(typeInfoPath, typeInfoFile.readAll());
        if ( !reader(&newObjects, &snapshot.moduleApis, &snapshot.dependencies) ){
            qDebug() << "Some error happened" << reader.errorMessage();
        }

        foreach( LanguageUtils::FakeMetaObject::ConstPtr fmo, newObjects ){
            snapshot.objects.append(fmo);
        }

        foreach( const QString& dependency, snapshot.dependencies){
            QQmlLibraryDependency parsedDependency = QQmlLibraryDependency::parse(dependency);
            if ( parsedDependency.isValid() ){
                snapshot.parsedDependencies << parsedDependency;
                dependencies << parsedDependency;
            }
        }
    }

    base->data().setDependencies(snapshot.dependencies);
    base->data().setMetaObjects(snapshot.objects);
    base->data().setModuleApis(snapshot.moduleApis);
    base->data().setPluginTypeInfoStatus(QmlJS::LibraryInfo::DumpDone);
    base->updateExports();


    return base;
}

}


QProjectQmlScope::QProjectQmlScope(QQmlEngine *engine)
    : d_ptr(new QProjectQmlScopePrivate)
    , m_engine(engine)
    , m_defaultImportPaths(engine->importPathList())
{
}

QProjectQmlScope::~QProjectQmlScope(){

}

void QProjectQmlScope::scan(const QString &file, const QString &source){
    QmlJS::Document::MutablePtr document = QmlJS::Document::create(file, QmlJS::Dialect::Qml);
    document->setSource(source);
    document->parseQml();

    qDebug() << document->bind()->rootObjectValue();
    qDebug() << QmlJS::DescribeValueVisitor::describe(document->bind()->rootObjectValue());

    QmlJS::Bind* b = document->bind();
    qDebug() << b->rootObjectValue()->className();

    qDebug() << "Component name:" << document->componentName();

    return;
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
    if ( d_ptr->libraryExists(path) ){
        paths.append(path);
        return;
    }

    const QDir dir(path);
    if ( !dir.exists() )
        return;

    QFile dirFile(dir.filePath("qmldir"));
    if( !dirFile.exists() ){
        if ( !requiresQmlDir ){
            d_ptr->assignLibrary(path, QQmlLibraryInfo::create());
            paths.append(path);
        }
        return;
    }

    dirFile.open(QFile::ReadOnly);

    QmlDirParser dirParser;
    dirParser.parse(QString::fromUtf8(dirFile.readAll()));

    d_ptr->assignLibrary(path, QQmlLibraryInfo::create(dirParser));
    paths.append(path);
}


//TODO: Check if components in qml dir (eg. Smth Smth.qml) are already parsed by the engine/plugindump or do I
// have to parse them manually



void QProjectQmlScope::updateScope(){
    QHash<QString, QQmlLibraryInfo::Ptr> snapshot;
    QMap<QString, QString> dependencyMap;
    QMap<QString, QList<QQmlLibraryDependency> > dependencySnaps;

    bool reenter = true;
    while( reenter ){
        reenter = false;
        QHash<QString, QQmlLibraryInfo::Ptr> libraries = d_ptr->getNoInfoLibraries();
        QList<QQmlLibraryDependency> dependencies;

        for( QHash<QString, QQmlLibraryInfo::Ptr>::const_iterator it = libraries.begin(); it != libraries.end(); ++it ){
            if ( !snapshot.contains(it.key()) ){
                snapshot[it.key()] = projectqml_helpers::updateLibrary(it.key(), it.value()->data(), dependencies);
                dependencySnaps[it.key()] = dependencies;
            }
        }

        foreach ( const QQmlLibraryDependency& dependency, dependencies ){
            QList<QString> paths;
            findQmlLibraryInImports(dependency.path(), dependency.versionMajor(), dependency.versionMinor(), paths);
            if ( paths.size() > 0 ){
                dependencyMap[dependency.toString()] = paths[0];
                reenter = true;
            }
        }
    }

    for( QHash<QString, QQmlLibraryInfo::Ptr>::const_iterator it = snapshot.begin(); it != snapshot.end(); ++it ){
        QStringList dependencyPaths;
        if ( dependencySnaps.contains(it.key() ) ){
            foreach( const QQmlLibraryDependency& dep, dependencySnaps[it.key()] ){
                dependencyPaths << dependencyMap[dep.toString()];
            }
            (*it)->setDependencies(dependencyPaths);
        }
    }

    d_ptr->assignLibraries(snapshot);

}

int QProjectQmlScope::totalLibraries() const{
    return d_ptr->totalLibraries();
}

}// namespace




