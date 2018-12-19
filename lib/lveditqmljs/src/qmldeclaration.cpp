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

#include "live/qmldeclaration.h"

namespace lv{

QmlDeclaration::QmlDeclaration(const QStringList &identifierChain, ProjectDocument *document)
    : m_section(ProjectDocumentSection::create(QmlDeclaration::Section))
    , m_identifierLength(0)
    , m_identifierChain(identifierChain)
    , m_valueOffset(-1)
    , m_document(document)
{
}

QmlDeclaration::QmlDeclaration(
        const QStringList &identifierChain,
        const QString &identifierType,
        const QStringList &parentType,
        ProjectDocument *document)
    : m_section(ProjectDocumentSection::create(QmlDeclaration::Section))
    , m_identifierLength(0)
    , m_identifierChain(identifierChain)
    , m_type(identifierType)
    , m_parentType(parentType)
    , m_valueOffset(-1)
    , m_document(document)
{
}

QmlDeclaration::QmlDeclaration(
        const QStringList &identifierChain,
        const QString &identifierType,
        const QStringList &parentType,
        int identifierPosition,
        int identifierLength,
        ProjectDocument *document)
    : m_section(ProjectDocumentSection::create(QmlDeclaration::Section, identifierPosition, identifierLength))
    , m_identifierLength(identifierLength)
    , m_identifierChain(identifierChain)
    , m_type(identifierType)
    , m_parentType(parentType)
    , m_valueOffset(-1)
    , m_document(document)
{
}

QmlDeclaration::Ptr QmlDeclaration::create(const QStringList &identifierChain, ProjectDocument *document){
    return QmlDeclaration::Ptr(new QmlDeclaration(identifierChain, document));
}

QmlDeclaration::Ptr QmlDeclaration::create(
        const QStringList &identifierChain,
        const QString &type,
        const QStringList &parentType,
        ProjectDocument *document)
{
    return QmlDeclaration::Ptr(new QmlDeclaration(identifierChain, type, parentType, document));
}

QmlDeclaration::Ptr QmlDeclaration::create(
        const QStringList &identifierChain,
        const QString &type,
        const QStringList &parentType,
        int identifierPosition,
        int identifierLength,
        ProjectDocument *document)
{
    return QmlDeclaration::Ptr(new QmlDeclaration(
        identifierChain,
        type,
        parentType,
        identifierPosition,
        identifierLength,
        document
    ));
}

QmlDeclaration::~QmlDeclaration(){
}

}// namespace
