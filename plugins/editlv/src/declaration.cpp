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

#include "declaration.h"

namespace lv{ namespace el {


Declaration::Declaration(const std::vector<std::string> &identifierChain, ProjectDocument* document)
    //: m_section(ProjectDocumentSection::create(Declaration::Section))
    : m_identifierLength(0)
    , m_identifierChain(identifierChain)
    , m_valueOffset(-1)
    , m_document(document)
{
}
/// \brief Declaration constructor
///
/// Receives \p identifierChain, \p identifierType, \p parentType and a \p document
Declaration::Declaration(
        const std::vector<std::string> &identifierChain,
        const TypeReference &identifierType,
        const TypeReference &parentType,
        ProjectDocument *document)
    //: m_section(ProjectDocumentSection::create(Declaration::Section))
    : m_identifierLength(0)
    , m_identifierChain(identifierChain)
    , m_type(identifierType)
    , m_parentType(parentType)
    , m_valueOffset(-1)
    , m_document(document)
{
}

/// \brief Declaration constructor
///
/// Receives \p identifierChain, \p identifierType, \p parentType,
/// a \p identifierPosition, a \p identifierLength and a \p document
Declaration::Declaration(
        const std::vector<std::string> &identifierChain,
        const TypeReference &identifierType,
        const TypeReference &parentType,
        int identifierPosition,
        int identifierLength,
        ProjectDocument *document)
    //: m_section(ProjectDocumentSection::create(Declaration::Section, identifierPosition, identifierLength))
    : m_identifierLength(identifierLength)
    , m_identifierChain(identifierChain)
    , m_type(identifierType)
    , m_parentType(parentType)
    , m_valueOffset(-1)
    , m_document(document)
{
}

/// \brief Declaration constructor
///
/// Receives \p identifierChain, \p identifierType, \p parentType,
/// a \p identifierPosition, a \p identifierLength and a \p document
Declaration::Ptr Declaration::create(const std::vector<std::string> &identifierChain, ProjectDocument* document){
    return Declaration::Ptr(new Declaration(identifierChain, document));
}

/**
 * \brief Creates a new lv::Declaration
 *
 * Recieves \p identifierChain, \p identifierType, \p parentType and a \p document
 */
Declaration::Ptr Declaration::create(
        const std::vector<std::string> &identifierChain,
        const TypeReference &type,
        const TypeReference &parentType,
        ProjectDocument *document)
{
    return Declaration::Ptr(new Declaration(identifierChain, type, parentType, document));
}

/**
 * \brief Creates a new lv::Declaration
 *
 * Receives \p identifierChain, \p identifierType, \p parentType, \p identifierPosition, a
 * \p identifierLength and a \p document
 */
Declaration::Ptr Declaration::create(
        const std::vector<std::string> &identifierChain,
        const TypeReference &type,
        const TypeReference &parentType,
        int identifierPosition,
        int identifierLength,
        ProjectDocument *document)
{
    return Declaration::Ptr(new Declaration(
        identifierChain,
        type,
        parentType,
        identifierPosition,
        identifierLength,
        document));
}

/// \brief Declaration destructor
Declaration::~Declaration(){
}

/// \brief Returns true if this declaration is for an object
bool Declaration::isForObject() const{
    return m_valueOffset == 0;
}

/// \brief Returns true if this declaration is for a property
bool Declaration::isForProperty() const{
    return !isForObject();
}

}// namespace
}
