/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#include "projectqmlscanner_p.h"
#include "projectqmlscopecontainer_p.h"
#include "qmllibraryinfo_p.h"
#include "documentqmlobject_p.h"
#include "documentqmlscope.h"
#include "live/lockedfileiosession.h"
#include "live/visuallog.h"
#include "codeqmlhandler.h"

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

#include "plugintypesfacade.h"
#include "qmllibrarydependency.h"

namespace lv{

namespace projectqml_helpers{

class LibraryInfoSnapshot{
public:
    QStringList dependencies;
    QList<QmlLibraryDependency> parsedDependencies;
    QList<LanguageUtils::FakeMetaObject::ConstPtr> objects;
    QList<QmlJS::ModuleApiInfo> moduleApis;
};

void updateLibraryPrototypes(
        ProjectQmlScope::Ptr projectScope,
        const QString &path,
        QmlLibraryInfo::Ptr library)
{
    Q_UNUSED(path);
    int missingDependencies = 0;
    foreach( const QString& dependency, library->dependencyPaths() ){
        QmlLibraryInfo::Ptr linfo = projectScope->globalLibraries()->libraryInfo(dependency);
        if ( linfo->status() == QmlLibraryInfo::NotScanned ){
            missingDependencies++;
        }
    }

    int incompletePrototypes = 0;
    foreach ( LanguageUtils::FakeMetaObject::ConstPtr fmo, library->data().metaObjects() ){
        if ( fmo->superclassName() == "" ){
            QString typeName = fmo->className();
            QmlLibraryInfo::ExportVersion ev = library->findExport(typeName);
            if ( !ev.isValid() ){
                foreach( const QString& pathIt, library->dependencyPaths() ){
                    QmlLibraryInfo::Ptr linfo = projectScope->globalLibraries()->libraryInfo(pathIt);
                    ev = linfo->findExport(typeName);
                    if ( ev.isValid() ){
                        break;
                    }
                }
            }
            if ( ev.isValid() )
                const_cast<LanguageUtils::FakeMetaObject*>(fmo.data())->setSuperclassName(ev.object->className());
            else
                incompletePrototypes++;
        }
    }

    if ( incompletePrototypes == 0 || missingDependencies == 0 )
        library->setStatus(QmlLibraryInfo::Done);

    vlog_debug("editqmljs-projectscanner",
        "Updated library prototypes: " + path + " Missing prototypes: " + QString::number(incompletePrototypes) +
        " Missing dependencies: " + QString::number(missingDependencies)
    );
}

QmlLibraryInfo::ScanStatus loadPluginInfo(
        ProjectQmlScope::Ptr projectScope,
        const QmlDirParser& dirParser,
        QMutex* engineMutex,
        const QString& path,
        ProjectQmlScanner* scanner,
        QStringList& dependencyPaths,
        QByteArray* stream
)
{
    QMutexLocker engineLocker(engineMutex);

    QObject* requestObject = scanner->requestObject(path);
    if ( !PluginTypesFacade::isModule(dirParser.typeNamespace()) && requestObject == 0 ){

        /// If module is not loaded, find the library import, and request to load the module by creating
        /// a component from the engine

        QString uriForPath = projectScope->uriForPath(path);

        if ( uriForPath.isEmpty() || scanner->requestErrorStatus(path) ){
            vlog_debug("editqmljs-projectscanner", "Library PluginInfo Scan Error: " + path + ", uri:" + uriForPath);
            return QmlLibraryInfo::ScanError;
        }

        scanner->addLoadRequest(ProjectQmlScanner::TypeLoadRequest(uriForPath, path));
        return QmlLibraryInfo::RequiresDependency;

    } else {

        /// If module is loaded, scan objects, solve dependencies, and output library

        QHash<QByteArray, QSet<const QQmlType *> > qmlTypesByCppName;
        QList<const QQmlType*> nsTypes;
        PluginTypesFacade::extractTypes(dirParser.typeNamespace(), 0, nsTypes, qmlTypesByCppName);

        QSet<const QMetaObject *> metaTypes;
        QList<const QMetaObject*> unknownTypes;
        QStringList dependencies;

        PluginTypesFacade::getTypeDependencies(
            dirParser.typeNamespace(),
            nsTypes,
            qmlTypesByCppName,
            metaTypes,
            unknownTypes,
            dependencies
        );

        /// Scan dependencies both from qmldir, and from scanned type

        dependencyPaths.clear();

        QHash<QString, QmlDirParser::Component> dirParserDependencies = dirParser.dependencies();
        QHash<QString, QmlDirParser::Component>::iterator dpdIt;
        for ( dpdIt = dirParserDependencies.begin(); dpdIt != dirParserDependencies.end(); ++dpdIt ){
            QStringList localDependencyPaths;
            projectScope->findQmlLibraryInImports(
                dpdIt->typeName.replace(".", "/"),
                dpdIt->majorVersion,
                dpdIt->minorVersion,
                localDependencyPaths
            );
            foreach( const QString& depPath, localDependencyPaths )
                if ( !dependencyPaths.contains(depPath) )
                    dependencyPaths.append(depPath);
        }

        foreach( QString typeDependency, dependencies ){
            QStringList localDependencyPaths;
            projectScope->findQmlLibraryInImports(
                typeDependency.replace(".", "/"),
                1,
                0,
                localDependencyPaths
            );
            foreach( const QString& depPath, localDependencyPaths )
                if ( !dependencyPaths.contains(depPath) )
                    dependencyPaths.append(depPath);
        }

        /// If we have unknown types, check dependencies, if all dependencies have been loaded
        /// couple loose objects to this library, otherwise its not clear wether loose objects
        /// are part of this library or not

        if ( unknownTypes.size() > 0 ){

            QList<QmlLibraryInfo::Ptr> dependentLibraries;
            foreach( const QString& dpath, dependencyPaths ){
                QmlLibraryInfo::Ptr linfo = projectScope->globalLibraries()->libraryInfo(dpath);
                if ( linfo->status() == QmlLibraryInfo::NotScanned){
                    vlog_debug("editqmljs-projectscanner", "QmlInfo for: " + path + " requires " + dpath);
                    return QmlLibraryInfo::RequiresDependency; /// try again when more libraries are populated
                }
                dependentLibraries.append(linfo);
            }

            /// Insert unknown types that are not present within dependent libraries

            foreach( const QMetaObject* obj, unknownTypes ){
                LanguageUtils::FakeMetaObject::ConstPtr fakeobj;
                foreach ( QmlLibraryInfo::Ptr deplib, dependentLibraries ){
                    fakeobj = deplib->findObjectByClassName(obj->className());
                    if ( !fakeobj.isNull() )
                        break;
                }
                if ( fakeobj.isNull() )
                    metaTypes.insert(obj);
            }
        }

        QList<QString> dependencyUris;
        for ( dpdIt = dirParserDependencies.begin(); dpdIt != dirParserDependencies.end(); ++dpdIt ){
            dependencyUris.append(
                dpdIt->typeName + " " +
                QString::number(dpdIt->majorVersion) + "." +
                QString::number(dpdIt->minorVersion)
            );
        }

        PluginTypesFacade::extractPluginInfo(metaTypes, qmlTypesByCppName, dependencyUris, stream);

        return QmlLibraryInfo::Done;
    }
}

void scanObjectFile(
    ProjectQmlScope::Ptr projectScope,
    const QString& libraryPath,
    const QString& componentName,
    const QString& filePath,
    const QString& fileData,
    const LanguageUtils::ComponentVersion& componentVersion,
    QList<QString>& dependencyPaths,
    QList<LanguageUtils::FakeMetaObject::ConstPtr>& objects
)
{
    DocumentQmlInfo::Ptr documentInfo = DocumentQmlInfo::create(filePath);
    bool parseResult = documentInfo->parse(fileData);
    if ( !parseResult )
        return;

    QList<DocumentQmlScope::Import> imports = DocumentQmlScope::extractImports(documentInfo);
    QList<QString> paths;
    foreach( const DocumentQmlScope::Import import, imports ){
        if (import.importType() == DocumentQmlScope::Import::Directory) {
            projectScope->findQmlLibraryInPath(
                import.path(),
                false,
                paths
            );
        }

        if (import.importType() == DocumentQmlScope::Import::Library) {
            if (!import.isVersionValid())
                continue;
            projectScope->findQmlLibraryInImports(
                import.path(),
                import.versionMajor(),
                import.versionMinor(),
                paths
            );
        }
        foreach( const QString& pathIt, paths ){
            if ( pathIt != libraryPath && !dependencyPaths.contains(pathIt) ){
                dependencyPaths.append(pathIt);
            }
        }
    }

    LanguageUtils::FakeMetaObject::Ptr fmo = metaObjectFromQmlObject(
        documentInfo->extractValueObject(documentInfo->rootObject())
    );
    fmo->addExport(componentName, libraryPath, componentVersion);
    objects.append(fmo);
}

void scanQmlDirForQmlExports(
        ProjectQmlScope::Ptr projectScope,
        LockedFileIOSession::Ptr lockedFileIO,
        const QmlDirParser& dirParser,
        const QString &path,
        QmlLibraryInfo::Ptr library
)
{
    QList<QString> dependencyPaths = library->dependencyPaths();
    QList<LanguageUtils::FakeMetaObject::ConstPtr> objects;
    vlog_debug("editqmljs-projectscanner", "Scanning qmldir components in: " + path);

    QHash<QString, QmlDirParser::Component> components = dirParser.components();

    int componentCount = 0; // eliminate "className" which is also treated as a component

    QHash<QString, QmlDirParser::Component>::iterator it;
    for ( it = components.begin(); it != components.end(); ++it ){
        if ( it.key() == "classname" )
            continue;

        QString filePath = QDir::cleanPath(path + QDir::separator() + it->fileName);
        if ( QFile::exists(filePath) ){
            scanObjectFile(
                projectScope,
                path,
                it->typeName,
                filePath,
                QString::fromStdString(lockedFileIO->readFromFile(filePath.toStdString())),
                LanguageUtils::ComponentVersion(it->majorVersion, it->minorVersion),
                dependencyPaths,
                objects
            );
        } else {
            qWarning("Warning: Qml file does not exist for parsing: %s", qPrintable(filePath));
        }

        ++componentCount;
    }
    if ( componentCount == 0 ){
        library->setStatus(QmlLibraryInfo::Done);
        return;
    }

    library->data().setMetaObjects(objects);
    library->setStatus(QmlLibraryInfo::NoPrototypeLink);
    library->setDependencies(dependencyPaths);
    library->updateExports();

    updateLibraryPrototypes(projectScope, path, library);
}

void scanPathForQmlExports(
        ProjectQmlScope::Ptr projectScope,
        LockedFileIOSession::Ptr lockedFileIO,
        const QString &path,
        QmlLibraryInfo::Ptr library)
{
    QList<QString> dependencyPaths = library->dependencyPaths();
    QList<LanguageUtils::FakeMetaObject::ConstPtr> objects;
    vlog_debug("editqmljs-projectscanner", "Scannig path: " + path);

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

        scanObjectFile(
            projectScope,
            path,
            info.baseName(),
            info.filePath(),
            QString::fromStdString(lockedFileIO->readFromFile(info.filePath().toStdString())),
            LanguageUtils::ComponentVersion(1, 0),
            dependencyPaths,
            objects
        );
    }

    library->data().setMetaObjects(objects);
    library->setStatus(QmlLibraryInfo::NoPrototypeLink);
    library->setDependencies(dependencyPaths);
    library->updateExports();

    updateLibraryPrototypes(projectScope, path, library);
}

QMap<QString, QmlLibraryInfo::Ptr> updateLibrary(
        ProjectQmlScope::Ptr projectScope,
        LockedFileIOSession::Ptr lockedFileIO,
        QMutex* engineMutex,
        const QString& path,
        const QmlJS::LibraryInfo &libInfo,
        ProjectQmlScanner* scanner,
        QList<QmlLibraryDependency>& dependencies)
{
    vlog_debug("editqmljs-projectscanner", "Updating library: " + path);
    QMap<QString, QmlLibraryInfo::Ptr> base;
    QmlLibraryInfo::Ptr baseLib = QmlLibraryInfo::create();
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
    dirParser.parse(QString::fromStdString(lockedFileIO->readFromFile(dir.filePath("qmldir").toStdString())));

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

        /// Check parent dependencies (some libraries do not have a plugins.qmlinfo file since their
        /// parent modules cover their types. We must make sure the parent libraries are populated
        /// before continuing).

        QStringList namespaceSegments = dirParser.typeNamespace().split('.');
        QString builtNamespace = "";
        QStringList parentDependencies;
        for ( int i = 0; i < namespaceSegments.size() - 1; ++i ){
            builtNamespace += (builtNamespace.isEmpty() ? namespaceSegments[i] : ("/" + namespaceSegments[i]));
            projectScope->findQmlLibraryInImports(builtNamespace, 0, 0, parentDependencies);
        }

        foreach( const QString& parentDependency, parentDependencies){
            QmlLibraryInfo::Ptr parentLibInfo = projectScope->globalLibraries()->libraryInfo(parentDependency);

            if (parentLibInfo->status() == QmlLibraryInfo::NotScanned ){
                return base;
            }
        }


        /// If library has no plugins, simply return the qml files

        if ( dirParser.plugins().size() == 0 ){
            scanQmlDirForQmlExports(projectScope, lockedFileIO, dirParser, path, baseLib);
            return base;
        } else if ( PluginTypesFacade::pluginTypesEnabled() ){

            QByteArray stream;
            QStringList dependencyPaths;

            QmlLibraryInfo::ScanStatus loadStatus = loadPluginInfo(
                projectScope,
                dirParser,
                engineMutex,
                path,
                scanner,
                dependencyPaths,
                &stream
            );
            baseLib->setDependencies(dependencyPaths);

            if ( loadStatus != QmlLibraryInfo::Done ){
                baseLib->setStatus(loadStatus);
                return base;
            } else {
                /// Read new stream

                vlog_debug("editqmljs-projectscanner", "Updating library from stream: " + path);

                QHash<QString, LanguageUtils::FakeMetaObject::ConstPtr> newObjects;
                QmlJS::TypeDescriptionReader reader(defaultQmltypesPath, stream);
                if ( !reader(&newObjects, &snapshot.moduleApis, &snapshot.dependencies) ){
                    qCritical("TypeInfo parse error: %s", qPrintable(reader.errorMessage()));
                }
                for ( QHash<QString, LanguageUtils::FakeMetaObject::ConstPtr>::iterator it = newObjects.begin();
                      it != newObjects.end();
                      ++it )
                {
                    snapshot.objects << it.value();
                }

                baseLib->data().setMetaObjects(snapshot.objects);
                baseLib->data().setModuleApis(snapshot.moduleApis);
                baseLib->setStatus(QmlLibraryInfo::Done);
                baseLib->updateExports();

                scanner->removeLoadRequest(path);
                scanner->updatePluginInfo(path, stream);

                return base;
            }
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
            QmlLibraryDependency parsedDependency = QmlLibraryDependency::parse(dependency);
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
            int highestImportMajor = 1, highestImportMinor = 0;

            foreach( const LanguageUtils::FakeMetaObject::ConstPtr& dfmo, it.value() ){
                foreach ( const LanguageUtils::FakeMetaObject::Export& dfmoexp, dfmo->exports() ){
                    if ( dfmoexp.version.majorVersion() > highestImportMajor ){
                        highestImportMajor = dfmoexp.version.majorVersion();
                        highestImportMinor = dfmoexp.version.minorVersion();
                    } else if ( dfmoexp.version.majorVersion() == highestImportMajor &&
                                dfmoexp.version.minorVersion() > highestImportMinor ){
                        highestImportMinor = dfmoexp.version.minorVersion();
                    }
                }
            }

            QString newLibraryImport = it.key();
            QList<QString> newLibraryPaths;
            projectScope->findQmlLibraryInImports(
                newLibraryImport.replace(".", "/"),
                highestImportMajor,
                highestImportMinor,
                newLibraryPaths
            );

            foreach( const QString& newLibraryPath, newLibraryPaths){
                QmlLibraryInfo::Ptr newLib = QmlLibraryInfo::create();
                newLib->setDependencies(dependencyPaths);
                newLib->data().setDependencies(snapshot.dependencies);
                newLib->data().setMetaObjects(it.value());
                newLib->data().setModuleApis(snapshot.moduleApis);
                newLib->setStatus(QmlLibraryInfo::Done);
                newLib->updateExports();
                base[newLibraryPath] = newLib;
            }
        }
    }

    scanQmlDirForQmlExports(projectScope, lockedFileIO, dirParser, path, baseLib);

    QStringList concatenatedDependencies = baseLib->data().dependencies() + snapshot.dependencies;
    QList<LanguageUtils::FakeMetaObject::ConstPtr> concatenatedObjects = baseLib->data().metaObjects() + snapshot.objects;

    baseLib->setDependencies(dependencyPaths);
    baseLib->data().setDependencies(concatenatedDependencies);
    baseLib->data().setMetaObjects(concatenatedObjects);
    baseLib->data().setModuleApis(snapshot.moduleApis);
    baseLib->updateExports();

    return base;
}

} // namespace projectqml_helpers

ProjectQmlScanner::ProjectQmlScanner(
        QQmlEngine* engine,
        QMutex* engineMutex,
        LockedFileIOSession::Ptr lockedFileIO,
        QObject *parent)
    : QObject(parent)
    , m_project(0)
    , m_lastDocumentScope(0)
    , m_lockedFileIO(lockedFileIO)
    , m_thread(new QThread)
    , m_timer(new QTimer)
    , m_engine(engine)
    , m_engineMutex(engineMutex)
{
    m_timer->setInterval(5000);
    m_timer->setSingleShot(false);

    this->moveToThread(m_thread);
    connect(this, SIGNAL(queueProjectScan()), this, SLOT(scanProjectScope()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(scanProjectScope()));

    connect(
        this, SIGNAL(queueDocumentScan(const QString&,const QString&,ProjectQmlScope*,CodeQmlHandler*)),
        this, SLOT(scanDocumentScope(const QString&,const QString&,ProjectQmlScope*,CodeQmlHandler*))
    );

    m_thread->start();
}

ProjectQmlScanner::~ProjectQmlScanner(){
    m_thread->exit();
    if ( !m_thread->wait(5000) ){
        qCritical("QmlScanner Thread failed to close, forcing quit. This may lead to inconsistent application state.");
        m_thread->terminate();
        m_thread->wait();
    }
    delete m_thread;
    delete m_timer;
}

void ProjectQmlScanner::setProjectScope(ProjectQmlScope::Ptr scope){
    m_project = scope;
    m_timer->start();
    emit queueProjectScan();
}

void ProjectQmlScanner::queueDocumentScopeScan(
        const QString &path,
        const QString &content,
        ProjectQmlScope* projectScope,
        CodeQmlHandler* codeHandler)
{
    emit queueDocumentScan(path, content, projectScope, codeHandler);
}

void ProjectQmlScanner::scanDocumentScope(
        const QString &path,
        const QString &content,
        ProjectQmlScope *projectScope,
        CodeQmlHandler *codeHandler)
{
    if ( m_project.data() != projectScope )
        return;

    ProjectQmlScanMonitor::DocumentQmlScopeTransport* travel = new ProjectQmlScanMonitor::DocumentQmlScopeTransport;
    travel->codeHandler = codeHandler;
    travel->documentScope = DocumentQmlScope::createScope(path, content, m_project);
    travel->path = path;
//    m_lastDocumentScope = DocumentQmlScope::createScope(path, content, m_project);


    emit documentScopeReady(travel);
}

void ProjectQmlScanner::scanProjectScope(){
    scanProjectScopeRecurse();
}

void ProjectQmlScanner::scanProjectScopeRecurse(int limit){
    if ( limit == 0 ){
        qWarning("Warning: Project scanner loop reached end of limit.");
        return;
    }
    ProjectQmlScope::Ptr projectScope = m_project;

    QHash<QString, QmlLibraryInfo::Ptr> implicitLibrariesSnapshot;
    ProjectQmlScopeContainer* implicitContainer = projectScope->implicitLibraries();
    QHash<QString, QmlLibraryInfo::Ptr> implicitLibraries = implicitContainer->getNoInfoLibraries();
    for( QHash<QString, QmlLibraryInfo::Ptr>::const_iterator it = implicitLibraries.begin();
         it != implicitLibraries.end();
         ++it )
    {
        QmlLibraryInfo::Ptr newLibInfo = QmlLibraryInfo::create();
        projectqml_helpers::scanPathForQmlExports(projectScope, m_lockedFileIO, it.key(), newLibInfo);
        implicitLibrariesSnapshot[it.key()] = newLibInfo;
    }
    if ( implicitLibrariesSnapshot.size() > 0 ){
        implicitContainer->assignLibraries(implicitLibrariesSnapshot);
    }

    QHash<QString, QmlLibraryInfo::Ptr> globalLibrariesSnapshot;
    ProjectQmlScopeContainer* globalContainer = projectScope->globalLibraries();

    bool reenter = true;
    bool unsolvedDependencies = false;
    while( reenter ){
        reenter = false;
        QHash<QString, QmlLibraryInfo::Ptr> libraries = globalContainer->getNoInfoLibraries();
        QList<QmlLibraryDependency> dependencies;

        for( QHash<QString, QmlLibraryInfo::Ptr>::const_iterator it = libraries.begin(); it != libraries.end(); ++it ){
            if ( !globalLibrariesSnapshot.contains(it.key()) ){
                QMap<QString, QmlLibraryInfo::Ptr> libinfos = projectqml_helpers::updateLibrary(
                    projectScope,
                    m_lockedFileIO,
                    m_engineMutex,
                    it.key(),
                    it.value()->data(),
                    this,
                    dependencies
                );

                for ( QMap<QString, QmlLibraryInfo::Ptr>::iterator liit = libinfos.begin(); liit != libinfos.end(); ++liit ){
                    globalLibrariesSnapshot[liit.key()] = liit.value();
                    if( liit.value()->status() == QmlLibraryInfo::NotScanned )
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
        else
            updatePrototypeList();
    } else if ( implicitLibrariesSnapshot.size() > 0 ){
        updatePrototypeList();
        emit projectScopeReady();
    }
}

void ProjectQmlScanner::updatePrototypeList(){

    ProjectQmlScopeContainer* implicitContainer = m_project->implicitLibraries();
    QHash<QString, QmlLibraryInfo::Ptr> implicitLibraries = implicitContainer->getNoLinkLibraries();
    for( QHash<QString, QmlLibraryInfo::Ptr>::const_iterator it = implicitLibraries.begin();
         it != implicitLibraries.end();
         ++it )
    {
        projectqml_helpers::updateLibraryPrototypes(m_project, it.key(), it.value());
    }

    ProjectQmlScopeContainer* globalContainer = m_project->globalLibraries();
    QHash<QString, QmlLibraryInfo::Ptr> libraries = globalContainer->getNoLinkLibraries();
    for( QHash<QString, QmlLibraryInfo::Ptr>::const_iterator it = libraries.begin();
         it != libraries.end();
         ++it )
    {
        projectqml_helpers::updateLibraryPrototypes(m_project, it.key(), it.value());
    }
}

void ProjectQmlScanner::addLoadRequest(const ProjectQmlScanner::TypeLoadRequest &request){
    if ( !hasRequest(request.libraryPath) ){
        m_loadRequests.append(request);
        emit requestObjectLoad(request.importUri);
    }
}

bool ProjectQmlScanner::requestErrorStatus(const QString &path){
    foreach( const ProjectQmlScanner::TypeLoadRequest& req, m_loadRequests ){
        if ( req.libraryPath == path ){
            return req.isError;
        }
    }
    return false;
}

QObject *ProjectQmlScanner::requestObject(const QString &path){
    foreach( const ProjectQmlScanner::TypeLoadRequest& req, m_loadRequests ){
        if ( req.libraryPath == path ){
            return req.object;
        }
    }
    return 0;
}

bool ProjectQmlScanner::hasRequest(const QString &path) const{
    foreach( const ProjectQmlScanner::TypeLoadRequest& req, m_loadRequests ){
        if ( req.libraryPath == path ){
            return true;
        }
    }
    return false;
}

bool ProjectQmlScanner::tryToExtractPluginInfo(const QString& path, QByteArray* stream){
    QmlDirParser dirParser;

    const QDir dir(path);
    QFile dirFile(dir.filePath("qmldir"));
    if( !dirFile.exists() ){
        qCritical("Expected qmldir file does not exist: %s", qPrintable(dir.filePath("qmldir")));
        return false;
    }

    dirParser.parse(QString::fromStdString(m_lockedFileIO->readFromFile(dir.filePath("qmldir").toStdString())));

    QStringList dependencyPaths;

    QmlLibraryInfo::ScanStatus status = projectqml_helpers::loadPluginInfo(
        m_project, dirParser, m_engineMutex, path, this, dependencyPaths, stream
    );
    return status == QmlLibraryInfo::Done;
}

void ProjectQmlScanner::updatePluginInfo(const QString &libraryPath, const QByteArray &libInfo){
    QFile f(QDir::cleanPath(libraryPath + QDir::separator() + "plugins.qmltypes"));
    if ( f.open(QIODevice::WriteOnly) ){
        f.write(libInfo);
        f.close();
    } else {
        qCritical("Failed to write plugininfo file: %s", qPrintable(f.fileName()));
    }
}

void ProjectQmlScanner::updateLoadRequest(const QString &uri, QObject *object, bool isError){
    for ( QList<ProjectQmlScanner::TypeLoadRequest>::iterator it = m_loadRequests.begin();
          it != m_loadRequests.end();
          ++it )
    {
        if ( it->importUri == uri ){
            it->isError = isError;
            it->object = object;
            break;
        }
    }
}

void ProjectQmlScanner::removeLoadRequest(const QString& path){
    for( int i = 0; i < m_loadRequests.size(); ++i ){
        if ( m_loadRequests[i].libraryPath == path ){
            m_loadRequests.removeAt(i);
            return;
        }
    }
}

}// namespace
