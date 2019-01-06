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
    /** Context state */
    enum Context{
        /** Context is within an import statement */
        InImport = 1,
        /** Context is within an import statement at the version section */
        InImportVersion = 2,
        /** Context is within qml */
        InQml = 4,
        /** Context is within the left side of the binding */
        InLhsOfBinding = 8,
        /** Context is within the right side of the binding */
        InRhsofBinding = 16,
        /** Context is within the right side of a slot binding */
        InAfterOnLhsOfBinding = 32,
        /** Context is within a string literal */
        InStringLiteral = 64,
    };

    /** Shared const pointer to QmlCompletionContext */
    typedef QSharedPointer<const QmlCompletionContext> ConstPtr;
    /** Shared pointer to QmlCompletionContext */
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

/// \brief Returns the current context state
inline int QmlCompletionContext::context() const{
    return m_context;
}

/// \brief Returns the object typename for this context if it has any, or an empty string if it
/// hasn't been captured
inline QString QmlCompletionContext::objectTypeName() const{
    return m_objectTypePath.size() > 0 ? m_objectTypePath.last() : "";
}

/// \brief Returns the object type for this context, or an empty list if it hasn't been captured
inline const QStringList &QmlCompletionContext::objectTypePath() const{
    return m_objectTypePath;
}

/// \brief Returns the property name for this completion context, or an empty string if it
/// hasn't been captured
inline QString QmlCompletionContext::propertyName() const{
    return m_propertyPath.size() > 0 ? m_propertyPath.last() : "";
}

/// \brief Returns the property type for this context, or an empty list if it hasn't been captured
inline const QStringList &QmlCompletionContext::propertyPath() const{
    return m_propertyPath;
}

/// \brief Returns the current expression the cursor is under
inline const QStringList &QmlCompletionContext::expressionPath() const{
    return m_expressionPath;
}

/// \brief Returns the property start position for this context, or -1 if it hasn't been captured
inline int QmlCompletionContext::propertyPosition() const{
    return m_propertyPosition;
}

}// namespace

#endif // LVQMLCOMPLETIONCONTEXT_H
