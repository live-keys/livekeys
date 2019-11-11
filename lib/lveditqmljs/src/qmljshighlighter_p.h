/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
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
#include "live/qmleditfragment.h"
#include "qmljssettings.h"
#include "qmljs/qmljsscanner.h"
#include "live/syntaxhighlighter.h"

namespace lv{

/**
 * \brief The QCodeJSHighlighter is a private class used internally for highlighting.
 * \private
 */
class QmlJsHighlighter : public SyntaxHighlighter{

public:
    enum LookAheadType{
        Property,
        Type,
        Unknown
    };

public:
    QmlJsHighlighter(QmlJsSettings* settings, DocumentHandler* handler, QTextDocument *parent = nullptr);

    void setTarget(QTextDocument* target);

    LookAheadType lookAhead(
        const QString& text,
        const QList<QmlJS::Token>& tokens,
        QList<QmlJS::Token>::ConstIterator it,
        int state);

    static void collapse(const QTextBlock& tb, int& numLines, QString& replacement);

protected:
    void highlightBlock(const QString &text);
    QList<TextFormatRange> highlight(int lastUserState, int position, const QString& text);
    QList<TextFormatRange> highlightSections(const QList<ProjectDocumentSection::Ptr>&);
    void highlightSection(const ProjectDocumentSection::Ptr& section, ProjectDocumentBlockData *blockData, bool& exceeded);

private:
    static QSet<QString> m_knownIds;
    static QSet<QString> createKnownIds();

    QmlJsSettings*   m_settings;
};

inline void QmlJsHighlighter::setTarget(QTextDocument *target){
    setDocument(target);
}

}// namespace

#endif // LVQMLJSHIGHLIGHTER_P_H
