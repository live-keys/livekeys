/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#include "live/documentqmlscope.h"
#include "live/projectqmlscope.h"

#include "qmljs/qmljsdocument.h"
#include "qmljs/qmljsinterpreter.h"
#include "qmljs/qmljsbind.h"
#include "qmljs/qmljstypedescriptionreader.h"
#include "languageutils/fakemetaobject.h"

#include "qmllibraryinfo_p.h"

namespace lv{

// QDocumentQmlScope::Import implementation
// ----------------------------------------

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

DocumentQmlScope::DocumentQmlScope(ProjectQmlScope::Ptr scope, DocumentQmlInfo::Ptr documentInfo)
    : m_projectScope(scope)
    , m_documentInfo(documentInfo)
{
}

DocumentQmlScope::~DocumentQmlScope(){
}

DocumentQmlScope::Ptr DocumentQmlScope::createEmptyScope(ProjectQmlScope::Ptr projectScope){
    DocumentQmlInfo::Ptr documentInfo = DocumentQmlInfo::create("");
    documentInfo->parse("");
    return DocumentQmlScope::Ptr(new DocumentQmlScope(projectScope, documentInfo));
}

DocumentQmlScope::Ptr DocumentQmlScope::createScope(
        const QString &fileName,
        const QString &data,
        ProjectQmlScope::Ptr projectScope)
{
    DocumentQmlInfo::Ptr documentInfo = DocumentQmlInfo::create(fileName.isEmpty() ? "untitled.qml" : fileName);
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

bool DocumentQmlScope::hasImport(const DocumentQmlScope::Import &key){
    foreach( const ImportEntry& imp, m_imports ){
        if ( imp.first == key )
            return true;
    }
    return false;
}

void DocumentQmlScope::addImport(const DocumentQmlScope::Import &key, const QString &path){
    m_imports.append(QPair<Import, QString>(key, path));
}


}// namespace

