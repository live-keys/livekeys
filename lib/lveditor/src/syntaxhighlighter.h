/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef LVSYNTAXHIGHLIGHTER_H
#define LVSYNTAXHIGHLIGHTER_H

#include <QtCore/qobject.h>
#include <QtGui/qtextobject.h>
#include "live/lveditorglobal.h"
#include "live/projectdocument.h"

class QTextDocument;
class QTextCharFormat;
class QFont;
class QColor;
class QTextBlockUserData;

namespace lv{

class SyntaxHighlighterPrivate;
class LV_EDITOR_EXPORT SyntaxHighlighter : public QObject {

    Q_OBJECT
    Q_DECLARE_PRIVATE(SyntaxHighlighter)

public:
    class TextFormatRange{
    public:
        int start;
        int length;
        int userstate;
        int userstateFollows;
        bool collapsible;
        CollapseFunctionType function;
        QTextCharFormat format;
        TextFormatRange() : userstate(-1), userstateFollows(-1), collapsible(false), function(nullptr) {}
    };



    explicit SyntaxHighlighter(QObject *parent);
    explicit SyntaxHighlighter(QTextDocument *parent);
    virtual ~SyntaxHighlighter();

    void setDocument(QTextDocument *doc);
    QTextDocument *document() const;
    static QString stateToString(int state);

    static const int StateMask;
public slots:
    void rehighlight();
    void rehighlightBlock(const QTextBlock &block);

    void _q_reformatBlocks(int from, int charsRemoved, int charsAdded);
    void _q_delayedRehighlight();

protected:
//    virtual void highlightBlock(const QString &text) = 0;
    virtual void documentChanged(int, int, int) {}
    virtual QList<TextFormatRange> highlight(int lastUserState, int position, const QString& text) = 0;
    virtual QList<TextFormatRange> highlightSections(const QList<ProjectDocumentSection::Ptr>&) = 0;

    void setFormat(int start, int count, const QTextCharFormat &format);
    void setFormat(int start, int count, const QColor &color);
    void setFormat(int start, int count, const QFont &font);
    QTextCharFormat format(int pos) const;

    int previousBlockState() const;
    int currentBlockState() const;
    void setCurrentBlockState(int newState);

    void setCurrentBlockUserData(QTextBlockUserData *data);
    QTextBlockUserData *currentBlockUserData() const;

    QTextBlock currentBlock() const;

private:

    SyntaxHighlighterPrivate* d_ptr;

    Q_DISABLE_COPY(SyntaxHighlighter)
};

} // namespace

#endif // LVSYNTAXHIGHLIGHTER_H
