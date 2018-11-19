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

#ifndef LVTEXTCONTROL_P_P_H
#define LVTEXTCONTROL_P_P_H

#include "QtGui/qtextdocumentfragment.h"
#include "QtGui/qtextcursor.h"
#include "QtGui/qtextformat.h"
#include "QtGui/qabstracttextdocumentlayout.h"
#include "QtCore/qbasictimer.h"
#include "QtCore/qpointer.h"
#include "private/qobject_p.h"
#include "textcontrol_p.h"

class QMimeData;
class QAbstractScrollArea;

namespace lv {
class TextEdit;

class TextControlPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(TextControl)
public:
    TextControlPrivate();

    bool cursorMoveKeyEvent(QKeyEvent *e);

    void updateCurrentCharFormat();

    void setContent(Qt::TextFormat format, const QString &text);

    void paste(const QMimeData *source);

    void setCursorPosition(const QPointF &pos);
    void setCursorPosition(int pos, QTextCursor::MoveMode mode = QTextCursor::MoveAnchor);

    void repaintCursor();
    inline void repaintSelection()
    { repaintOldAndNewSelection(QTextCursor()); }
    void repaintOldAndNewSelection(const QTextCursor &oldSelection);

    void selectionChanged(bool forceEmitSelectionChanged = false);

    void _q_updateCurrentCharFormatAndSelection();

#ifndef QT_NO_CLIPBOARD
    void setClipboardSelection();
#endif

    void _q_updateCursorPosChanged(const QTextCursor &someCursor);

    void setBlinkingCursorEnabled(bool enable);
    void updateCursorFlashTime();

    void extendWordwiseSelection(int suggestedNewPosition, qreal mouseXPosition);
    void extendBlockwiseSelection(int suggestedNewPosition);

    void _q_setCursorAfterUndoRedo(int undoPosition, int charsAdded, int charsRemoved);

    QRectF rectForPosition(int position) const;

    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *event, const QPointF &pos);
    void mouseMoveEvent(QMouseEvent *event, const QPointF &pos);
    void mouseReleaseEvent(QMouseEvent *event, const QPointF &pos);
    void mouseDoubleClickEvent(QMouseEvent *event, const QPointF &pos);
    bool sendMouseEventToInputContext(QMouseEvent *event, const QPointF &pos);
    void focusEvent(QFocusEvent *e);
#ifndef QT_NO_IM
    void inputMethodEvent(QInputMethodEvent *);
#endif
    void hoverEvent(QHoverEvent *e, const QPointF &pos);

    void activateLinkUnderCursor(QString href = QString());

#ifndef QT_NO_IM
    bool isPreediting() const;
    void commitPreedit();
    void cancelPreedit();
#endif

    QPointF tripleClickPoint;
    QPointF mousePressPos;

    QTextCharFormat lastCharFormat;

    QTextDocument *doc;
    QTextCursor cursor;
    QTextCursor selectedWordOnDoubleClick;
    QTextCursor selectedBlockOnTripleClick;
    QString anchorOnMousePress;
    QString linkToCopy;
    QString hoveredLink;

    QBasicTimer cursorBlinkTimer;
    QBasicTimer tripleClickTimer;

    TextEdit *textEdit;

#ifndef QT_NO_IM
    int preeditCursor;
#endif

    Qt::TextInteractionFlags interactionFlags;

    bool cursorOn : 1;
    bool cursorIsFocusIndicator : 1;
    bool mousePressed : 1;
    bool lastSelectionState : 1;
    bool ignoreAutomaticScrollbarAdjustement : 1;
    bool overwriteMode : 1;
    bool acceptRichText : 1;
    bool cursorVisible : 1; // used to hide the cursor in the preedit area
    bool cursorBlinkingEnabled : 1;
    bool hasFocus : 1;
    bool hadSelectionOnMousePress : 1;
    bool wordSelectionEnabled : 1;
    bool hasImState : 1;
    bool cursorRectangleChanged : 1;
    bool clearSelectionOnFocus : 1;

    int lastSelectionStart;
    int lastSelectionEnd;

    void _q_copyLink();
};

}

#endif // LVTEXTCONTROL_P_P_H
