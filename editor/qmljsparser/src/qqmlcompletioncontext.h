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
        InImportVersion = 2,
        InQml = 4,
        InLhsOfBinding = 8,
        InRhsofBinding = 16,
        InAfterOnLhsOfBinding = 32,
        InStringLiteral = 64,
    };

    typedef QSharedPointer<const QQmlCompletionContext> ConstPtr;
    typedef QSharedPointer<QQmlCompletionContext>       Ptr;

public:
    ~QQmlCompletionContext();

    static Ptr create(
        int context,
        const QStringList& objectTypePath,
        const QStringList& propertyPath,
        const QStringList& expressionPath = QStringList(),
        int propertyPosition = -1
    );

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

protected:
    QQmlCompletionContext(
        int context,
        const QStringList& objectTypePath,
        const QStringList& propertyPath,
        const QStringList& expressionPath = QStringList(),
        int propertyPosition = -1
    );

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
