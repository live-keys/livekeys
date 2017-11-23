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

#ifndef LVCODEDECLARATION_H
#define LVCODEDECLARATION_H

#include <QSharedPointer>

#include "live/lveditorglobal.h"

namespace lv{

class ProjectDocument;

class LV_EDITOR_EXPORT CodeDeclaration{

public:
    typedef QSharedPointer<CodeDeclaration>       Ptr;
    typedef QSharedPointer<const CodeDeclaration> ConstPtr;

public:
    static CodeDeclaration::Ptr create(const QStringList& identifierChain, ProjectDocument* document = 0);
    static CodeDeclaration::Ptr create(
            const QStringList &identifierChain,
            const QString& type,
            const QStringList& parentType,
            ProjectDocument* document = 0
    );
    static CodeDeclaration::Ptr create(
            const QStringList &identifierChain,
            const QString& type,
            const QStringList& parentType,
            int identifierPosition,
            int identifierLength,
            ProjectDocument* document = 0
    );

    ~CodeDeclaration();

    void setIdentifierPosition(int position);
    int identifierPosition() const;

    void setIdentifierLength(int length);
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

private:
    CodeDeclaration(const QStringList& identifierChain, ProjectDocument* document = 0);
    CodeDeclaration(
            const QStringList &identifierChain,
            const QString& type,
            const QStringList& parentType,
            ProjectDocument* document = 0
    );
    CodeDeclaration(
            const QStringList &identifierChain,
            const QString& type,
            const QStringList& parentType,
            int identifierPosition,
            int identifierLength,
            ProjectDocument* document = 0
    );

private:
    Q_DISABLE_COPY(CodeDeclaration)

    int         m_identifierPosition;
    int         m_identifierLength;
    QStringList m_identifierChain;
    QString     m_type;
    QStringList m_parentType;


    int m_valueOffset;
    int m_valueLength;

    ProjectDocument* m_document;
};

inline void CodeDeclaration::setIdentifierPosition(int position){
    m_identifierPosition = position;
}

inline int CodeDeclaration::identifierPosition() const{
    return m_identifierPosition;
}

inline void CodeDeclaration::setIdentifierLength(int length){
    m_identifierLength = length;
}

inline int CodeDeclaration::identifierLength() const{
    return m_identifierLength;
}

inline int CodeDeclaration::position() const{
    return identifierPosition();
}

inline int CodeDeclaration::valuePosition() const{
    if ( valueLength() == 0 || valueOffset() == -1 )
        return -1;
    return identifierPosition() + identifierLength() + valueOffset();
}

inline const QStringList &CodeDeclaration::identifierChain() const{
    return m_identifierChain;
}

inline const QString &CodeDeclaration::type() const{
    return m_type;
}

inline const QStringList &CodeDeclaration::parentType() const{
    return m_parentType;
}

inline ProjectDocument *CodeDeclaration::document(){
    return m_document;
}

inline int CodeDeclaration::length() const{
    return m_identifierLength + m_valueOffset + m_valueLength;
}

inline int CodeDeclaration::valueOffset() const{
    return m_valueOffset;
}

inline void CodeDeclaration::setValuePositionOffset(int positionOffset){
    m_valueOffset = positionOffset;
}

inline int CodeDeclaration::valueLength() const{
    return m_valueLength;
}

inline void CodeDeclaration::setValueLength(int valueLength){
    m_valueLength = valueLength;
}

}// namespace

#endif // LVCODEDECLARATION_H
