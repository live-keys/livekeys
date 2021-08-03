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

#include "live/qmldeclaration.h"

namespace lv{

/**
 * \class lv::QmlDeclaration
 * \ingroup lveditqmljs
 * \brief Holds a qml property declaration
 *
 * ```js
 * Item{
 *     property int declaration: 20
 * }
 * ```
 *
 * In the above case, the lv::QmlDeclaration has the following properties:
 *
 * - identifierChain : the property identifier, in this case ```declaration```
 * - identifierPosition: position of the ```declaration``` property
 * - identifierLength: length of the ```declaration``` property
 * - type: type of the ```declaration``` property, in this case ```int```
 * - document: document where the ```declaration``` property was declared
 * - length: length of the declaration property
 * - valueOffset: offset for the value of the ```declaration``` property, in this case it's the position
 *   of the number ```20``` minus the position of the property
 * - valueLength: length of the value for the ```declaration``` property, or ```2```
 * - section: A section within the project document
 */

/// \brief QmlDeclaration constructor taking in an \p identifierChain and a \p document
QmlDeclaration::QmlDeclaration(const QStringList &identifierChain, ProjectDocument *document)
    : m_section(ProjectDocumentSection::create(QmlDeclaration::Section))
    , m_identifierLength(0)
    , m_identifierChain(identifierChain)
    , m_valueObjectScopeOffset(-1)
    , m_valueOffset(-1)
    , m_document(document)
{
}
/// \brief QmlDeclaration constructor
///
/// Receives \p identifierChain, \p identifierType, \p parentType and a \p document
QmlDeclaration::QmlDeclaration(
        const QStringList &identifierChain,
        const QmlTypeReference &identifierType,
        const QmlTypeReference &parentType,
        ProjectDocument *document)
    : m_section(ProjectDocumentSection::create(QmlDeclaration::Section))
    , m_identifierLength(0)
    , m_identifierChain(identifierChain)
    , m_type(identifierType)
    , m_parentType(parentType)
    , m_valueObjectScopeOffset(-1)
    , m_valueOffset(-1)
    , m_document(document)
{
}

/// \brief QmlDeclaration constructor
///
/// Receives \p identifierChain, \p identifierType, \p parentType,
/// a \p identifierPosition, a \p identifierLength and a \p document
QmlDeclaration::QmlDeclaration(
        const QStringList &identifierChain,
        const QmlTypeReference &identifierType,
        const QmlTypeReference &parentType,
        int identifierPosition,
        int identifierLength,
        ProjectDocument *document,
        bool isWritable)
    : m_section(ProjectDocumentSection::create(QmlDeclaration::Section, identifierPosition, identifierLength))
    , m_identifierLength(identifierLength)
    , m_identifierChain(identifierChain)
    , m_type(identifierType)
    , m_parentType(parentType)
    , m_valueObjectScopeOffset(-1)
    , m_valueOffset(-1)
    , m_isWritable(isWritable)
    , m_document(document)
{
}

/// \brief QmlDeclaration constructor
///
/// Receives \p identifierChain, \p identifierType, \p parentType,
/// a \p identifierPosition, a \p identifierLength and a \p document
QmlDeclaration::Ptr QmlDeclaration::create(const QStringList &identifierChain, ProjectDocument *document){
    return QmlDeclaration::Ptr(new QmlDeclaration(identifierChain, document));
}

/**
 * \brief Creates a new lv::QmlDeclaration
 *
 * Recieves \p identifierChain, \p identifierType, \p parentType and a \p document
 */
QmlDeclaration::Ptr QmlDeclaration::create(
        const QStringList &identifierChain,
        const QmlTypeReference &type,
        const QmlTypeReference &parentType,
        ProjectDocument *document)
{
    return QmlDeclaration::Ptr(new QmlDeclaration(identifierChain, type, parentType, document));
}

/**
 * \brief Creates a new lv::QmlDeclaration
 *
 * Receives \p identifierChain, \p identifierType, \p parentType, \p identifierPosition, a
 * \p identifierLength and a \p document
 */
QmlDeclaration::Ptr QmlDeclaration::create(const QStringList &identifierChain,
        const QmlTypeReference &type,
        const QmlTypeReference &parentType,
        int identifierPosition,
        int identifierLength,
        ProjectDocument *document,
        bool isWritable)
{
    return QmlDeclaration::Ptr(new QmlDeclaration(
        identifierChain,
        type,
        parentType,
        identifierPosition,
        identifierLength,
        document,
        isWritable
    ));
}

/// \brief QmlDeclaration destructor
QmlDeclaration::~QmlDeclaration(){
}

/// \brief If this declaration is a property and initialises an object, this will return the start of the object scope
int QmlDeclaration::valueObjectScopeOffset() const{
    return m_valueObjectScopeOffset;
}

/// \brief If this declaration is a property and initialises an object, this will set the start of the object scope
void QmlDeclaration::setValueObjectScopeOffset(int objectScopeOffset){
    m_valueObjectScopeOffset = objectScopeOffset;
}

/// \brief Returns true if this declaration is for an object
bool QmlDeclaration::isForObject() const{
    return m_valueOffset == 0;
}

/// \brief Returns true if this declaration is for a component
bool QmlDeclaration::isForComponent() const{
    return m_type.path() == "QtQuick" && m_type.name() == "Component";
}

/// \brief Returns true if this declaration is for a property
bool QmlDeclaration::isForProperty() const{
    return !isForObject();
}

/// \brief Returns true if this declaration is for a slot
bool QmlDeclaration::isForSlot() const{
    return m_type.language() == QmlTypeReference::Qml && m_type.name() == "slot";
}

bool QmlDeclaration::isForImports() const{
    return m_type.language() == QmlTypeReference::Qml && m_type.name() == "import";
}

}// namespace
