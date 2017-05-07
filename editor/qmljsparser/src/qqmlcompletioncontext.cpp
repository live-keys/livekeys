#include "qqmlcompletioncontext.h"

namespace lcv{

QQmlCompletionContext::QQmlCompletionContext(
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

bool QQmlCompletionContext::operator ==(const QQmlCompletionContext &other) const{
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

bool QQmlCompletionContext::operator !=(const QQmlCompletionContext &other) const{
    return !(*this == other);
}

}// namespace
