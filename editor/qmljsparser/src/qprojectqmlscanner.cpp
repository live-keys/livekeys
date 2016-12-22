#include "qprojectqmlscanner_p.h"
#include "qprojectqmlscopecontainer_p.h"
#include "qqmllibraryinfo_p.h"
#include "qdocumentqmlobject_p.h"
#include "qdocumentqmlscope.h"
#include "qlockedfileiosession.h"

#include "qmljs/qmljsdocument.h"
#include "qmljs/qmljsinterpreter.h"
#include "qmljs/qmljsbind.h"
#include "qmljs/qmljstypedescriptionreader.h"

#include "qmljs/qmljsdescribevalue.h"

#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QQmlEngine>

#include "qplugintypesfacade.h"


#define QDOCUMENT_QML_SCANNER_DEBUG_FLAG
#ifdef QDOCUMENT_QML_SCANNER_DEBUG_FLAG
#define QDOCUMENT_QML_SCANNER_DEBUG(_param) qDebug() << "QML SCANNER:" << (_param)
#else
#define QDOCUMENT_QML_SCANNER_DEBUG(_param)
#endif

namespace lcv{

namespace projectqml_helpers{

class LibraryInfoSnapshot{
public:
    QStringList dependencies;
    QList<QQmlLibraryDependency> parsedDependencies;
    QList<LanguageUtils::FakeMetaObject::ConstPtr> objects;
    QList<QmlJS::ModuleApiInfo> moduleApis;
};

//TODO: Check if components in qml dir (eg. Smth Smth.qml) are already parsed by the engine/plugindump
//TODO: Create parseable samples

void scanPathForQmlExports(
        QProjectQmlScope::Ptr projectScope,
        QLockedFileIOSession::Ptr lockedFileIO,
        const QString &path,
        QQmlLibraryInfo::Ptr library)
{
    QList<QString> dependencyPaths = library->dependencyPaths();
    QList<LanguageUtils::FakeMetaObject::ConstPtr> objects;
    QDOCUMENT_QML_SCANNER_DEBUG("Scannig path: " + path);

    QDirIterator dit(path);
    while( dit.hasNext() ){
        dit.next();
        QFileInfo info = dit.fileInfo();
        if ( info.fileName() == "." || info.fileName() == ".." || info.isDir() || info.fileName().isEmpty() )
            continue;
        if ( info.suffix() != "qml" )
            continue;
        if ( !info.fileName().at(0).isUpper() )
            continue;

        QDocumentQmlInfo::MutablePtr documentInfo = QDocumentQmlInfo::create(info.filePath());
        bool parseResult = documentInfo->parse(lockedFileIO->readFromFile(info.filePath()));
        if ( !parseResult )
            continue;

        QList<QDocumentQmlScope::Import> imports = QDocumentQmlScope::extractImports(documentInfo);
        QList<QString> paths;
        foreach( const QDocumentQmlScope::Import import, imports ){

            if (import.importType() == QmlJS::ImportType::Directory) {
                projectScope->findQmlLibraryInPath(
                    import.path(),
                    false,
                    paths
                );
            }

            if (import.importType() == QmlJS::ImportType::Library) {
                if (!import.isVersionValid())
                    continue;
                projectScope->findQmlLibraryInImports(
                    import.path(),
                    import.versionMajor(),
                    import.versionMinor(),
                    paths
                );
            }
            foreach( const QString& path, paths ){
                if ( !dependencyPaths.contains(path) )
                    dependencyPaths.append(path);
            }
        }


        LanguageUtils::FakeMetaObject::Ptr fmo = metaObjectFromQmlObject(
            documentInfo->extractValueObject(documentInfo->rootObject())
        );
        fmo->addExport(info.baseName(), path, LanguageUtils::ComponentVersion(1, 0));
        QString typeName = fmo->className();

        QQmlLibraryInfo::ExportVersion ev;
        bool librariesDumped = true;
        bool dependencyLoop = false;
        foreach( const QString& path, paths ){
            QQmlLibraryInfo::Ptr linfo = projectScope->globalLibraries()->libraryInfo(path);
            if ( linfo->data().pluginTypeInfoStatus() != QmlJS::LibraryInfo::DumpDone){
                librariesDumped = false;
                foreach( const QString& dependency, linfo->dependencyPaths() ){
                    if ( dependency == path )
                        dependencyLoop = true;
                }
            } else {
                ev = linfo->findExport(typeName);
                if ( ev.isValid() )
                    break;
            }
        }

        if ( ev.isValid() ){
            fmo->setSuperclassName(ev.object->className());
            objects.append(fmo);
        } else {
            if ( librariesDumped || dependencyLoop ){ /// avoid dependency loop, simply add the object as is
                objects.append(fmo);
                QDOCUMENT_QML_SCANNER_DEBUG("Dependency loop detected in: " + info.filePath());
            } else { /// try again when more libraries are populated
                return;
            }
        }
    }

    library->data().setMetaObjects(objects);
    library->data().setPluginTypeInfoStatus(QmlJS::LibraryInfo::DumpDone);
    library->setDependencies(dependencyPaths);
    library->updateExports();
}

QMap<QString, QQmlLibraryInfo::Ptr> updateLibrary(
        QProjectQmlScope::Ptr projectScope,
        QLockedFileIOSession::Ptr lockedFileIO,
        const QString& path,
        const QmlJS::LibraryInfo &libInfo,
        QList<QQmlLibraryDependency>& dependencies)
{
    QDOCUMENT_QML_SCANNER_DEBUG("Updating library: " + path);
    QMap<QString, QQmlLibraryInfo::Ptr> base;
    QQmlLibraryInfo::Ptr baseLib = QQmlLibraryInfo::create();
    base[path] = baseLib;

    LibraryInfoSnapshot snapshot;

    QList<QString> dependencyPaths;

    const QDir dir(path);
    QFile dirFile(dir.filePath("qmldir"));
    if( !dirFile.exists() ){
        scanPathForQmlExports(projectScope, lockedFileIO, path, baseLib);
        return base;
    }

    QmlDirParser dirParser;
    dirParser.parse(lockedFileIO->readFromFile(dir.filePath("qmldir")));

    /// Check parent dependencies (some libraries do not have a plugins.qmlinfo file since their
    /// parent modules cover their types. We must make sure the parent libraries are populated
    /// before continuing).

    QStringList namespaceSegments = dirParser.typeNamespace().split('.');
    QString builtNamespace = "";
    QStringList parentDependencies;
    for ( int i = 0; i < namespaceSegments.size() - 1; ++i ){
        builtNamespace += (builtNamespace.isEmpty() ? namespaceSegments[i] : ("." + namespaceSegments[i]));
        projectScope->findQmlLibraryInImports(builtNamespace, 0, 0, parentDependencies);
    }

    foreach( const QString& parentDependency, parentDependencies){
        QQmlLibraryInfo::Ptr parentLibInfo = projectScope->globalLibraries()->libraryInfo(parentDependency);

        if (parentLibInfo->data().pluginTypeInfoStatus() != QmlJS::LibraryInfo::DumpDone ){
            return base;
        }
    }

    /// Add typeinfopaths

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

    if ( typeInfoPaths.isEmpty() ){
        if ( dirParser.plugins().size() == 0 ){
            scanPathForQmlExports(projectScope, lockedFileIO, path, baseLib); //TODO, switch to qdirparser type
            return base;
        } else {
//        //TODO: Block engine and create dump if necessary

//            QHash<QByteArray, QSet<const QQmlType *> > qmlTypesByCppName;
//            QList<const QQmlType*> nsTypes;
//            QPluginTypesFacade::extractTypes(dirParser.typeNamespace(), 0, nsTypes, qmlTypesByCppName);

//            if ( nsTypes.size() == 0 ){
//                baseLib->data().setPluginTypeInfoStatus(QmlJS::LibraryInfo::TypeInfoFileError);
//                return base;
//            } else {
//                QList<const QMetaObject*> unknownTypes;
//                QStringList dependencies;

//                QPluginTypesFacade::getTypeDependencies(
//                    dirParser.typeNamespace(),
//                    nsTypes,
//                    qmlTypesByCppName,
//                    unknownTypes,
//                    dependencies
//                );

//                if ( unknownTypes.size() > 0 ){
//                    QStringList dependencyPaths;
//                    QStringList paths;
//                    //TODO: Merge with qmldir dependencies
//                    foreach( QString typeDependency, dependencies ){
//                        projectScope->findQmlLibraryInImports(
//                            typeDependency.replace(".", "/"),
//                            2,
//                            0,
//                            paths
//                        );
//                        foreach( const QString& depPath, paths ){
//                            if ( !dependencyPaths.contains(depPath) ){
//                                dependencyPaths.append(depPath);
//                            }
//                        }
//                    }

//                    QList<QQmlLibraryInfo::Ptr> dependentLibraries;
//                    foreach( const QString& path, paths ){
//                        QQmlLibraryInfo::Ptr linfo = projectScope->globalLibraries()->libraryInfo(path);
//                        if ( linfo->data().pluginTypeInfoStatus() != QmlJS::LibraryInfo::DumpDone){
//                            return base; /// try again when more libraries are populated
//                        }
//                        dependentLibraries.append(linfo);
//                    }


//                    foreach( const QMetaObject* obj, unknownTypes ){
//                        LanguageUtils::FakeMetaObject::ConstPtr fakeobj;
//                        foreach ( QQmlLibraryInfo::Ptr deplib, dependentLibraries ){
//                            fakeobj = deplib->findObjectByClassName(obj->className());
//                            if ( !fakeobj.isNull() )
//                                break;
//                        }
//                        if ( fakeobj.isNull() )
//                            qDebug() << "WILL FIND:" << obj->className();
//                    }

//                }
//            }
        }



    }


    foreach( const QString& typeInfoPath, typeInfoPaths ){

        QFile typeInfoFile(typeInfoPath);
        if ( !typeInfoFile.open(QIODevice::ReadOnly) ){
            qCritical("Cannot read plugin file: %s", qPrintable(typeInfoFile.fileName()));
            continue;
        }

        QHash<QString, LanguageUtils::FakeMetaObject::ConstPtr> newObjects;
        QmlJS::TypeDescriptionReader reader(typeInfoPath, typeInfoFile.readAll());
        if ( !reader(&newObjects, &snapshot.moduleApis, &snapshot.dependencies) ){
            qCritical("TypeInfo parse error: %s", qPrintable(reader.errorMessage()));
        }

        foreach( const QString& dependency, snapshot.dependencies){
            QQmlLibraryDependency parsedDependency = QQmlLibraryDependency::parse(dependency);
            if ( parsedDependency.isValid() ){
                projectScope->findQmlLibraryInImports(
                    parsedDependency.path(),
                    parsedDependency.versionMajor(),
                    parsedDependency.versionMinor(),
                    dependencyPaths
                );
                snapshot.parsedDependencies << parsedDependency;
                dependencies << parsedDependency;
            }
        }

        QMap<QString, QList<LanguageUtils::FakeMetaObject::ConstPtr> > newLibraries;
        for ( QHash<QString, LanguageUtils::FakeMetaObject::ConstPtr>::iterator it = newObjects.begin();
              it != newObjects.end();
              ++it )
        {
            QString typeModule = "";
            bool childModule = true;
            if ( it.value()->exports().size() > 0){
                foreach( LanguageUtils::FakeMetaObject::Export expt, it.value()->exports() ){

                    if ( !expt.package.startsWith(dirParser.typeNamespace()) && expt.package != "<cpp>" ){
                        childModule = false;
                        typeModule = "";
                    } else if ( expt.package.startsWith(dirParser.typeNamespace() ) && childModule ){
                        typeModule = expt.package;
                    } else if ( typeModule.isEmpty() && childModule ){
                        typeModule = expt.package;
                    }

                }
            } else {
                typeModule = "<cpp>";
            }

            if ( !typeModule.isEmpty() ){
                if ( typeModule == "<cpp>" || typeModule == dirParser.typeNamespace() ){
                    snapshot.objects.append(it.value());
                } else {
                    newLibraries[typeModule].append(it.value());
                }
            }
        }

        for ( QMap<QString, QList<LanguageUtils::FakeMetaObject::ConstPtr> >::iterator it = newLibraries.begin();
              it != newLibraries.end();
              ++it )
        {
//            qDebug() << "      --- NEW LIBRARY:" << it.key();
            //TODO: Add library
        }
    }


    baseLib->setDependencies(dependencyPaths);
    baseLib->data().setDependencies(snapshot.dependencies);
    baseLib->data().setMetaObjects(snapshot.objects);
    baseLib->data().setModuleApis(snapshot.moduleApis);
    baseLib->data().setPluginTypeInfoStatus(QmlJS::LibraryInfo::DumpDone);
    baseLib->updateExports();

    return base;
}

} // namespace projectqml_helpers

QProjectQmlScanner::QProjectQmlScanner(QLockedFileIOSession::Ptr lockedFileIO, QObject *parent)
    : QObject(parent)
    , m_project(0)
    , m_lastDocumentScope(0)
    , m_thread(new QThread)
    , m_timer(new QTimer)
    , m_lockedFileIO(lockedFileIO)
{
    m_timer->setInterval(5000);
    m_timer->setSingleShot(false);

    this->moveToThread(m_thread);
    connect(this, SIGNAL(queueProjectScan()), this, SLOT(scanProjectScope()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(scanProjectScope()));

    connect(
        this, SIGNAL(queueDocumentScan(const QString&,const QString&,QProjectQmlScope*)),
        this, SLOT(scanDocumentScope(const QString&,const QString&,QProjectQmlScope*))
    );

    m_thread->start();
}

QProjectQmlScanner::~QProjectQmlScanner(){
    m_thread->exit();
    if ( m_thread->wait(5000) ){
        qCritical("QmlScanner Thread failed to close, forcing quit. This may lead to inconsistent application state.");
        m_thread->terminate();
        m_thread->wait();
    }
    delete m_thread;
}

void QProjectQmlScanner::setProjectScope(QProjectQmlScope::Ptr scope){
    m_project = scope;
    m_timer->start();
    emit queueProjectScan();
}

void QProjectQmlScanner::queueDocumentScopeScan(
        const QString &path,
        const QString &content,
        QProjectQmlScope* projectScope)
{
    emit queueDocumentScan(path, content, projectScope);
}

void QProjectQmlScanner::scanDocumentScope(
        const QString &path,
        const QString &content,
        QProjectQmlScope *projectScope)
{
    if ( m_project.data() != projectScope )
        return;
    m_lastDocumentScope = QDocumentQmlScope::createScope(path, content, m_project);
    emit documentScopeReady();
}

void QProjectQmlScanner::scanProjectScope(){
    scanProjectScopeRecurse();
}

void QProjectQmlScanner::scanProjectScopeRecurse(int limit){
    if ( limit == 0 ){
        qWarning("Warning: Project scanner loop reaching end of limit.");
        return;
    }

    QHash<QString, QQmlLibraryInfo::Ptr> implicitLibrariesSnapshot;
    QProjectQmlScopeContainer* implicitContainer = m_project->implicitLibraries();
    QHash<QString, QQmlLibraryInfo::Ptr> implicitLibraries = implicitContainer->getNoInfoLibraries();
    for( QHash<QString, QQmlLibraryInfo::Ptr>::const_iterator it = implicitLibraries.begin();
         it != implicitLibraries.end();
         ++it )
    {
        QQmlLibraryInfo::Ptr newLibInfo = QQmlLibraryInfo::create();
        projectqml_helpers::scanPathForQmlExports(m_project, m_lockedFileIO, it.key(), newLibInfo);
        implicitLibrariesSnapshot[it.key()] = newLibInfo;
    }
    if ( implicitLibrariesSnapshot.size() > 0 ){
        implicitContainer->assignLibraries(implicitLibrariesSnapshot);
    }

    QHash<QString, QQmlLibraryInfo::Ptr> globalLibrariesSnapshot;
    QProjectQmlScopeContainer* globalContainer = m_project->globalLibraries();

    bool reenter = true;
    bool unsolvedDependencies = false;
    while( reenter ){
        reenter = false;
        QHash<QString, QQmlLibraryInfo::Ptr> libraries = globalContainer->getNoInfoLibraries();
        QList<QQmlLibraryDependency> dependencies;

        for( QHash<QString, QQmlLibraryInfo::Ptr>::const_iterator it = libraries.begin(); it != libraries.end(); ++it ){
            if ( !globalLibrariesSnapshot.contains(it.key()) ){
                QMap<QString, QQmlLibraryInfo::Ptr> libinfos = projectqml_helpers::updateLibrary(
                    m_project,
                    m_lockedFileIO,
                    it.key(),
                    it.value()->data(),
                    dependencies
                );

                for ( QMap<QString, QQmlLibraryInfo::Ptr>::iterator liit = libinfos.begin(); liit != libinfos.end(); ++liit ){
                    globalLibrariesSnapshot[liit.key()] = liit.value();
                    if( liit.value()->data().pluginTypeInfoStatus() == QmlJS::LibraryInfo::NoTypeInfo )
                        unsolvedDependencies = true;
                }

                reenter = true;
            }
        }

    }

    if ( globalLibrariesSnapshot.size() > 0 ){
        globalContainer->assignLibraries(globalLibrariesSnapshot);
        emit projectScopeReady();
        if ( unsolvedDependencies )
            scanProjectScopeRecurse(--limit);
    } else if ( implicitLibrariesSnapshot.size() > 0 ){
        emit projectScopeReady();
    }
}

}// namespace
