/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#ifndef LVQMLDECLARATION_H
#define LVQMLDECLARATION_H

#include <QSharedPointer>

#include "live/lveditqmljsglobal.h"
#include "live/projectdocument.h"

namespace lv{

class LV_EDITQMLJS_EXPORT QmlDeclaration{

public:
    /** Shared pointer to QmlDeclaration */
    typedef QSharedPointer<QmlDeclaration>       Ptr;
    /** Shared const pointer to QmlDeclaration */
    typedef QSharedPointer<const QmlDeclaration> ConstPtr;

    friend class ProjectDocument;
    friend class DocumentHandler;

    /** ProjectDocument section type. */
    enum SectionType{
        /** Section type */
        Section = 1000
    };

public:
    static QmlDeclaration::Ptr create(const QStringList& identifierChain, ProjectDocument* document = 0);
    static QmlDeclaration::Ptr create(
        const QStringList &identifierChain,
        const QString& type,
        const QStringList& parentType,
        ProjectDocument* document = 0
    );
    static QmlDeclaration::Ptr create(
        const QStringList &identifierChain,
        const QString& type,
        const QStringList& parentType,
        int identifierPosition,
        int identifierLength,
        ProjectDocument* document = 0
    );

    ~QmlDeclaration();

    int identifierPosition() const;
    int identifierLength() const;

    int position() const;
    int valuePosition() const;

    const QStringList& identifierChain() const;
    const QString& type() const;
    const QStringList& parentType() const;

    ProjectDocument* document();

    int length() const;

    int valueOffset() const;
    void setValuePositionOffset(int positionOffset);

    int valueLength() const;
    void setValueLength(int valueLength);

    ProjectDocumentSection::Ptr section();
    void setSection(ProjectDocumentSection::Ptr section);

private:
    QmlDeclaration(const QStringList& identifierChain, ProjectDocument* document = 0);
    QmlDeclaration(
        const QStringList &identifierChain,
        const QString& type,
        const QStringList& parentType,
        ProjectDocument* document = 0
    );
    QmlDeclaration(
        const QStringList &identifierChain,
        const QString& type,
        const QStringList& parentType,
        int identifierPosition,
        int identifierLength,
        ProjectDocument* document = 0
    );

private:
    Q_DISABLE_COPY(QmlDeclaration)


    ProjectDocumentSection::Ptr m_section;

    int         m_identifierLength;
    QStringList m_identifierChain;
    QString     m_type;
    QStringList m_parentType;


    int         m_valueOffset;

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
inline const QString &QmlDeclaration::type() const{
    return m_type;
}

/// \brief Returns the paret type for this declaration
inline const QStringList &QmlDeclaration::parentType() const{
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
