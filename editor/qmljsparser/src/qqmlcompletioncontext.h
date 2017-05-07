#ifndef QQMLCOMPLETIONCONTEXT_H
#define QQMLCOMPLETIONCONTEXT_H

#include "qqmljsparserglobal.h"
#include "qcodecompletionmodel.h"

#include <QString>
#include <QStringList>

namespace lcv{

class Q_QMLJSPARSER_EXPORT QQmlCompletionContext : public QCodeCompletionContext{

public:
    enum Context{
        InImport = 1,
        InQml = 2,
        InLhsOfBinding = 4,
        InRhsofBinding = 8,
        InAfterOnLhsOfBinding = 16,
        InStringLiteral = 32,
    };

public:
    QQmlCompletionContext(
        int context,
        const QStringList& objectTypePath,
        const QStringList& propertyPath,
        const QStringList& expressionPath = QStringList(),
        int propertyPosition = -1
    );
    ~QQmlCompletionContext();

    int context() const;
    QString contextString() const;

    QString objectTypeName() const;
    const QStringList& objectTypePath() const;

    QString propertyName() const;
    const QStringList& propertyPath() const;
    int propertyPosition() const;

    const QStringList& expressionPath() const;

    bool operator ==(const QQmlCompletionContext& other) const;
    bool operator !=(const QQmlCompletionContext& other) const;

private:
    int         m_context;
    QStringList m_objectTypePath;
    QStringList m_propertyPath;
    QStringList m_expressionPath;
    int         m_propertyPosition;

};

inline int QQmlCompletionContext::context() const{
    return m_context;
}

inline QString QQmlCompletionContext::objectTypeName() const{
    return m_objectTypePath.size() > 0 ? m_objectTypePath.last() : "";
}

inline const QStringList &QQmlCompletionContext::objectTypePath() const{
    return m_objectTypePath;
}

inline QString QQmlCompletionContext::propertyName() const{
    return m_propertyPath.size() > 0 ? m_propertyPath.last() : "";
}

inline const QStringList &QQmlCompletionContext::propertyPath() const{
    return m_propertyPath;
}

inline const QStringList &QQmlCompletionContext::expressionPath() const{
    return m_expressionPath;
}

inline int QQmlCompletionContext::propertyPosition() const{
    return m_propertyPosition;
}

}// namespace

#endif // QQMLCOMPLETIONCONTEXT_H
