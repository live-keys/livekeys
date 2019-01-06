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


/**
 * \class lv::QmlCompletionContext
 * \ingroup lveditqmljs
 * \brief Contains a qml code completion context.
 *
 * You can use the QmlCompletionContextFinder to find such a context at a given cursor position.
 *
 * To find out the actual context state, use the QmlCompletionContext::context method.
 *
 * \code
 * QTextCursor cursor(document);
 * cursor.setPosition(100);
 * QmlCompletionContextFinder finder;
 *
 * QmlCompltionContext::ConstPtr context = finder.getContext(cursor);
 * vlog() << (context->context() == QmlCompletionContext::InImport);
 * \endcode
 *
 * For a string based output, QmlCompletionContext::contextString() will give a concatenated string of
 * the states
 *
 * \code
 * // ...
 * QmlCompletionContext::ConstPtr context = finder.getContext(cursor);
 * vlog() << context->contextString();
 * \endcode
 */


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

/// \brief Completion context destructor
QmlCompletionContext::~QmlCompletionContext(){
}

/**
 * \brief Creates a new QmlCompletionContext
 * \param context The given context
 * \param objectTypePath The object type
 * \param propertyPath The property chain
 * \param expressionPath The expression chain
 * \param propertyPosition The property position
 * \return a new QmlCompletionContext::Ptr
 */
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

/// \brief Returns the context a string of concatenated states
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

/// \brief Returns true if this QmlCompletionContext is equal to other, false otherwise
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

/// \brief Returns true if this QmlCompletionContext is not equal to other, false otherwise
bool QmlCompletionContext::operator !=(const QmlCompletionContext &other) const{
    return !(*this == other);
}

}// namespace
