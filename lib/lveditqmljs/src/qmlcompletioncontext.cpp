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

#include "live/qmlcompletioncontext.h"

namespace lv{

QmlCompletionContext::QmlCompletionContext(
        int context,
        const QStringList &objectTypePath,
        const QStringList &identifierPath,
        const QStringList &expressionPath,
        int propertyPosition)
    : m_context(context)
    , m_objectTypePath(objectTypePath)
    , m_propertyPath(identifierPath)
    , m_expressionPath(expressionPath)
    , m_propertyPosition(propertyPosition)
{
}

QmlCompletionContext::~QmlCompletionContext(){
}

QmlCompletionContext::Ptr QmlCompletionContext::create(
        int context,
        const QStringList &objectTypePath,
        const QStringList &propertyPath,
        const QStringList &expressionPath,
        int propertyPosition)
{
    return QmlCompletionContext::Ptr(new QmlCompletionContext(
        context,
        objectTypePath,
        propertyPath,
        expressionPath,
        propertyPosition
    ));
}

QString QmlCompletionContext::contextString() const{
    QString base;
    if ( m_context & QmlCompletionContext::InImport)
        base += "InImport";
    if ( m_context & QmlCompletionContext::InImportVersion)
        base += base.isEmpty() ? "InImportVersion" : " | InImportVersion";
    if ( m_context & QmlCompletionContext::InQml)
        base += base.isEmpty() ? "InQml" : " | InQml";
    if ( m_context & QmlCompletionContext::InLhsOfBinding)
        base += base.isEmpty() ? "InLhsOfBinding" : " | InLhsOfBinding";
    if ( m_context & QmlCompletionContext::InRhsofBinding)
        base += base.isEmpty() ? "InRhsofBinding" : " | InRhsofBinding";
    if ( m_context & QmlCompletionContext::InAfterOnLhsOfBinding)
        base += base.isEmpty() ? "InAfterOnLhsOfBinding" : " | InAfterOnLhsOfBinding";
    if ( m_context & QmlCompletionContext::InStringLiteral)
        base += base.isEmpty() ? "InString" : " | InString";
    return base;
}

bool QmlCompletionContext::operator ==(const QmlCompletionContext &other) const{
    if ( m_context != other.context() )
        return false;
    if ( m_objectTypePath != other.objectTypePath() )
        return false;
    if ( m_propertyPath != other.propertyPath() )
        return false;
    if ( m_expressionPath.size() != other.expressionPath().size() )
        return false;
    for ( int i = 0; i < m_expressionPath.size() - 1; ++i ){
        if ( m_expressionPath[i] != other.expressionPath()[i] )
            return false;
    }
    return true;
}

bool QmlCompletionContext::operator !=(const QmlCompletionContext &other) const{
    return !(*this == other);
}

}// namespace
