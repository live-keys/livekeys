#include "qdocumentqmlscope.h"
#include "qprojectqmlscope.h"

#include "qmljs/qmljsdocument.h"
#include "qmljs/qmljsinterpreter.h"
#include "qmljs/qmljsbind.h"
#include "qmljs/qmljstypedescriptionreader.h"
#include "languageutils/fakemetaobject.h"

#include "qqmllibraryinfo_p.h"

namespace lcv{

// QDocumentQmlScope::Import implementation
// ----------------------------------------

QDocumentQmlScope::Import::Import(
        QDocumentQmlScope::Import::Type importType,
        const QString &path,
        const QString& as,
        int vMajor,
        int vMinor)
    : m_type(importType)
    , m_path(path)
    , m_as(as)
    , m_versionMajor(vMajor)
    , m_versionMinor(vMinor)
{
}

// QDocumentQmlScope implementation
// --------------------------------

QDocumentQmlScope::QDocumentQmlScope(QProjectQmlScope::Ptr scope, QDocumentQmlInfo::MutablePtr documentInfo)
    : m_projectScope(scope)
    , m_documentInfo(documentInfo)
{
}

QDocumentQmlScope::~QDocumentQmlScope(){
}

QDocumentQmlScope::Ptr QDocumentQmlScope::createEmptyScope(QProjectQmlScope::Ptr projectScope){
    QDocumentQmlInfo::MutablePtr documentInfo = QDocumentQmlInfo::create("");
    documentInfo->parse("");
    return QDocumentQmlScope::Ptr(new QDocumentQmlScope(projectScope, documentInfo));
}

QDocumentQmlScope::Ptr QDocumentQmlScope::createScope(
        const QString &fileName,
        const QString &data,
        QProjectQmlScope::Ptr projectScope)
{
    QDocumentQmlInfo::MutablePtr documentInfo = QDocumentQmlInfo::create(fileName);
    documentInfo->parse(data);
    documentInfo->createRanges();

    QDocumentQmlScope::Ptr documentScope(new QDocumentQmlScope(projectScope, documentInfo));

//    QmlJS::Document::MutablePtr document = QmlJS::Document::create(fileName, QmlJS::Dialect::Qml);
//    document->setSource(data);
//    document->parseQml();

    foreach (const QmlJS::ImportInfo &importInfo, documentInfo->internalBind()->imports()) {
        QmlJS::ImportKey impKey(importInfo);
        Import::Type importType;
        switch(impKey.type){
        case QmlJS::ImportType::Invalid:           importType = Import::Invalid; break;
        case QmlJS::ImportType::Library:           importType = Import::Library; break;
        case QmlJS::ImportType::Directory:         importType = Import::Directory; break;
        case QmlJS::ImportType::ImplicitDirectory: importType = Import::ImplicitDirectory; break;
        case QmlJS::ImportType::File:              importType = Import::File; break;
        case QmlJS::ImportType::UnknownFile:       importType = Import::UnknownFile; break;
        default:                                   importType = Import::Invalid;
        }

        Import import(
            importType,
            importInfo.path(),
            importInfo.as(),
            importInfo.version().majorVersion(),
            importInfo.version().minorVersion()
        );

        //TODO: Add imports for implicit directory and implicit file

        if( !documentScope->hasImport(import) ){
            QList<QString> paths;
            if (importInfo.type() == QmlJS::ImportType::Directory) {
                projectScope->findQmlLibrary(
                    importInfo.path(),
                    importInfo.version().majorVersion(),
                    importInfo.version().minorVersion(),
                    paths
                );
                if ( paths.isEmpty() ){
                    import.setImportType(Import::Invalid);
                    documentScope->addImport(import, import.path());
                }
                foreach (const QString& path, paths )
                    documentScope->addImport(import, path);
            }
            if (importInfo.type() == QmlJS::ImportType::Library) {
                if (!importInfo.version().isValid())
                    continue;
                projectScope->findQmlLibraryInImports(
                    importInfo.path(),
                    importInfo.version().majorVersion(),
                    importInfo.version().minorVersion(),
                    paths
                );
                if ( paths.isEmpty() ){
                    import.setImportType(Import::Invalid);
                    documentScope->addImport(import, import.path());
                }
                foreach (const QString& path, paths )
                    documentScope->addImport(import, path);
            }
        }

    }

    return documentScope;
}

bool QDocumentQmlScope::hasImport(const QDocumentQmlScope::Import &key){
    foreach( const ImportEntry& imp, m_imports ){
        if ( imp.first == key )
            return true;
    }
    return false;
}

void QDocumentQmlScope::addImport(const QDocumentQmlScope::Import &key, const QString &path){
    m_imports.append(QPair<Import, QString>(key, path));
}


}// namespace

