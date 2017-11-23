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

#ifndef LVQMLJSHIGHLIGHTER_P_H
#define LVQMLJSHIGHLIGHTER_P_H

#include <QTextBlockUserData>
#include <QSyntaxHighlighter>

#include "live/projectdocument.h"
#include "live/documenthandlerstate.h"
#include "live/documenteditfragment.h"
#include "live/coderuntimebinding.h"
#include "qmljssettings.h"
#include "qmljs/qmljsscanner.h"

namespace lv{

/**
 * @brief The QCodeJSHighlighter is a private class used internally for highlighting.
 */
class QmlJsHighlighter : public QSyntaxHighlighter{

public:
    enum LookAheadType{
        Property,
        Type,
        Unknown
    };

public:
    QmlJsHighlighter(QmlJsSettings* settings, QTextDocument *parent = 0, lv::DocumentHandlerState* state = 0);

    void setTarget(QTextDocument* target, DocumentHandlerState* state);

    LookAheadType lookAhead(
        const QString& text,
        const QList<QmlJS::Token>& tokens,
        QList<QmlJS::Token>::ConstIterator it,
        int state);

protected:
    void highlightBlock(const QString &text);

private:
    static QSet<QString> m_knownIds;
    static QSet<QString> createKnownIds();

    QmlJsSettings* m_settings;

    lv::DocumentHandlerState* m_documentState;
};

inline void QmlJsHighlighter::setTarget(QTextDocument *target, DocumentHandlerState *state){
    m_documentState = state;
    setDocument(target);
}

}// namespace

#endif // LVQMLJSHIGHLIGHTER_P_H
