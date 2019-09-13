/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
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

#include "live/documentqmlscope.h"
#include "live/projectqmlscope.h"

#include "qmljs/qmljsdocument.h"
#include "qmljs/qmljsinterpreter.h"
#include "qmljs/qmljsbind.h"
#include "qmljs/qmljstypedescriptionreader.h"
#include "languageutils/fakemetaobject.h"

#include "qmllibraryinfo_p.h"

namespace lv{

/**
 * \class lv::DocumentQmlScope
 * \ingroup lveditqmljs
 * \brief Imports and import data for a lv::ProjectDocument
 *
 * The DocumentQmlScope contains external information, or information comming in from
 * imports for a lv::ProjectDocument.
 *
 * DocumentQmlScopes are mostly used internally, expecially by the lv::CodeQmlHandler
 */

/**
 * \class lv::DocumentQmlScope::Import
 * \ingroup lveditqmljs
 * \brief Import data associated with a lv::DocumentQmlScope
 */

/**
 * \property lv::DocumentQmlScope::Import::NoVersion
 * \brief Value used to check wether a version is valid or not
 */

// QDocumentQmlScope::Import implementation
// ----------------------------------------

/**
 * \brief Import constructor
 *
 * Takes an \p importType, the \p path of the import, the \p as namespace in which to import for this
 * document and the two versions: \p vMajor, \p vMinor
 */
DocumentQmlScope::Import::Import(
        DocumentQmlScope::Import::Type importType,
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

/**
 * \brief DocumentQmlScope constructor
 *
 * Receives a \p scope pointer to the lv::ProjectQmlScope, and the \p documentInfo for this document.
 */
DocumentQmlScope::DocumentQmlScope(ProjectQmlScope::Ptr scope, DocumentQmlInfo::Ptr documentInfo)
    : m_projectScope(scope)
    , m_documentInfo(documentInfo)
{
}

/// \brief DocumentQmlScope destructor
DocumentQmlScope::~DocumentQmlScope(){
}

/**
 * \brief Creates an empty DocumentQmlScope from a \p projectScope
 */
DocumentQmlScope::Ptr DocumentQmlScope::createEmptyScope(ProjectQmlScope::Ptr projectScope){
    DocumentQmlInfo::Ptr documentInfo = DocumentQmlInfo::create("");
    documentInfo->parse("");
    return DocumentQmlScope::Ptr(new DocumentQmlScope(projectScope, documentInfo));
}


/**
 * @brief Creates a new lv::DocumentQmlScope
 *
 * Creates a new lv::DocumentQmlScope where the \p fileName and \p data are parsed for imports, and for each import
 * the \p projectScope in order to populate the returned object.
 */
DocumentQmlScope::Ptr DocumentQmlScope::createScope(
        const QString &fileName,
        const QString &data,
        ProjectQmlScope::Ptr projectScope)
{
    QString printName = fileName;
    int tempAnnotator = fileName.indexOf("T:");
    if ( tempAnnotator > -1 ){
        printName = fileName.mid(0, tempAnnotator) + fileName.mid(tempAnnotator + 2 ) + "untitled.qml";
    }

    DocumentQmlInfo::Ptr documentInfo = DocumentQmlInfo::create(printName);
    documentInfo->parse(data);
    documentInfo->createRanges();

    DocumentQmlScope::Ptr documentScope(new DocumentQmlScope(projectScope, documentInfo));
    projectScope->addImplicitLibrary(documentInfo->path());

    QList<DocumentQmlScope::Import> imports = extractImports(documentInfo);
    foreach( DocumentQmlScope::Import import, imports ){

        if( !documentScope->hasImport(import) ){
            QList<QString> paths;
            if (import.importType() == DocumentQmlScope::Import::Directory) {
                projectScope->findQmlLibraryInPath(
                    import.path(),
                    false,
                    paths
                );
                if ( paths.isEmpty() ){
                    import.setImportType(Import::Invalid);
                    documentScope->addImport(import, import.path());
                }
                foreach (const QString& path, paths )
                    documentScope->addImport(import, path);
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


/**
 * \brief Extracts the list of imports from the given \p documentInfo.
 */
QList<DocumentQmlScope::Import> DocumentQmlScope::extractImports(DocumentQmlInfo::Ptr documentInfo){
    QList<DocumentQmlScope::Import> imports;
    QList<QmlJS::ImportInfo> importInfos = documentInfo->internalBind()->imports();

    for ( QList<QmlJS::ImportInfo>::iterator it = importInfos.begin(); it != importInfos.end(); ++it ){
        QmlJS::ImportKey impKey(*it);
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

        imports << Import(
           importType,
           it->path(),
           it->as(),
           it->version().majorVersion(),
           it->version().minorVersion()
       );
    }
    return imports;
}

/// \brief Checks for a given import \p key and \returns true if it exists, false otherwise
bool DocumentQmlScope::hasImport(const DocumentQmlScope::Import &key){
    foreach( const ImportEntry& imp, m_imports ){
        if ( imp.first == key )
            return true;
    }
    return false;
}

/// \brief Adds an import \p path to a given \p key.
void DocumentQmlScope::addImport(const DocumentQmlScope::Import &key, const QString &path){
    m_imports.append(QPair<Import, QString>(key, path));
}


}// namespace

