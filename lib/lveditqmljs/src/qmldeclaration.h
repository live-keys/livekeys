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
    typedef QSharedPointer<QmlDeclaration>       Ptr;
    typedef QSharedPointer<const QmlDeclaration> ConstPtr;

    friend class ProjectDocument;
    friend class DocumentHandler;

    enum SectionType{
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

    ProjectDocumentSection::Ptr section(){ return m_section; }
    void setSection(ProjectDocumentSection::Ptr section) { m_section = section; }

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

inline int QmlDeclaration::identifierPosition() const{
    return m_section->position();
}

inline int QmlDeclaration::identifierLength() const{
    return m_identifierLength;
}

inline int QmlDeclaration::position() const{
    return identifierPosition();
}

inline int QmlDeclaration::valuePosition() const{
    if ( valueLength() == 0 || valueOffset() == -1 )
        return -1;
    return identifierPosition() + identifierLength() + valueOffset();
}

inline const QStringList &QmlDeclaration::identifierChain() const{
    return m_identifierChain;
}

inline const QString &QmlDeclaration::type() const{
    return m_type;
}

inline const QStringList &QmlDeclaration::parentType() const{
    return m_parentType;
}

inline ProjectDocument *QmlDeclaration::document(){
    return m_document;
}

inline int QmlDeclaration::length() const{
    return m_section->length();
}

inline int QmlDeclaration::valueOffset() const{
    return m_valueOffset;
}

inline void QmlDeclaration::setValuePositionOffset(int positionOffset){
    int vl = valueLength();
    m_valueOffset = positionOffset;
    m_section->resize(m_identifierLength + valueOffset() + vl);
}

inline int QmlDeclaration::valueLength() const{
    return m_section->length() - m_identifierLength - m_valueOffset;
}

inline void QmlDeclaration::setValueLength(int valueLength){
    m_section->resize(m_identifierLength + m_valueOffset + valueLength);
}

}// namespace

#endif // LVQMLDECLARATION_H
