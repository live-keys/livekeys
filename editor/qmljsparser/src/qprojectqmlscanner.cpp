#include "qprojectqmlscanner_p.h"
#include "qprojectqmlscopecontainer_p.h"
#include "qqmllibraryinfo_p.h"
#include "qdocumentqmlobject_p.h"
#include "qdocumentqmlscope.h"
#include "qlockedfileio.h"

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


//#define QDOCUMENT_QML_SCANNER_DEBUG_FLAG
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
        bool parseResult = documentInfo->parse(QLockedFileIO::instance().readFromFile(info.filePath()));
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
            if ( linfo->data().pluginTypeInfoStatus() == QmlJS::LibraryInfo::NoTypeInfo ){
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
            qDebug() << "Found super class name:" << fmo->superclassName() << "for" << fmo->className();
        } else {
            if ( librariesDumped || dependencyLoop ){ /// avoid dependency loop, simply add the object as is
                objects.append(fmo);
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

QQmlLibraryInfo::Ptr updateLibrary(
        QProjectQmlScope::Ptr projectScope,
        const QString& path,
        const QmlJS::LibraryInfo &libInfo,
        QList<QQmlLibraryDependency>& dependencies)
{
    QDOCUMENT_QML_SCANNER_DEBUG("Updating library: " + path);
    QQmlLibraryInfo::Ptr base = QQmlLibraryInfo::create();
    LibraryInfoSnapshot snapshot;

    QList<QString> dependencyPaths;

    const QDir dir(path);
    QFile dirFile(dir.filePath("qmldir"));
    if( !dirFile.exists() ){
        scanPathForQmlExports(projectScope, path, base);
        return base;
    }

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

    //

//    qDebug() << path << typeInfoPaths;
//    foreach( const QmlDirParser::Component& com, dirParser.components() ){
//        qDebug() << com.typeName << com.fileName << com.majorVersion << "." << com.minorVersion;
//    }

    //TODO: Block engine and create dump if necessary

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

        foreach( LanguageUtils::FakeMetaObject::ConstPtr fmo, newObjects ){
            snapshot.objects.append(fmo);
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
    }

    base->setDependencies(dependencyPaths);
    base->data().setDependencies(snapshot.dependencies);
    base->data().setMetaObjects(snapshot.objects);
    base->data().setModuleApis(snapshot.moduleApis);
    base->data().setPluginTypeInfoStatus(QmlJS::LibraryInfo::DumpDone);
    base->updateExports();


    return base;
}

} // namespace projectqml_helpers

QProjectQmlScanner::QProjectQmlScanner(QObject *parent)
    : QObject(parent)
    , m_project(0)
    , m_lastDocumentScope(0)
    , m_thread(new QThread)
    , m_timer(new QTimer)
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
    qDebug() << "Emmiting new project scan queue";
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

    QHash<QString, QQmlLibraryInfo::Ptr> implicitLibrariesSnapshot;
    QProjectQmlScopeContainer* implicitContainer = m_project->implicitLibraries();
    QHash<QString, QQmlLibraryInfo::Ptr> implicitLibraries = implicitContainer->getNoInfoLibraries();
    for( QHash<QString, QQmlLibraryInfo::Ptr>::const_iterator it = implicitLibraries.begin();
         it != implicitLibraries.end();
         ++it )
    {
        QQmlLibraryInfo::Ptr newLibInfo = QQmlLibraryInfo::create();
        projectqml_helpers::scanPathForQmlExports(m_project, it.key(), newLibInfo);
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
                QQmlLibraryInfo::Ptr libinfo = projectqml_helpers::updateLibrary(
                    m_project,
                    it.key(),
                    it.value()->data(),
                    dependencies
                );
                globalLibrariesSnapshot[it.key()] = libinfo;
                if( libinfo->data().pluginTypeInfoStatus() == QmlJS::LibraryInfo::NoTypeInfo )
                    unsolvedDependencies = true;
                reenter = true;
            }
        }

    }

    if ( globalLibrariesSnapshot.size() > 0 ){
        globalContainer->assignLibraries(globalLibrariesSnapshot);
        emit projectScopeReady();
        if ( unsolvedDependencies )
            scanProjectScope();
    } else if ( implicitLibrariesSnapshot.size() > 0 ){
        emit projectScopeReady();
    }

}
}// namespace
