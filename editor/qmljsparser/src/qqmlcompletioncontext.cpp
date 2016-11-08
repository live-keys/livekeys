#include "qqmlcompletioncontext.h"

QQmlCompletionContext::QQmlCompletionContext(
        int context,
        const QStringList &objectTypePath,
        const QStringList &identifierPath,
        const QStringList &expressionPath)
    : m_context(context)
    , m_objectTypePath(objectTypePath)
    , m_propertyPath(identifierPath)
    , m_expressionPath(expressionPath)
{
}

QQmlCompletionContext::~QQmlCompletionContext(){
}

QString QQmlCompletionContext::contextString() const{
    QString base;
    if ( m_context & QQmlCompletionContext::InImport)
        base += "InImport";
    if ( m_context & QQmlCompletionContext::InQml)
        base += base.isEmpty() ? "InQml" : " | InQml";
    if ( m_context & QQmlCompletionContext::InLhsOfBinding)
        base += base.isEmpty() ? "InLhsOfBinding" : " | InLhsOfBinding";
    if ( m_context & QQmlCompletionContext::InRhsofBinding)
        base += base.isEmpty() ? "InRhsofBinding" : " | InRhsofBinding";
    if ( m_context & QQmlCompletionContext::InAfterOnLhsOfBinding)
        base += base.isEmpty() ? "InAfterOnLhsOfBinding" : " | InAfterOnLhsOfBinding";
    if ( m_context & QQmlCompletionContext::InStringLiteral)
        base += base.isEmpty() ? "InString" : " | InString";
    return base;
}

