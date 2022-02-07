/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

#ifndef LVQMLDECLARATION_H
#define LVQMLDECLARATION_H

#include <QSharedPointer>

#include "live/lveditqmljsglobal.h"
#include "live/projectdocument.h"
#include "live/qmllanguageinfo.h"

namespace lv{

class LV_EDITQMLJS_EXPORT QmlDeclaration{

public:
    /** Shared pointer to QmlDeclaration */
    typedef QSharedPointer<QmlDeclaration>       Ptr;
    /** Shared const pointer to QmlDeclaration */
    typedef QSharedPointer<const QmlDeclaration> ConstPtr;

    friend class ProjectDocument;
    friend class CodeHandler;

    /** ProjectDocument section type. */
    enum SectionType{
        /** Section type */
        Section = 1000
    };

public:
    static QmlDeclaration::Ptr create(
        const QStringList& identifierChain,
        ProjectDocument* document = nullptr);
    static QmlDeclaration::Ptr create(
        const QStringList &identifierChain,
        const QmlTypeReference& type,
        const QmlTypeReference& parentType,
        ProjectDocument* document = nullptr
    );
    static QmlDeclaration::Ptr create(
        const QStringList &identifierChain,
        const QmlTypeReference &type,
        const QmlTypeReference &parentType,
        int identifierPosition,
        int identifierLength,
        ProjectDocument* document = nullptr,
        bool isWritable = false
    );

    ~QmlDeclaration();

    int identifierPosition() const;
    int identifierLength() const;

    int position() const;
    int valuePosition() const;

    const QStringList& identifierChain() const;
    const QmlTypeReference &type() const;
    const QmlTypeReference& parentType() const;

    ProjectDocument* document();

    int length() const;

    int valueOffset() const;
    void setValuePositionOffset(int positionOffset);

    int valueObjectScopeOffset() const;
    void setValueObjectScopeOffset(int objectScopeOffset);

    int valueLength() const;
    void setValueLength(int valueLength);

    ProjectDocumentSection::Ptr section();
    void setSection(ProjectDocumentSection::Ptr section);

    bool isForList() const;
    bool isForObject() const;
    bool isForComponent() const;
    bool isForProperty() const;
    bool isForSlot() const;
    bool isForImports() const;

    bool isWritable() const;

private:
    QmlDeclaration(
        const QStringList& identifierChain,
        ProjectDocument* document = nullptr);
    QmlDeclaration(
        const QStringList &identifierChain,
        const QmlTypeReference& type,
        const QmlTypeReference& parentType,
        ProjectDocument* document = nullptr
    );
    QmlDeclaration(const QStringList &identifierChain,
        const QmlTypeReference &type,
        const QmlTypeReference &parentType,
        int identifierPosition,
        int identifierLength,
        ProjectDocument* document = nullptr,
        bool isWritable = false);

private:
    Q_DISABLE_COPY(QmlDeclaration)

    ProjectDocumentSection::Ptr m_section;

    int             m_identifierLength;
    QStringList     m_identifierChain;
    QmlTypeReference m_type;
    QmlTypeReference m_parentType;

    int             m_valueObjectScopeOffset;
    int             m_valueOffset;

    bool            m_isWritable;

    ProjectDocument* m_document;
};

/// \brief Returns the identifier position
inline int QmlDeclaration::identifierPosition() const{
    return m_section->position();
}

/// \brief Returns the identifier length
inline int QmlDeclaration::identifierLength() const{
    return m_identifierLength;
}

/// \brief Returns position of this declaration, equivalent to identifier position
inline int QmlDeclaration::position() const{
    return identifierPosition();
}

/// \brief Returns the value position for this delcaration, calculated relative to the identifierPosition
inline int QmlDeclaration::valuePosition() const{
    if ( valueLength() == 0 || valueOffset() == -1 )
        return -1;
    return identifierPosition() + identifierLength() + valueOffset();
}

/// \brief Returns the identifierChain for this delcaration
inline const QStringList &QmlDeclaration::identifierChain() const{
    return m_identifierChain;
}

/// \brief Returns the type of this declaration
inline const QmlTypeReference &QmlDeclaration::type() const{
    return m_type;
}

/// \brief Returns the object type for this declaration
inline const QmlTypeReference &QmlDeclaration::parentType() const{
    return m_parentType;
}

/// \brief Returns the document this declaration is in
inline ProjectDocument *QmlDeclaration::document(){
    return m_document;
}

/// \brief Returns the length of this declaration
inline int QmlDeclaration::length() const{
    return m_section->length();
}

/// \brief Returns the valueOffset for this declaration
inline int QmlDeclaration::valueOffset() const{
    return m_valueOffset;
}

/// \brief Assings a new \p positionOffset for this declaration
inline void QmlDeclaration::setValuePositionOffset(int positionOffset){
    int vl = valueLength();
    m_valueOffset = positionOffset;
    m_section->resize(m_identifierLength + valueOffset() + vl);
}

/// \brief Returns the value lenght of this declaration
inline int QmlDeclaration::valueLength() const{
    return m_section->length() - m_identifierLength - m_valueOffset;
}

/// \brief Sets the value length of this declaration
inline void QmlDeclaration::setValueLength(int valueLength){
    m_section->resize(m_identifierLength + m_valueOffset + valueLength);
}

/// \brief Checks wether this is a list-child declaration instead of a property-based one
inline bool QmlDeclaration::isForList() const{
    return m_parentType.isEmpty();
}

inline bool QmlDeclaration::isWritable() const{
    return m_isWritable;
}

/// \brief Returns the current lv::ProjectDocument section.
inline ProjectDocumentSection::Ptr QmlDeclaration::section(){
    return m_section;
}

/// \brief Assigns the lv::ProjectDocument section.
inline void QmlDeclaration::setSection(ProjectDocumentSection::Ptr section) {
    m_section = section;
}

}// namespace

#endif // LVQMLDECLARATION_H
