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

#ifndef LVQMLCOMPLETIONCONTEXT_H
#define LVQMLCOMPLETIONCONTEXT_H

#include "live/lveditqmljsglobal.h"
#include "live/codecompletionmodel.h"

#include <QString>
#include <QStringList>
#include <QSharedPointer>

namespace lv{

class LV_EDITQMLJS_EXPORT QmlCompletionContext : public CodeCompletionContext{

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

    typedef QSharedPointer<const QmlCompletionContext> ConstPtr;
    typedef QSharedPointer<QmlCompletionContext>       Ptr;

public:
    ~QmlCompletionContext();

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

    bool operator ==(const QmlCompletionContext& other) const;
    bool operator !=(const QmlCompletionContext& other) const;

protected:
    QmlCompletionContext(
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

inline int QmlCompletionContext::context() const{
    return m_context;
}

inline QString QmlCompletionContext::objectTypeName() const{
    return m_objectTypePath.size() > 0 ? m_objectTypePath.last() : "";
}

inline const QStringList &QmlCompletionContext::objectTypePath() const{
    return m_objectTypePath;
}

inline QString QmlCompletionContext::propertyName() const{
    return m_propertyPath.size() > 0 ? m_propertyPath.last() : "";
}

inline const QStringList &QmlCompletionContext::propertyPath() const{
    return m_propertyPath;
}

inline const QStringList &QmlCompletionContext::expressionPath() const{
    return m_expressionPath;
}

inline int QmlCompletionContext::propertyPosition() const{
    return m_propertyPosition;
}

}// namespace

#endif // LVQMLCOMPLETIONCONTEXT_H
