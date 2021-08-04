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

#ifndef LVDeclaration_H
#define LVDeclaration_H

#include <memory>
#include "live/elements/languageinfo.h"
#include "live/projectdocument.h"

namespace lv{ namespace el {

class Declaration{

public:
    /** Shared pointer to Declaration */
    typedef std::shared_ptr<Declaration>       Ptr;
    /** Shared const pointer to Declaration */
    typedef std::shared_ptr<const Declaration> ConstPtr;

    friend class lv::ProjectDocument;
    friend class lv::CodeHandler;

    /** ProjectDocument section type. */
    enum SectionType{
        /** Section type */
        Section = 1000
    };

public:
    static Declaration::Ptr create(
        const std::vector<std::string>& identifierChain,
        ProjectDocument* document
    );
    static Declaration::Ptr create(
        const std::vector<std::string> &identifierChain,
        const TypeReference& type,
        const TypeReference& parentType,
        ProjectDocument* document
    );
    static Declaration::Ptr create(const std::vector<std::string> &identifierChain,
        const TypeReference &type,
        const TypeReference &parentType,
        int identifierPosition,
        int identifierLength,
        ProjectDocument* document
    );

    ~Declaration();

    int identifierPosition() const;
    int identifierLength() const;

    int position() const;
    int valuePosition() const;

    const std::vector<std::string>& identifierChain() const;
    const TypeReference &type() const;
    const TypeReference& parentType() const;

    lv::ProjectDocument *document();

    int length() const;

    int valueOffset() const;
    void setValuePositionOffset(int positionOffset);

    int valueLength() const;
    void setValueLength(int valueLength);

    bool isListDeclaration() const;

//    ProjectDocumentSection::Ptr section();
//    void setSection(ProjectDocumentSection::Ptr section); // TODO: ELEMENTS

    bool isForObject() const;
    bool isForProperty() const;

private:
    Declaration(
        const std::vector<std::string>& identifierChain,
        ProjectDocument* document);
    Declaration(
        const std::vector<std::string> &identifierChain,
        const TypeReference& type,
        const TypeReference& parentType,
        ProjectDocument* document
    );
    Declaration(
        const std::vector<std::string> &identifierChain,
        const TypeReference &type,
        const TypeReference &parentType,
        int identifierPosition,
        int identifierLength,
        ProjectDocument* document
    );

private:
    DISABLE_COPY(Declaration);

    // ProjectDocumentSection::Ptr m_section;

    int             m_identifierLength;
    std::vector<std::string>     m_identifierChain;
    TypeReference m_type;
    TypeReference m_parentType;
    int             m_valueOffset;
    ProjectDocument* m_document;


};

/// \brief Returns the identifier position
inline int Declaration::identifierPosition() const{
    return 0;
    //return m_section->position();
}

/// \brief Returns the identifier length
inline int Declaration::identifierLength() const{
    return m_identifierLength;
}

/// \brief Returns position of this declaration, equivalent to identifier position
inline int Declaration::position() const{
    return identifierPosition();
}

/// \brief Returns the value position for this delcaration, calculated relative to the identifierPosition
inline int Declaration::valuePosition() const{
    if ( valueLength() == 0 || valueOffset() == -1 )
        return -1;
    return identifierPosition() + identifierLength() + valueOffset();
}

/// \brief Returns the identifierChain for this delcaration
inline const std::vector<std::string> &Declaration::identifierChain() const{
    return m_identifierChain;
}

/// \brief Returns the type of this declaration
inline const TypeReference &Declaration::type() const{
    return m_type;
}

/// \brief Returns the object type for this declaration
inline const TypeReference &Declaration::parentType() const{
    return m_parentType;
}


/// \brief Returns the length of this declaration
inline int Declaration::length() const{
    return 0;
    // return m_section->length();
}

/// \brief Returns the valueOffset for this declaration
inline int Declaration::valueOffset() const{
    return m_valueOffset;
}

/// \brief Assings a new \p positionOffset for this declaration
inline void Declaration::setValuePositionOffset(int positionOffset){
    int vl = valueLength();
    m_valueOffset = positionOffset;
    // m_section->resize(m_identifierLength + valueOffset() + vl);
}

/// \brief Returns the value lenght of this declaration
inline int Declaration::valueLength() const{
    return 0;
    //return m_section->length() - m_identifierLength - m_valueOffset;
}

/// \brief Sets the value length of this declaration
inline void Declaration::setValueLength(int valueLength){
    //m_section->resize(m_identifierLength + m_valueOffset + valueLength);
}

/// \brief Checks wether this is a list-child declaration instead of a property-based one
inline bool Declaration::isListDeclaration() const{
    return m_parentType.isEmpty();
}

///// \brief Returns the current lv::ProjectDocument section.
//inline ProjectDocumentSection::Ptr Declaration::section(){
//    return m_section;
//}

///// \brief Assigns the lv::ProjectDocument section.
//inline void Declaration::setSection(ProjectDocumentSection::Ptr section) {
//    m_section = section;
//}

}// namespace
}// namespace
#endif // LVDeclaration_H
