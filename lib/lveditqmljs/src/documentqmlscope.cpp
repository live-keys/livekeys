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

    const DocumentQmlInfo::ImportList& imports = documentInfo->imports();
    for( const DocumentQmlInfo::Import& import: imports ){
        QList<QString> paths;
        if (import.importType() == DocumentQmlInfo::Import::Directory) {
            projectScope->findQmlLibraryInPath(
                import.uri(),
                false,
                paths
            );
            if ( paths.isEmpty() ){
                documentInfo->updateImportType(import.uri(), DocumentQmlInfo::Import::Invalid);
            }
        }
        if (import.importType() == DocumentQmlInfo::Import::Library) {
            if (!import.isVersionValid())
                continue;
            projectScope->findQmlLibraryInImports(
                import.uri(),
                import.versionMajor(),
                import.versionMinor(),
                paths
            );
            if ( paths.isEmpty() ){
                documentInfo->updateImportType(import.uri(), DocumentQmlInfo::Import::Invalid);
            }
        }
    }

    return documentScope;
}


}// namespace

