#include "qmllanguagescanner.h"
#include "live/visuallogqt.h"
#include "live/lockedfileiosession.h"
#include "live/applicationcontext.h"

#include "qmljs/qmljsdocument.h"
#include "qmljs/qmljsinterpreter.h"
#include "qmljs/qmljsbind.h"
#include "qmljs/qmljstypedescriptionreader.h"
#include "qmljs/qmljsdescribevalue.h"

#include <QDir>
#include <QDirIterator>
#include <QQmlEngine>
#include <QThread>
#include <QProcess>

#include "qmllanguageinfo_p.h"
#include "plugintypesfacade.h"
#include "qmllibrarydependency.h"
#include "qmlprojectmonitor_p.h"

namespace lv{

QmlLanguageScanner::QmlLanguageScanner(LockedFileIOSession::Ptr lio, const QStringList &importPaths, QObject *parent)
    : QObject(parent)
    , m_defaultImportPaths(importPaths)
    , m_ioSession(lio)
    , m_isProcessing(false)
    , m_stopRequest(false)
    , m_forkEnabled(true)
{
}

QmlLanguageScanner::~QmlLanguageScanner(){
}

/**
  Processes any queued libraries.
  For each library:
    If the library hasn't been scanned yet, scan it's exports and populate all
    type infos. Each dependency added will be pushed in front of the queue for
    scanning if it hasn't been scanned already. Library will be pushed at the end
    of the queue
    Note: In qml, some libraries do not have a plugin.qmlinfo file, but their parents
    do, and provide types for those libraries, so we will need to scan those types
    first of all.

    If the library has been scanned, update it's prototypes through it's dependencies
    These are of 2 types:
       * Documents [will have to scan imports]
       * Declared types through cpp
  */
void QmlLanguageScanner::processQueue(){
    m_isProcessing = true;
    emit isProcessingChanged(true);

    QLinkedList<QmlLibraryInfo::Ptr> queue;

    m_queueMutex.lock();
    for ( auto it = m_queue.begin(); it != m_queue.end(); ++it ){
        queue.append(*it);
    }
    m_queue.clear();
    m_queueMutex.unlock();

    while ( !queue.isEmpty() && !m_stopRequest ){
        QmlLibraryInfo::Ptr lib = queue.front();
        if ( m_libraries.contains(lib->uri()) ){
            lib = m_libraries[lib->uri()];
        }
        queue.removeFirst();

        if ( lib->status() == QmlLibraryInfo::NotScanned ){

            // check if parent has been populated
            if ( lib->uri().contains(".") && lib->uri() != lib->path() ){
                QString parentLib = lib->uri().mid(0, lib->uri().lastIndexOf('.'));
                auto foundLib = m_libraries.find(parentLib);
                if ( foundLib == m_libraries.end() ){

                    QDir dir(lib->path());
                    if ( dir.cdUp() ){
                        QmlLibraryInfo::Ptr plib = QmlLibraryInfo::create(parentLib);
                        plib->setPath(dir.path());
                        insertNewLibrary(plib);
                        queue.append(plib);
                        queue.append(lib);
                        continue;
                    }

                } else if ( foundLib.value()->status() == QmlLibraryInfo::NotScanned ){
                    queue.append(foundLib.value());
                    queue.append(lib);
                    continue;
                }
            }

            QList<QmlTypeInfo::Ptr> externalTypes = scanLibrary(lib);
            if ( externalTypes.size() ){
                for ( const QmlTypeInfo::Ptr& eti : externalTypes ){
                    QString libPath = eti->prefereredType().path();

                    QmlLibraryInfo::Ptr typeLib;

                    auto foundLib = m_libraries.find(libPath);
                    if ( foundLib == m_libraries.end() ){
                        typeLib = ProjectQmlScope::findQmlLibraryInImports(
                            m_defaultImportPaths, libPath, lib->importVersionMajor(), lib->importVersionMinor()
                        );
//                        insertNewLibrary(typeLib);
                    } else {
                        typeLib = foundLib.value();
                    }

                    if ( typeLib ){
                        typeLib->addType(eti);
                    }
                }
            }

            for ( auto it = lib->dependencies().begin(); it != lib->dependencies().end(); ++it ){
                if ( m_libraries.contains(*it) ){
                    queue.append(m_libraries[*it]);
                }
            }
            lib->setStatus(QmlLibraryInfo::NoPrototypeLink);
            insertNewLibrary(lib);
            queue.append(lib);

            if ( m_updateListener ){
                m_updateListener(lib, queue.size());
            }

        } else if ( lib->status() == QmlLibraryInfo::NoPrototypeLink ){
            updateLibraryUnknownTypes(lib);
            lib->setStatus(QmlLibraryInfo::Done);
            queue.append(lib);
            if ( m_updateListener ){
                m_updateListener(lib, queue.size());
            }
        }
    }

    if ( m_queueFinished )
        m_queueFinished();

    m_isProcessing = false;
    emit isProcessingChanged(false);
}

void QmlLanguageScanner::scanDocument(const QString &path, const QString &content, CodeQmlHandler *handler){
    DocumentQmlInfo::Ptr docinfo = DocumentQmlInfo::create(path);
    docinfo->parse(content);
    docinfo->createRanges();

    QmlLibraryInfo::Ptr lib = ProjectQmlScope::findQmlLibraryInPath(docinfo->path(), docinfo->path(), false);
    if ( lib ){
        queueLibrary(lib);
    }

    const DocumentQmlInfo::ImportList& imports = docinfo->imports();
    for( const DocumentQmlInfo::Import& import: imports ){
        if (import.importType() == DocumentQmlInfo::Import::Directory) {
            QmlLibraryInfo::Ptr foundLib = ProjectQmlScope::findQmlLibraryInPath(import.uri(), import.uri(), false);
            if ( foundLib ){
                queueLibrary(foundLib);
            } else {
                docinfo->updateImportType(import.uri(), DocumentQmlInfo::Import::Invalid);
            }
        }
        if (import.importType() == DocumentQmlInfo::Import::Library) {
            if (!import.isVersionValid())
                continue;
            QmlLibraryInfo::Ptr foundLib = ProjectQmlScope::findQmlLibraryInImports(
                m_defaultImportPaths,
                import.uri(),
                import.versionMajor(),
                import.versionMinor()
            );

            if ( foundLib ){
                queueLibrary(foundLib);
            } else {
                docinfo->updateImportType(import.uri(), DocumentQmlInfo::Import::Invalid);
            }
        }
    }

    QmlLanguageScanner::DocumentTransport* transport = new QmlLanguageScanner::DocumentTransport;
    transport->codeHandler = handler;
    transport->documentInfo = docinfo;

    emit documentScanReady(transport);
}

void QmlLanguageScanner::queueLibrary(const QmlLibraryInfo::Ptr &lib){
    m_queueMutex.lock();

    QmlLibraryInfo::Ptr copy = QmlLibraryInfo::create(lib->uri());
    copy->setPath(lib->path());
    m_queue.append(copy);

    m_queueMutex.unlock();
}

void QmlLanguageScanner::onMessage(std::function<void(int, const QString &)> listener){
    m_messageListener = listener;
}

/// In qml some libraries do not have a plugins.qmlinfo file since their
/// parent modules cover their types. In case we find some of those types, we will
/// have to populate those libraries as well.
QList<QmlTypeInfo::Ptr> QmlLanguageScanner::scanLibrary(const QmlLibraryInfo::Ptr &lib){
    QList<QmlTypeInfo::Ptr> result;

    vlog_debug("editqmljs-scanner", "Updating library: " + lib->path());

    // Case 1: dir file doesn't exist

    QDir dir(lib->path());
    QFile dirFile(dir.filePath("qmldir"));
    if( !dirFile.exists() ){

        vlog_debug("editqmljs-scanner", "Scanning path exports: " + lib->path());

        scanPathForExports(lib->path(), lib);
        return result;
    }

    // Case 2: dir file exists

    QmlDirParser dirParser;
    dirParser.parse(QString::fromStdString(m_ioSession->readFromFile(dir.filePath("qmldir").toStdString())));

    // Case 2.a: Dir file and plugins.qmltypes exists

    QStringList typeInfoPaths;

    QString defaultQmltypesFileName = "plugins.qmltypes";
    const QString defaultQmltypesPath = QDir::cleanPath(lib->path() + QDir::separator() + defaultQmltypesFileName);
    if ( QFile::exists(defaultQmltypesPath) && !typeInfoPaths.contains(defaultQmltypesPath) )
        typeInfoPaths += defaultQmltypesPath;

    foreach (const QmlDirParser::TypeInfo &typeInfo, dirParser.typeInfos() ){
        QString fullPath = QDir::cleanPath(lib->path() + QDir::separator() + typeInfo.fileName);
        if (!typeInfoPaths.contains(fullPath) && QFile::exists(fullPath))
            typeInfoPaths += fullPath;
    }

    if ( !typeInfoPaths.isEmpty() ){
        vlog_debug("editqmljs-scanner", "Scanning type info files: " + lib->path());

        foreach( const QString& typeInfoPath, typeInfoPaths ){
            result.append(scanTypeInfoFile(typeInfoPath, lib));
        }
    } else if ( dirParser.plugins().size() > 0 ){

        if ( m_forkEnabled && lib->exports().isEmpty() && PluginTypesFacade::pluginTypesEnabled() && !lib->uri().endsWith(".private") ){
            QProcess proc;

            //TODO: layers "base" doesn't seem to work
            QStringList arguments = QStringList() << "--layers" << "window" << "--global" << "editqml/plugininfo" << lib->uri();
            QString program = QString::fromStdString(lv::ApplicationContext::instance().applicationFilePath());

            proc.setProcessChannelMode(QProcess::MergedChannels);
            proc.start(program, arguments);

            proc.waitForFinished(10000);

            QString stream = proc.readAll();
            int trim = stream.indexOf("import");
            if ( trim >= 0 ){
                QByteArray trimmedStream = stream.mid(trim).toUtf8();
                scanTypeInfoStream(defaultQmltypesPath, trimmedStream, lib);
            }
        }
    }

    scanQmlDirForExports(dirParser, lib);
    return result;
}

QList<QmlTypeInfo::Ptr> QmlLanguageScanner::scanTypeInfoFile(const QString &typeInfo, const QmlLibraryInfo::Ptr &lib){
    QFile typeInfoFile(typeInfo);
    if ( !typeInfoFile.open(QIODevice::ReadOnly) ){
        if ( m_messageListener ){
            m_messageListener(VisualLog::MessageInfo::Error, "Cannot read plugin file: " + typeInfoFile.fileName());
        }
        return QList<QmlTypeInfo::Ptr>();
    }

    return scanTypeInfoStream(typeInfo, typeInfoFile.readAll(), lib);
}

QList<QmlTypeInfo::Ptr> QmlLanguageScanner::scanTypeInfoStream(
        const QString &typeInfoPath,
        const QByteArray &stream,
        const QmlLibraryInfo::Ptr &lib)
{
    QList<QmlTypeInfo::Ptr> result;

    QHash<QString, LanguageUtils::FakeMetaObject::ConstPtr> newObjects;
    QList<QmlJS::ModuleApiInfo> moduleApis;
    QStringList dependencies;

    QmlJS::TypeDescriptionReader reader(typeInfoPath, stream);
    if ( !reader(&newObjects, &moduleApis, &dependencies) ){
        if ( m_messageListener ){
            m_messageListener(VisualLog::MessageInfo::Error, "TypeInfo parse error: " + reader.errorMessage());
        }
    }

    foreach( const QString& dependency, dependencies){
        QmlLibraryDependency parsedDependency = QmlLibraryDependency::parse(dependency);
        if ( parsedDependency.isValid() ){
            if ( !m_libraries.contains(parsedDependency.path()) ){
                QmlLibraryInfo::Ptr depLib = ProjectQmlScope::findQmlLibraryInImports(
                    m_defaultImportPaths, parsedDependency.path(), parsedDependency.versionMajor(), parsedDependency.versionMinor()
                );
                if ( !depLib )
                    continue;

                insertNewLibrary(depLib);
            }

            lib->addDependency(parsedDependency.path());
        }
    }

    for ( auto it = newObjects.begin(); it != newObjects.end(); ++it ){
        QString typeModule = "";
        bool childModule = true;
        if ( it.value()->exports().size() > 0){
            foreach( LanguageUtils::FakeMetaObject::Export expt, it.value()->exports() ){
                if ( !expt.package.startsWith(lib->uri()) && expt.package != "<cpp>" ){
                    childModule = false;
                    typeModule = "";
                } else if ( expt.package.startsWith(lib->uri()) && childModule ){
                    typeModule = expt.package;
                } else if ( typeModule.isEmpty() && childModule ){
                    typeModule = expt.package;
                }
            }
        } else {
            typeModule = "<cpp>";
        }


        if ( !typeModule.isEmpty() ){
            if ( typeModule == "<cpp>" || typeModule == lib->uri() ){
                lib->addType(QmlTypeInfoPrivate::fromMetaObject(it.value(), lib->uri()));
            } else {
                result.append(QmlTypeInfoPrivate::fromMetaObject(it.value(), typeModule));
            }
        }
    }

    return result;
}

QmlTypeInfo::Ptr QmlLanguageScanner::scanObjectFile(
        QmlLibraryInfo::Ptr lib,
        const QString &filePath,
        const QString &componentName,
        const QString &fileData)
{
    DocumentQmlInfo::Ptr documentInfo = DocumentQmlInfo::create(filePath);
    bool parseResult = documentInfo->parse(fileData);
    if ( !parseResult )
        return nullptr;

    QList<DocumentQmlInfo::Import> imports = documentInfo->imports();
    QList<QPair<QString, QString> > dependencies;

    foreach( const DocumentQmlInfo::Import import, imports ){
        if (import.importType() == DocumentQmlInfo::Import::Directory){
            if ( !m_libraries.contains(import.uri()) ){
                QmlLibraryInfo::Ptr depLib = ProjectQmlScope::findQmlLibraryInPath(
                    import.uri(), import.uri(), false
                );
                if ( !depLib )
                    continue;

                insertNewLibrary(lib);
            }
            lib->addDependency(import.uri());
            dependencies.append(qMakePair(import.uri(), import.as()));
        }
        if (import.importType() == DocumentQmlInfo::Import::Library) {
            if ( !m_libraries.contains(import.uri()) ){
                QmlLibraryInfo::Ptr depLib = ProjectQmlScope::findQmlLibraryInImports(
                    m_defaultImportPaths, import.uri(), import.versionMajor(), import.versionMinor()
                );
                if ( !depLib )
                    continue;

                insertNewLibrary(depLib);
            }
            lib->addDependency(import.uri());
            dependencies.append(qMakePair(import.uri(), import.as()));
        }
    }

    QmlTypeInfo::Ptr ti = documentInfo->extractValueObjectWithExport(
        documentInfo->rootObject(),
        componentName,
        lib->uri()
    );

    if ( ti ){
        QmlDocumentReference dr;
        dr.dependencies = dependencies;
        dr.path = filePath;
        ti->setDocument(dr);
    }
    return ti;
}

void QmlLanguageScanner::scanQmlDirForExports(const QmlDirParser &dirParser, const QmlLibraryInfo::Ptr &lib){
    QHash<QString, QmlDirParser::Component> components = dirParser.components();

    for ( auto it = components.begin(); it != components.end(); ++it ){
        if ( it.key() == "classname" )
            continue;

        QString filePath = QDir::cleanPath(lib->path() + QDir::separator() + it->fileName);
        if ( QFile::exists(filePath) ){
            QString fileContent = QString::fromStdString(m_ioSession->readFromFile(filePath.toStdString()));
            QmlTypeInfo::Ptr fileType = scanObjectFile(lib, filePath, it->typeName, fileContent);
            if ( fileType ){
                lib->addType(fileType);
            }
        } else {
            if ( m_messageListener ){
                m_messageListener(VisualLog::MessageInfo::Warning, "Qml file does not exist for parsing: " + filePath);
            }
        }
    }
}

void QmlLanguageScanner::scanPathForExports(const QString &path, const QmlLibraryInfo::Ptr &lib){
    vlog_debug("editqmljs-scanner", "Scannig path: " + path);

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

        QString fileContent = QString::fromStdString(m_ioSession->readFromFile(info.filePath().toStdString()));
        QmlTypeInfo::Ptr fileType = scanObjectFile(lib, info.filePath(), info.baseName(), fileContent);
        if ( fileType ){
            lib->addType(fileType);
        }
    }
}

void QmlLanguageScanner::updateLibraryUnknownTypes(const QmlLibraryInfo::Ptr &lib){

    const QMap<QString, QmlTypeInfo::Ptr>& exp = lib->exports();

    for ( auto it = exp.begin(); it != exp.end(); ++it ){
        QmlTypeInfo::Ptr ti = it.value();
        if ( ti->document().isValid() ){
            for ( int i = 0; i < ti->totalProperties(); ++i ){

                if ( ti->propertyAt(i).typeName.language() == QmlTypeReference::Unknown ){

                    QString importAs = "";
                    QString lookedUpType = ti->propertyAt(i).typeName.name();


                    int splitIndex = lookedUpType.indexOf('.');
                    if ( splitIndex != -1 ){
                        importAs = lookedUpType.mid(0, splitIndex);
                        lookedUpType = lookedUpType.mid(splitIndex + 1);
                    }

                    const QList<QPair<QString, QString> > dependencies = ti->document().dependencies;
                    for ( auto depIt = dependencies.begin(); depIt != dependencies.end(); ++depIt ){
                        if ( depIt->second == importAs ){
                            QmlLibraryInfo::Ptr lib = m_libraries[depIt->first];
                            if ( lib ){
                                for ( auto typeIt = lib->exports().begin(); typeIt != lib->exports().end(); ++typeIt ){
                                    if ( typeIt.value()->exportType().name() == lookedUpType ){

                                        QmlPropertyInfo newProp = ti->propertyAt(i);
                                        newProp.typeName = typeIt.value()->exportType();
                                        ti->updateProperty(i, newProp);
                                    }
                                }
                            }
                        }
                    }

                }
            }
        }
    }
}

void QmlLanguageScanner::insertNewLibrary(const QmlLibraryInfo::Ptr &lib){
    if ( !m_libraries.contains(lib->uri()) ){
        m_libraries[lib->uri()] = lib;
    }
}

} // namespace
