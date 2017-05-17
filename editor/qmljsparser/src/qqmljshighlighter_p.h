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

#ifndef QQMLJSHIGHLIGHTER_P_HPP
#define QQMLJSHIGHLIGHTER_P_HPP

#include <QTextBlockUserData>
#include <QSyntaxHighlighter>

#include "qprojectdocument.h"
#include "qdocumentcodestate.h"
#include "qdocumenteditfragment.h"
#include "qmljs/qmljsscanner.h"

namespace lcv{

/**
 * @brief The QCodeJSHighlighter is a private class used internally.
 */
class QQmlJsHighlighter : public QSyntaxHighlighter{

public:
    enum ColorComponent{
        Text,
        Comment,
        Number,
        String,
        Operator,
        Identifier,
        Keyword,
        BuiltIn,
        QmlProperty,
        QmlType,
        QmlRuntimeBoundProperty,
        QmlRuntimeModifiedValue,
        QmlEdit
    };

    enum LookAheadType{
        Property,
        Type,
        Unknown
    };

public:
    QQmlJsHighlighter(QTextDocument *parent = 0, lcv::QDocumentCodeState* state = 0);

    QTextCharFormat& operator[](const QString& key);
    QTextCharFormat& operator[](const ColorComponent& key);

    void setTarget(QTextDocument* target, QDocumentCodeState* state);

    bool hasKey(const QString& key);

    QHash<QString, ColorComponent>::ConstIterator rolesBegin() const;
    QHash<QString, ColorComponent>::ConstIterator rolesEnd() const;

    LookAheadType lookAhead(
        const QString& text,
        const QList<QmlJS::Token>& tokens,
        QList<QmlJS::Token>::ConstIterator it,
        int state);

protected:
    void highlightBlock(const QString &text);

private:
    QTextCharFormat createFormat(const QColor& foreground);
    QTextCharFormat createFormat(const QColor& foreground, const QColor& background);


    QSet<QString> m_keywords;
    QSet<QString> m_knownIds;

    QHash<QString, ColorComponent> m_formatRoles;
    QHash<ColorComponent, QTextCharFormat> m_formats;

    lcv::QDocumentCodeState* m_documentState;
};


inline QTextCharFormat &QQmlJsHighlighter::operator[](const QString &key){
    return m_formats[m_formatRoles[key]];
}

inline QTextCharFormat &QQmlJsHighlighter::operator[](const QQmlJsHighlighter::ColorComponent &key){
    return m_formats[key];
}

inline void QQmlJsHighlighter::setTarget(QTextDocument *target, QDocumentCodeState *state){
    m_documentState = state;
    setDocument(target);
}

inline bool QQmlJsHighlighter::hasKey(const QString &key){
    return m_formatRoles.contains(key);
}

inline QHash<QString, QQmlJsHighlighter::ColorComponent>::ConstIterator QQmlJsHighlighter::rolesBegin() const{
    return m_formatRoles.begin();
}

inline QHash<QString, QQmlJsHighlighter::ColorComponent>::ConstIterator QQmlJsHighlighter::rolesEnd() const{
    return m_formatRoles.end();
}

inline QTextCharFormat QQmlJsHighlighter::createFormat(const QColor &foreground){
    QTextCharFormat fmt;
    fmt.setForeground(QBrush(foreground));
    return fmt;
}

inline QTextCharFormat QQmlJsHighlighter::createFormat(const QColor &foreground, const QColor &background){
    QTextCharFormat fmt;
    fmt.setForeground(QBrush(foreground));
    fmt.setBackground(QBrush(background));
    return fmt;
}

}// namespace

#endif // QCODEJSHIGHLIGHTER_P_HPP
