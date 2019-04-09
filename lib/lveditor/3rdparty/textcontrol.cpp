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

#include "textcontrol_p.h"
#include "textcontrol_p_p.h"

#ifndef QT_NO_TEXTCONTROL

#include "qabstracttextdocumentlayout.h"
#include <qcoreapplication.h>
#include <qfont.h>
#include <qevent.h>
#include <qdebug.h>
#include <qdrag.h>
#include <qclipboard.h>
#include <qtimer.h>
#include <qinputmethod.h>
#include "qtextdocument.h"
#include "textdocumentlayout.h"

#include "qtextlist.h"
#include "qtextdocumentwriter.h"

// <TOREMOVE>
// See what is being used throughout
#include "private/qqmlglobal_p.h"

#include <qtextformat.h>
#include <qdatetime.h>
#include <qbuffer.h>
#include <qguiapplication.h>
#include <limits.h>
#include <qtexttable.h>
#include <qvariant.h>
#include <qurl.h>
#include <qstylehints.h>
#include <qmetaobject.h>

#include "textedit_p.h"
//#include "palettemanager.h"
#include "linecontrol.h"

namespace lv {

// ### these should come from QStyleHints
const int textCursorWidth = 1;

QT_BEGIN_NAMESPACE

#ifndef QT_NO_CONTEXTMENU
#endif

// could go into QTextCursor...
static QTextLine currentTextLine(const QTextCursor &cursor)
{
    const QTextBlock block = cursor.block();
    if (!block.isValid())
        return QTextLine();

    const QTextLayout *layout = block.layout();
    if (!layout)
        return QTextLine();

    const int relativePos = cursor.position() - block.position();
    return layout->lineForTextPosition(relativePos);
}

TextControlPrivate::TextControlPrivate()
    : doc(0),
#ifndef QT_NO_IM
      preeditCursor(0),
#endif
      interactionFlags(Qt::TextEditorInteraction),
      cursorOn(false),
      cursorIsFocusIndicator(false),
      mousePressed(false),
      lastSelectionState(false),
      ignoreAutomaticScrollbarAdjustement(false),
      overwriteMode(false),
      acceptRichText(true),
      cursorVisible(false),
      cursorBlinkingEnabled(false),
      hasFocus(false),
      hadSelectionOnMousePress(false),
      wordSelectionEnabled(false),
      hasImState(false),
      cursorRectangleChanged(false),
      clearSelectionOnFocus(true),
      lastSelectionStart(-1),
      lastSelectionEnd(-1)
{}

bool TextControlPrivate::cursorMoveKeyEvent(QKeyEvent *e)
{
#ifdef QT_NO_SHORTCUT
    Q_UNUSED(e);
#endif

    Q_Q(TextControl);
    if (cursor.isNull())
        return false;

    const QTextCursor oldSelection = cursor;
    const int oldCursorPos = cursor.position();

    QTextCursor::MoveMode mode = QTextCursor::MoveAnchor;
    QTextCursor::MoveOperation op = QTextCursor::NoMove;

    if (false) {
    }
#ifndef QT_NO_SHORTCUT
    if (e == QKeySequence::MoveToNextChar) {
            op = QTextCursor::Right;
    }
    else if (e == QKeySequence::MoveToPreviousChar) {
            op = QTextCursor::Left;
    }
    else if (e == QKeySequence::SelectNextChar) {
           op = QTextCursor::Right;
           mode = QTextCursor::KeepAnchor;
    }
    else if (e == QKeySequence::SelectPreviousChar) {
            op = QTextCursor::Left;
            mode = QTextCursor::KeepAnchor;
    }
    else if (e == QKeySequence::SelectNextWord) {
            op = QTextCursor::WordRight;
            mode = QTextCursor::KeepAnchor;
    }
    else if (e == QKeySequence::SelectPreviousWord) {
            op = QTextCursor::WordLeft;
            mode = QTextCursor::KeepAnchor;
    }
    else if (e == QKeySequence::SelectStartOfLine) {
            op = QTextCursor::StartOfLine;
            mode = QTextCursor::KeepAnchor;
    }
    else if (e == QKeySequence::SelectEndOfLine) {
            op = QTextCursor::EndOfLine;
            mode = QTextCursor::KeepAnchor;
    }
    else if (e == QKeySequence::SelectStartOfBlock) {
            op = QTextCursor::StartOfBlock;
            mode = QTextCursor::KeepAnchor;
    }
    else if (e == QKeySequence::SelectEndOfBlock) {
            op = QTextCursor::EndOfBlock;
            mode = QTextCursor::KeepAnchor;
    }
    else if (e == QKeySequence::SelectStartOfDocument) {
            op = QTextCursor::Start;
            mode = QTextCursor::KeepAnchor;
    }
    else if (e == QKeySequence::SelectEndOfDocument) {
            op = QTextCursor::End;
            mode = QTextCursor::KeepAnchor;
    }
    else if (e == QKeySequence::SelectPreviousLine) {
            op = QTextCursor::Up;
            mode = QTextCursor::KeepAnchor;
    }
    else if (e == QKeySequence::SelectNextLine) {
            op = QTextCursor::Down;
            mode = QTextCursor::KeepAnchor;
            {
                QTextBlock block = cursor.block();
                QTextLine line = currentTextLine(cursor);
                if (!block.next().isValid()
                    && line.isValid()
                    && line.lineNumber() == block.layout()->lineCount() - 1)
                    op = QTextCursor::End;
            }
    }
    else if (e == QKeySequence::MoveToNextWord) {
            op = QTextCursor::WordRight;
    }
    else if (e == QKeySequence::MoveToPreviousWord) {
            op = QTextCursor::WordLeft;
    }
    else if (e == QKeySequence::MoveToEndOfBlock) {
            op = QTextCursor::EndOfBlock;
    }
    else if (e == QKeySequence::MoveToStartOfBlock) {
            op = QTextCursor::StartOfBlock;
    }
    else if (e == QKeySequence::MoveToNextLine) {
            op = QTextCursor::Down;
    }
    else if (e == QKeySequence::MoveToPreviousLine) {
            op = QTextCursor::Up;
    }
    else if (e == QKeySequence::MoveToStartOfLine) {
            op = QTextCursor::StartOfLine;
    }
    else if (e == QKeySequence::MoveToEndOfLine) {
            op = QTextCursor::EndOfLine;
    }
    else if (e == QKeySequence::MoveToStartOfDocument) {
            op = QTextCursor::Start;
    }
    else if (e == QKeySequence::MoveToEndOfDocument) {
            op = QTextCursor::End;
    }
#endif // QT_NO_SHORTCUT
    else {
        return false;
    }

// Except for pageup and pagedown, OS X has very different behavior, we don't do it all, but
// here's the breakdown:
// Shift still works as an anchor, but only one of the other keys can be down Ctrl (Command),
// Alt (Option), or Meta (Control).
// Command/Control + Left/Right -- Move to left or right of the line
//                 + Up/Down -- Move to top bottom of the file. (Control doesn't move the cursor)
// Option + Left/Right -- Move one word Left/right.
//        + Up/Down  -- Begin/End of Paragraph.
// Home/End Top/Bottom of file. (usually don't move the cursor, but will select)

    bool visualNavigation = cursor.visualNavigation();
    cursor.setVisualNavigation(true);

    // auto pm = textEdit->getPaletteManager();
    // int result = pm->isLineBeforePalette(cursor.block().blockNumber());
    LineControl* lc = textEdit->lineControl();
    int result = lc->isLineBeforePalette(cursor.block().blockNumber());
    if (result != 0)
    {
        if (cursor.atBlockEnd() &&(e == QKeySequence::MoveToNextChar || e ==  QKeySequence::MoveToNextWord || e == QKeySequence::MoveToEndOfLine))
        {
            for (int i = 0; i < result; ++i)
                cursor.movePosition(QTextCursor::Down);
            cursor.movePosition(QTextCursor::EndOfBlock);
        }

        if (cursor.atBlockEnd() && (e == QKeySequence::SelectNextChar || e == QKeySequence::SelectNextWord || e == QKeySequence::SelectEndOfLine))
        {
            for (int i = 0; i < result; ++i)
            {
                cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor);
            }
            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        }

        if (e == QKeySequence::MoveToNextLine)
        {
            for (int i = 0; i < result; ++i)
            {
                cursor.movePosition(QTextCursor::Down);
            }
        }

        if (e == QKeySequence::SelectNextLine)
        {
            for (int i = 0; i < result; ++i)
            {
                cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor);
            }
        }

    }

    result = lc->isLineAfterPalette(cursor.block().blockNumber());
    if (result != 0)
    {
        if (cursor.atBlockStart() &&(e == QKeySequence::MoveToPreviousChar || e ==  QKeySequence::MoveToPreviousWord || e == QKeySequence::MoveToStartOfLine))
        {
            for (int i = 0; i < result; ++i)
                cursor.movePosition(QTextCursor::Up);
            cursor.movePosition(QTextCursor::StartOfBlock);
        }

        if (cursor.atBlockStart() && (e == QKeySequence::SelectPreviousChar || e == QKeySequence::SelectPreviousWord || e == QKeySequence::SelectStartOfLine))
        {
            for (int i = 0; i < result; ++i)
            {
                cursor.movePosition(QTextCursor::Up, QTextCursor::KeepAnchor);
            }
            cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
        }

        if (e == QKeySequence::MoveToPreviousLine)
        {
            for (int i = 0; i < result; ++i)
            {
                cursor.movePosition(QTextCursor::Up);
            }
        }

        if (e == QKeySequence::SelectPreviousLine)
        {
            for (int i = 0; i < result; ++i)
            {
                cursor.movePosition(QTextCursor::Up, QTextCursor::KeepAnchor);
            }
        }

    }


    const bool moved = cursor.movePosition(op, mode);
    cursor.setVisualNavigation(visualNavigation);

    bool isNavigationEvent
            =  e->key() == Qt::Key_Up
            || e->key() == Qt::Key_Down
            || e->key() == Qt::Key_Left
            || e->key() == Qt::Key_Right;

    if (moved) {
        if (cursor.position() != oldCursorPos)
            emit q->cursorPositionChanged();
        q->updateCursorRectangle(true);
    } else if (isNavigationEvent && oldSelection.anchor() == cursor.anchor()) {
        return false;
    }

    selectionChanged(/*forceEmitSelectionChanged =*/(mode == QTextCursor::KeepAnchor));

    repaintOldAndNewSelection(oldSelection);

    return true;
}

void TextControlPrivate::updateCurrentCharFormat()
{
    Q_Q(TextControl);

    QTextCharFormat fmt = cursor.charFormat();
    if (fmt == lastCharFormat)
        return;
    lastCharFormat = fmt;

    emit q->currentCharFormatChanged(fmt);
    cursorRectangleChanged = true;
}

void TextControlPrivate::setContent(Qt::TextFormat format, const QString &text)
{
    Q_Q(TextControl);

#ifndef QT_NO_IM
    cancelPreedit();
#endif

    // for use when called from setPlainText. we may want to re-use the currently
    // set char format then.
    const QTextCharFormat charFormatForInsertion = cursor.charFormat();

    bool previousUndoRedoState = doc->isUndoRedoEnabled();
    doc->setUndoRedoEnabled(false);

    const int oldCursorPos = cursor.position();

    // avoid multiple textChanged() signals being emitted
    qmlobject_disconnect(doc, QTextDocument, SIGNAL(contentsChanged()), q, TextControl, SIGNAL(textChanged()));

    if (!text.isEmpty()) {
        // clear 'our' cursor for insertion to prevent
        // the emission of the cursorPositionChanged() signal.
        // instead we emit it only once at the end instead of
        // at the end of the document after loading and when
        // positioning the cursor again to the start of the
        // document.
        cursor = QTextCursor();
        if (format == Qt::PlainText) {
            QTextCursor formatCursor(doc);
            // put the setPlainText and the setCharFormat into one edit block,
            // so that the syntax highlight triggers only /once/ for the entire
            // document, not twice.
            formatCursor.beginEditBlock();
            doc->setPlainText(text);
            doc->setUndoRedoEnabled(false);
            formatCursor.select(QTextCursor::Document);
            formatCursor.setCharFormat(charFormatForInsertion);
            formatCursor.endEditBlock();
        } else {
#ifndef QT_NO_TEXTHTMLPARSER
            doc->setHtml(text);
#else
            doc->setPlainText(text);
#endif
            doc->setUndoRedoEnabled(false);
        }
        cursor = QTextCursor(doc);
    } else {
        doc->clear();
    }
    cursor.setCharFormat(charFormatForInsertion);

    qmlobject_connect(doc, QTextDocument, SIGNAL(contentsChanged()), q, TextControl, SIGNAL(textChanged()));
    emit q->textChanged();
    doc->setUndoRedoEnabled(previousUndoRedoState);
    _q_updateCurrentCharFormatAndSelection();
    doc->setModified(false);

    q->updateCursorRectangle(true);
    if (cursor.position() != oldCursorPos)
        emit q->cursorPositionChanged();
}

void TextControlPrivate::setCursorPosition(const QPointF &pos)
{
    Q_Q(TextControl);
    const int cursorPos = q->hitTest(pos, Qt::FuzzyHit);
    if (cursorPos == -1)
        return;
    cursor.setPosition(cursorPos);
}

void TextControlPrivate::setCursorPosition(int pos, QTextCursor::MoveMode mode)
{
    cursor.setPosition(pos, mode);

    if (mode != QTextCursor::KeepAnchor) {
        selectedWordOnDoubleClick = QTextCursor();
        selectedBlockOnTripleClick = QTextCursor();
    }
}

void TextControlPrivate::repaintCursor()
{
    Q_Q(TextControl);
    emit q->updateCursorRequest();
}

void TextControlPrivate::repaintOldAndNewSelection(const QTextCursor &oldSelection)
{
    Q_Q(TextControl);
    if (cursor.hasSelection()
        && oldSelection.hasSelection()
        && cursor.currentFrame() == oldSelection.currentFrame()
        && !cursor.hasComplexSelection()
        && !oldSelection.hasComplexSelection()
        && cursor.anchor() == oldSelection.anchor()
        ) {
        QTextCursor differenceSelection(doc);
        differenceSelection.setPosition(oldSelection.position());
        differenceSelection.setPosition(cursor.position(), QTextCursor::KeepAnchor);
        emit q->updateRequest();
    } else {
        if (!oldSelection.hasSelection() && !cursor.hasSelection()) {
            if (!oldSelection.isNull())
                emit q->updateCursorRequest();
            emit q->updateCursorRequest();

        } else {
            if (!oldSelection.isNull())
                emit q->updateRequest();
            emit q->updateRequest();
        }
    }
}

void TextControlPrivate::selectionChanged(bool forceEmitSelectionChanged /*=false*/)
{
    Q_Q(TextControl);
    if (forceEmitSelectionChanged) {
#ifndef QT_NO_IM
        if (hasFocus)
            qGuiApp->inputMethod()->update(Qt::ImCurrentSelection);
#endif
        emit q->selectionChanged();
    }

    bool current = cursor.hasSelection();
    int selectionStart = cursor.selectionStart();
    int selectionEnd = cursor.selectionEnd();
    if (current == lastSelectionState && (!current || (selectionStart == lastSelectionStart && selectionEnd == lastSelectionEnd)))
        return;

    if (lastSelectionState != current) {
        lastSelectionState = current;
        emit q->copyAvailable(current);
    }

    lastSelectionStart = selectionStart;
    lastSelectionEnd = selectionEnd;

    if (!forceEmitSelectionChanged) {
#ifndef QT_NO_IM
        if (hasFocus)
            qGuiApp->inputMethod()->update(Qt::ImCurrentSelection);
#endif
        emit q->selectionChanged();
    }
}

void TextControlPrivate::_q_updateCurrentCharFormatAndSelection()
{
    updateCurrentCharFormat();
    selectionChanged();
}

#ifndef QT_NO_CLIPBOARD
void TextControlPrivate::setClipboardSelection()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    if (!cursor.hasSelection() || !clipboard->supportsSelection())
        return;
    Q_Q(TextControl);
    QMimeData *data = q->createMimeDataFromSelection();
    clipboard->setMimeData(data, QClipboard::Selection);
}
#endif

void TextControlPrivate::_q_updateCursorPosChanged(const QTextCursor &someCursor)
{
    Q_Q(TextControl);
    if (someCursor.isCopyOf(cursor)) {
        emit q->cursorPositionChanged();
        q->updateCursorRectangle(true);
    }
}

void TextControlPrivate::setBlinkingCursorEnabled(bool enable)
{
    if (cursorBlinkingEnabled == enable)
        return;

    cursorBlinkingEnabled = enable;
    updateCursorFlashTime();

    if (enable)
        connect(qApp->styleHints(), &QStyleHints::cursorFlashTimeChanged, this, &TextControlPrivate::updateCursorFlashTime);
    else
        disconnect(qApp->styleHints(), &QStyleHints::cursorFlashTimeChanged, this, &TextControlPrivate::updateCursorFlashTime);
}

void TextControlPrivate::updateCursorFlashTime()
{
    // Note: cursorOn represents the current blinking state controlled by a timer, and
    // should not be confused with cursorVisible or cursorBlinkingEnabled. However, we
    // interpretate a cursorFlashTime of 0 to mean "always on, never blink".
    cursorOn = true;
    int flashTime = QGuiApplication::styleHints()->cursorFlashTime();

    if (cursorBlinkingEnabled && flashTime >= 2)
        cursorBlinkTimer.start(flashTime / 2, q_func());
    else
        cursorBlinkTimer.stop();

    repaintCursor();
}

void TextControlPrivate::extendWordwiseSelection(int suggestedNewPosition, qreal mouseXPosition)
{
    Q_Q(TextControl);

    // if inside the initial selected word keep that
    if (suggestedNewPosition >= selectedWordOnDoubleClick.selectionStart()
        && suggestedNewPosition <= selectedWordOnDoubleClick.selectionEnd()) {
        q->setTextCursor(selectedWordOnDoubleClick);
        return;
    }

    QTextCursor curs = selectedWordOnDoubleClick;
    curs.setPosition(suggestedNewPosition, QTextCursor::KeepAnchor);

    if (!curs.movePosition(QTextCursor::StartOfWord))
        return;
    const int wordStartPos = curs.position();

    const int blockPos = curs.block().position();
    const QPointF blockCoordinates = q->blockBoundingRect(curs.block()).topLeft();

    QTextLine line = currentTextLine(curs);
    if (!line.isValid())
        return;

    const qreal wordStartX = line.cursorToX(curs.position() - blockPos) + blockCoordinates.x();

    if (!curs.movePosition(QTextCursor::EndOfWord))
        return;
    const int wordEndPos = curs.position();

    const QTextLine otherLine = currentTextLine(curs);
    if (otherLine.textStart() != line.textStart()
        || wordEndPos == wordStartPos)
        return;

    const qreal wordEndX = line.cursorToX(curs.position() - blockPos) + blockCoordinates.x();

    if (!wordSelectionEnabled && (mouseXPosition < wordStartX || mouseXPosition > wordEndX))
        return;

    if (suggestedNewPosition < selectedWordOnDoubleClick.position()) {
        cursor.setPosition(selectedWordOnDoubleClick.selectionEnd());
        setCursorPosition(wordStartPos, QTextCursor::KeepAnchor);
    } else {
        cursor.setPosition(selectedWordOnDoubleClick.selectionStart());
        setCursorPosition(wordEndPos, QTextCursor::KeepAnchor);
    }

    if (interactionFlags & Qt::TextSelectableByMouse) {
#ifndef QT_NO_CLIPBOARD
        setClipboardSelection();
#endif
        selectionChanged(true);
    }
}

void TextControlPrivate::extendBlockwiseSelection(int suggestedNewPosition)
{
    Q_Q(TextControl);

    // if inside the initial selected line keep that
    if (suggestedNewPosition >= selectedBlockOnTripleClick.selectionStart()
        && suggestedNewPosition <= selectedBlockOnTripleClick.selectionEnd()) {
        q->setTextCursor(selectedBlockOnTripleClick);
        return;
    }

    if (suggestedNewPosition < selectedBlockOnTripleClick.position()) {
        cursor.setPosition(selectedBlockOnTripleClick.selectionEnd());
        cursor.setPosition(suggestedNewPosition, QTextCursor::KeepAnchor);
        cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
    } else {
        cursor.setPosition(selectedBlockOnTripleClick.selectionStart());
        cursor.setPosition(suggestedNewPosition, QTextCursor::KeepAnchor);
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    }

    if (interactionFlags & Qt::TextSelectableByMouse) {
#ifndef QT_NO_CLIPBOARD
        setClipboardSelection();
#endif
        selectionChanged(true);
    }
}

void TextControl::undo()
{
    Q_D(TextControl);
    d->repaintSelection();
    const int oldCursorPos = d->cursor.position();
    d->doc->undo(&d->cursor);
    if (d->cursor.position() != oldCursorPos)
        emit cursorPositionChanged();
    updateCursorRectangle(true);
}

void TextControl::redo()
{
    Q_D(TextControl);
    d->repaintSelection();
    const int oldCursorPos = d->cursor.position();
    d->doc->redo(&d->cursor);
    if (d->cursor.position() != oldCursorPos)
        emit cursorPositionChanged();
    updateCursorRectangle(true);
}

void TextControl::clear()
{
    Q_D(TextControl);
    d->cursor.select(QTextCursor::Document);
    d->cursor.removeSelectedText();
}

TextControl::TextControl(QTextDocument *doc, QObject *parent)
    : QObject(*new TextControlPrivate, parent)
{
    Q_D(TextControl);
    Q_ASSERT(doc);

    QAbstractTextDocumentLayout *layout = doc->documentLayout();
    qmlobject_connect(layout, QAbstractTextDocumentLayout, SIGNAL(update(QRectF)), this, TextControl, SIGNAL(updateRequest()));
    qmlobject_connect(layout, QAbstractTextDocumentLayout, SIGNAL(updateBlock(QTextBlock)), this, TextControl, SIGNAL(updateRequest()));
    qmlobject_connect(doc, QTextDocument, SIGNAL(contentsChanged()), this, TextControl, SIGNAL(textChanged()));
    qmlobject_connect(doc, QTextDocument, SIGNAL(contentsChanged()), this, TextControl, SLOT(_q_updateCurrentCharFormatAndSelection()));
    qmlobject_connect(doc, QTextDocument, SIGNAL(cursorPositionChanged(QTextCursor)), this, TextControl, SLOT(_q_updateCursorPosChanged(QTextCursor)));
    connect(doc, &QTextDocument::contentsChange, this, &TextControl::contentsChange);

    layout->setProperty("cursorWidth", textCursorWidth);

    d->doc = doc;
    d->cursor = QTextCursor(doc);
    d->lastCharFormat = d->cursor.charFormat();
    doc->setPageSize(QSizeF(0, 0));
    doc->setModified(false);
    doc->setUndoRedoEnabled(true);
}

TextControl::~TextControl()
{
}

QTextDocument *TextControl::document() const
{
    Q_D(const TextControl);
    return d->doc;
}

void TextControl::updateCursorRectangle(bool force)
{
    Q_D(TextControl);
    const bool update = d->cursorRectangleChanged || force;
    d->cursorRectangleChanged = false;
    if (update)
        emit cursorRectangleChanged();
}

void TextControl::clearSelectionOnFocus(bool value){
    Q_D(TextControl);
    d->clearSelectionOnFocus = value;
}

void TextControl::setTextCursor(const QTextCursor &cursor)
{
    Q_D(TextControl);
#ifndef QT_NO_IM
    d->commitPreedit();
#endif
    d->cursorIsFocusIndicator = false;
    const bool posChanged = cursor.position() != d->cursor.position();
    const QTextCursor oldSelection = d->cursor;
    d->cursor = cursor;
    d->cursorOn = d->hasFocus && (d->interactionFlags & Qt::TextEditable);
    d->_q_updateCurrentCharFormatAndSelection();
    updateCursorRectangle(true);
    d->repaintOldAndNewSelection(oldSelection);
    if (posChanged)
        emit cursorPositionChanged();
}

QTextCursor TextControl::textCursor() const
{
    Q_D(const TextControl);
    return d->cursor;
}

#ifndef QT_NO_CLIPBOARD

void TextControl::cut()
{
    Q_D(TextControl);
    if (!(d->interactionFlags & Qt::TextEditable) || !d->cursor.hasSelection())
        return;
    copy();
    d->cursor.removeSelectedText();
}

void TextControl::copy()
{
    Q_D(TextControl);
    if (!d->cursor.hasSelection())
        return;
    QMimeData *data = createMimeDataFromSelection();
    QGuiApplication::clipboard()->setMimeData(data);
}

void TextControl::paste(QClipboard::Mode mode)
{
    const QMimeData *md = QGuiApplication::clipboard()->mimeData(mode);
    if (md)
        insertFromMimeData(md);
}
#endif

void TextControl::selectAll()
{
    Q_D(TextControl);
    const int selectionLength = qAbs(d->cursor.position() - d->cursor.anchor());
    d->cursor.select(QTextCursor::Document);
    d->selectionChanged(selectionLength != qAbs(d->cursor.position() - d->cursor.anchor()));
    d->cursorIsFocusIndicator = false;
    emit updateRequest();
}

void TextControl::processEvent(QEvent *e, const QPointF &coordinateOffset)
{
    QMatrix m;
    m.translate(coordinateOffset.x(), coordinateOffset.y());
    processEvent(e, m);
}

void TextControl::processEvent(QEvent *e, const QMatrix &matrix)
{
    Q_D(TextControl);
    if (d->interactionFlags == Qt::NoTextInteraction) {
        e->ignore();
        return;
    }

    // PALETTE
    QMouseEvent *ev = nullptr;
    if (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseMove ||
            e->type() == QEvent::MouseButtonRelease || e->type() == QEvent::MouseButtonDblClick)
    {
        ev = static_cast<QMouseEvent *>(e);
        QPointF mousePos = ev->localPos();
        int oldy = mousePos.y();
        // mousePos.setY(d->textEdit->getPaletteManager()->positionOffset(oldy));
        mousePos.setY(d->textEdit->lineControl()->positionOffset(oldy));
#if (QT_VERSION > QT_VERSION_CHECK(5,7,1))
        ev->setLocalPos(mousePos);
#else
        ev = new QMouseEvent(ev->type(), mousePos, ev->windowPos(), ev->screenPos(), ev->button(), ev->buttons(), ev->modifiers(), ev->source());
        e = ev;
#endif
    }

    switch (e->type()) {
        case QEvent::KeyPress:
            d->keyPressEvent(static_cast<QKeyEvent *>(e));
            break;
        case QEvent::KeyRelease:
            d->keyReleaseEvent(static_cast<QKeyEvent *>(e));
            break;
        case QEvent::MouseButtonPress: {
            QMouseEvent *ev = static_cast<QMouseEvent *>(e);
            d->mousePressEvent(ev, matrix.map(ev->localPos()));
            break; }
        case QEvent::MouseMove: {
            QMouseEvent *ev = static_cast<QMouseEvent *>(e);
            d->mouseMoveEvent(ev, matrix.map(ev->localPos()));
            break; }
        case QEvent::MouseButtonRelease: {
            QMouseEvent *ev = static_cast<QMouseEvent *>(e);
            d->mouseReleaseEvent(ev, matrix.map(ev->localPos()));
            break; }
        case QEvent::MouseButtonDblClick: {
            QMouseEvent *ev = static_cast<QMouseEvent *>(e);
            d->mouseDoubleClickEvent(ev, matrix.map(ev->localPos()));
            break; }
        case QEvent::HoverEnter:
        case QEvent::HoverMove:
        case QEvent::HoverLeave: {
            QHoverEvent *ev = static_cast<QHoverEvent *>(e);
            d->hoverEvent(ev, matrix.map(ev->posF()));
            break; }
#ifndef QT_NO_IM
        case QEvent::InputMethod:
            d->inputMethodEvent(static_cast<QInputMethodEvent *>(e));
            break;
#endif
        case QEvent::FocusIn:
        case QEvent::FocusOut:
            d->focusEvent(static_cast<QFocusEvent *>(e));
            break;

        case QEvent::ShortcutOverride:
            if (d->interactionFlags & Qt::TextEditable) {
                QKeyEvent* ke = static_cast<QKeyEvent *>(e);
                if (ke->modifiers() == Qt::NoModifier
                    || ke->modifiers() == Qt::ShiftModifier
                    || ke->modifiers() == Qt::KeypadModifier) {
                    if (ke->key() < Qt::Key_Escape) {
                        ke->accept();
                    } else {
                        switch (ke->key()) {
                            case Qt::Key_Return:
                            case Qt::Key_Enter:
                            case Qt::Key_Delete:
                            case Qt::Key_Home:
                            case Qt::Key_End:
                            case Qt::Key_Backspace:
                            case Qt::Key_Left:
                            case Qt::Key_Right:
                            case Qt::Key_Up:
                            case Qt::Key_Down:
                            case Qt::Key_Tab:

                            if (ke->key() == Qt::Key_Return)
                            {
                                auto layout = d->textEdit->getDocumentLayout();
                                std::pair<int, int> result(-1, -1);
                                if (layout)
                                {
                                    result = layout->isFirstLineOfCollapsedSection(d->cursor.block().blockNumber());
                                }
                                if (layout && result.first != -1)
                                {
                                    layout->expandLines(result.first, result.second);
                                }
                            }
                            ke->accept();
                        default:
                            break;
                        }
                    }
#ifndef QT_NO_SHORTCUT
                } else if (ke == QKeySequence::Copy
                           || ke == QKeySequence::Paste
                           || ke == QKeySequence::Cut
                           || ke == QKeySequence::Redo
                           || ke == QKeySequence::Undo
                           || ke == QKeySequence::MoveToNextWord
                           || ke == QKeySequence::MoveToPreviousWord
                           || ke == QKeySequence::MoveToStartOfDocument
                           || ke == QKeySequence::MoveToEndOfDocument
                           || ke == QKeySequence::SelectNextWord
                           || ke == QKeySequence::SelectPreviousWord
                           || ke == QKeySequence::SelectStartOfLine
                           || ke == QKeySequence::SelectEndOfLine
                           || ke == QKeySequence::SelectStartOfBlock
                           || ke == QKeySequence::SelectEndOfBlock
                           || ke == QKeySequence::SelectStartOfDocument
                           || ke == QKeySequence::SelectEndOfDocument
                           || ke == QKeySequence::SelectAll
                          ) {

                    auto layout = d->textEdit->getDocumentLayout();
                    std::pair<int, int> result(-1, -1);
                    if (layout && ke == QKeySequence::Paste)
                    {
                        result = layout->isFirstLineOfCollapsedSection(d->cursor.block().blockNumber());
                    }
                    if (result.first != -1)
                    {
                        layout->expandLines(result.first, result.second);
                    }

                    ke->accept();
#endif
                }
            }
            break;
        default:
            break;
    }

#if (QT_VERSION <= QT_VERSION_CHECK(5,7,1))
    if (ev) delete ev;
#endif
}

bool TextControl::event(QEvent *e)
{
    return QObject::event(e);
}

void TextControl::timerEvent(QTimerEvent *e)
{
    Q_D(TextControl);
    if (e->timerId() == d->cursorBlinkTimer.timerId()) {
        d->cursorOn = !d->cursorOn;

        d->repaintCursor();
    } else if (e->timerId() == d->tripleClickTimer.timerId()) {
        d->tripleClickTimer.stop();
    }
}

void TextControl::setPlainText(const QString &text)
{
    Q_D(TextControl);
    d->setContent(Qt::PlainText, text);
}

void TextControl::setHtml(const QString &text)
{
    Q_D(TextControl);
    d->setContent(Qt::RichText, text);
}


void TextControlPrivate::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Back) {
         e->ignore();
         return;
    }
    return;
}

void TextControlPrivate::keyPressEvent(QKeyEvent *e)
{
    Q_Q(TextControl);

/*    //PALETTE TEST
    static int cnt = 0;

    switch (cnt)
    {
    case 0: textEdit->linePaletteAdded(8, 9, 35, textEdit); break;
    case 1: textEdit->linePaletteAdded(15, 17, 75, nullptr); break;
    case 2: textEdit->linePaletteRemoved(nullptr); break;
    case 3: textEdit->linePaletteHeightChanged(textEdit, 55); break;
    }

    cnt++;*/

    if (e->key() == Qt::Key_Back) {
         e->ignore();
         return;
    }

#ifndef QT_NO_SHORTCUT
    if (e == QKeySequence::SelectAll) {
            e->accept();
            q->selectAll();
            return;
    }
#ifndef QT_NO_CLIPBOARD
    else if (e == QKeySequence::Copy) {
            e->accept();
            q->copy();
            return;
    }
#endif
#endif // QT_NO_SHORTCUT

    if (interactionFlags & Qt::TextSelectableByKeyboard
        && cursorMoveKeyEvent(e))
        goto accept;

    if (!(interactionFlags & Qt::TextEditable)) {
        e->ignore();
        return;
    }

    if (e->key() == Qt::Key_Direction_L || e->key() == Qt::Key_Direction_R) {
        QTextBlockFormat fmt;
        fmt.setLayoutDirection((e->key() == Qt::Key_Direction_L) ? Qt::LeftToRight : Qt::RightToLeft);
        cursor.mergeBlockFormat(fmt);
        goto accept;
    }

    // schedule a repaint of the region of the cursor, as when we move it we
    // want to make sure the old cursor disappears (not noticeable when moving
    // only a few pixels but noticeable when jumping between cells in tables for
    // example)
    repaintSelection();

    if (e->key() == Qt::Key_Backspace && !(e->modifiers() & ~Qt::ShiftModifier)) {
        QTextBlockFormat blockFmt = cursor.blockFormat();
        QTextList *list = cursor.currentList();
        if (list && cursor.atBlockStart() && !cursor.hasSelection()) {
            list->remove(cursor.block());
        } else if (cursor.atBlockStart() && blockFmt.indent() > 0) {
            blockFmt.setIndent(blockFmt.indent() - 1);
            cursor.setBlockFormat(blockFmt);
        } else {
            QTextCursor localCursor = cursor;
            std::pair<int, int> result(-1, -1);
            if (textEdit && textEdit->getDocumentLayout())
            {
                result = textEdit->getDocumentLayout()->isLineAfterCollapsedSection(localCursor.block().blockNumber());
            }

            if (result.first != -1 && localCursor.atBlockStart())
            {
                textEdit->manageExpandCollapse(result.first, false);
            }
            else {
                localCursor.deletePreviousChar();
            }

        }
        goto accept;
    }
#ifndef QT_NO_SHORTCUT
      else if (e == QKeySequence::InsertParagraphSeparator) {
        cursor.insertBlock();
        e->accept();
        goto accept;
    } else if (e == QKeySequence::InsertLineSeparator) {
        cursor.insertText(QString(QChar::LineSeparator));
        e->accept();
        goto accept;
    }
#endif
    if (false) {
    }
#ifndef QT_NO_SHORTCUT
    else if (e == QKeySequence::Undo) {
            q->undo();
    }
    else if (e == QKeySequence::Redo) {
           q->redo();
    }
#ifndef QT_NO_CLIPBOARD
    else if (e == QKeySequence::Cut) {
           q->cut();
    }
    else if (e == QKeySequence::Paste) {
        QClipboard::Mode mode = QClipboard::Clipboard;
        q->paste(mode);
    }
#endif
    else if (e == QKeySequence::Delete) {
        QTextCursor localCursor = cursor;
        localCursor.deleteChar();
    }
    else if (e == QKeySequence::DeleteEndOfWord) {
        if (!cursor.hasSelection())
            cursor.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
    }
    else if (e == QKeySequence::DeleteStartOfWord) {
        if (!cursor.hasSelection())
            cursor.movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
    }
    else if (e == QKeySequence::DeleteEndOfLine) {
        QTextBlock block = cursor.block();
        if (cursor.position() == block.position() + block.length() - 2)
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        else
            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
    }
#endif // QT_NO_SHORTCUT
    else {
        goto process;
    }
    goto accept;

process:
    {
        QString text = e->text();
        if (!text.isEmpty() && (text.at(0).isPrint() || text.at(0) == QLatin1Char('\t'))) {
            cursor.insertText(text);
            selectionChanged();
        } else {
            e->ignore();
            return;
        }
    }

 accept:

    e->accept();
    cursorOn = true;

    q->updateCursorRectangle(true);
    updateCurrentCharFormat();
}

QRectF TextControlPrivate::rectForPosition(int position) const
{
    Q_Q(const TextControl);
    const QTextBlock block = doc->findBlock(position);
    if (!block.isValid())
        return QRectF();
    const QTextLayout *layout = block.layout();
    const QPointF layoutPos = q->blockBoundingRect(block).topLeft();
    int relativePos = position - block.position();
#ifndef QT_NO_IM
    if (preeditCursor != 0) {
        int preeditPos = layout->preeditAreaPosition();
        if (relativePos == preeditPos)
            relativePos += preeditCursor;
        else if (relativePos > preeditPos)
            relativePos += layout->preeditAreaText().length();
    }
#endif
    QTextLine line = layout->lineForTextPosition(relativePos);

    QRectF r;
    // int offset = textEdit->getPaletteManager()->drawingOffset(block.blockNumber(), true);
    int offset = 0;
    if (textEdit && textEdit->lineControl())
        offset = textEdit->lineControl()->drawingOffset(block.blockNumber(), true);
    if (line.isValid()) {
        qreal x = line.cursorToX(relativePos);
        qreal w = 0;

        r = QRectF(layoutPos.x() + x, layoutPos.y() + line.y() + offset, textCursorWidth + w, line.height());
    } else {
        r = QRectF(layoutPos.x(), layoutPos.y() + offset, textCursorWidth, 10); // #### correct height
    }

    return r;
}

void TextControlPrivate::mousePressEvent(QMouseEvent *e, const QPointF &pos)
{
    Q_Q(TextControl);

    mousePressed = (interactionFlags & Qt::TextSelectableByMouse) && (e->button() & Qt::LeftButton);
    mousePressPos = pos.toPoint();

    if (sendMouseEventToInputContext(e, pos))
        return;

    if (interactionFlags & Qt::LinksAccessibleByMouse) {
        anchorOnMousePress = q->anchorAt(pos);

        if (cursorIsFocusIndicator) {
            cursorIsFocusIndicator = false;
            repaintSelection();
            cursor.clearSelection();
        }
    }
    if (e->button() & Qt::MiddleButton) {
        return;
    } else  if (!(e->button() & Qt::LeftButton)) {
        e->ignore();
        return;
    } else if (!(interactionFlags & (Qt::TextSelectableByMouse | Qt::TextEditable))) {
        if (!(interactionFlags & Qt::LinksAccessibleByMouse))
            e->ignore();
        return;
    }

    cursorIsFocusIndicator = false;
    const QTextCursor oldSelection = cursor;
    const int oldCursorPos = cursor.position();

#ifndef QT_NO_IM
    commitPreedit();
#endif

    if (tripleClickTimer.isActive()
        && ((pos - tripleClickPoint).toPoint().manhattanLength() < QGuiApplication::styleHints()->startDragDistance())) {

        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        selectedBlockOnTripleClick = cursor;

        anchorOnMousePress = QString();

        tripleClickTimer.stop();
    } else {
        int cursorPos = q->hitTest(pos, Qt::FuzzyHit);
        if (cursorPos == -1) {
            e->ignore();
            return;
        }

        if (e->modifiers() == Qt::ShiftModifier && (interactionFlags & Qt::TextSelectableByMouse)) {
            if (wordSelectionEnabled && !selectedWordOnDoubleClick.hasSelection()) {
                selectedWordOnDoubleClick = cursor;
                selectedWordOnDoubleClick.select(QTextCursor::WordUnderCursor);
            }

            if (selectedBlockOnTripleClick.hasSelection())
                extendBlockwiseSelection(cursorPos);
            else if (selectedWordOnDoubleClick.hasSelection())
                extendWordwiseSelection(cursorPos, pos.x());
            else if (!wordSelectionEnabled)
                setCursorPosition(cursorPos, QTextCursor::KeepAnchor);
        } else {
            setCursorPosition(cursorPos);
        }
    }

    if (cursor.position() != oldCursorPos) {
        q->updateCursorRectangle(true);
        emit q->cursorPositionChanged();
    }
    if (interactionFlags & Qt::TextEditable)
        _q_updateCurrentCharFormatAndSelection();
    else
        selectionChanged();
    repaintOldAndNewSelection(oldSelection);
    hadSelectionOnMousePress = cursor.hasSelection();
}

void TextControlPrivate::mouseMoveEvent(QMouseEvent *e, const QPointF &mousePos)
{
    Q_Q(TextControl);

    if ((e->buttons() & Qt::LeftButton)) {
        const bool editable = interactionFlags & Qt::TextEditable;

        if (!(mousePressed
              || editable
              || selectedWordOnDoubleClick.hasSelection()
              || selectedBlockOnTripleClick.hasSelection()))
            return;

        const QTextCursor oldSelection = cursor;
        const int oldCursorPos = cursor.position();

        if (!mousePressed)
            return;

        const qreal mouseX = qreal(mousePos.x());

        int newCursorPos = q->hitTest(mousePos, Qt::FuzzyHit);

#ifndef QT_NO_IM
        if (isPreediting()) {
            // note: oldCursorPos not including preedit
            int selectionStartPos = q->hitTest(mousePressPos, Qt::FuzzyHit);
            if (newCursorPos != selectionStartPos) {
                commitPreedit();
                // commit invalidates positions
                newCursorPos = q->hitTest(mousePos, Qt::FuzzyHit);
                selectionStartPos = q->hitTest(mousePressPos, Qt::FuzzyHit);
                setCursorPosition(selectionStartPos);
            }
        }
#endif

        if (newCursorPos == -1)
            return;

        if (wordSelectionEnabled && !selectedWordOnDoubleClick.hasSelection()) {
            selectedWordOnDoubleClick = cursor;
            selectedWordOnDoubleClick.select(QTextCursor::WordUnderCursor);
        }

        if (selectedBlockOnTripleClick.hasSelection())
            extendBlockwiseSelection(newCursorPos);
        else if (selectedWordOnDoubleClick.hasSelection())
            extendWordwiseSelection(newCursorPos, mouseX);
#ifndef QT_NO_IM
        else if (!isPreediting())
            setCursorPosition(newCursorPos, QTextCursor::KeepAnchor);
#endif

        if (interactionFlags & Qt::TextEditable) {
            if (cursor.position() != oldCursorPos) {
                emit q->cursorPositionChanged();
                q->updateCursorRectangle(true);
            }
            _q_updateCurrentCharFormatAndSelection();
#ifndef QT_NO_IM
            if (qGuiApp)
                qGuiApp->inputMethod()->update(Qt::ImQueryInput);
#endif
        } else if (cursor.position() != oldCursorPos) {
            emit q->cursorPositionChanged();
            q->updateCursorRectangle(true);
        }
        selectionChanged(true);
        repaintOldAndNewSelection(oldSelection);
    }

    sendMouseEventToInputContext(e, mousePos);
}

void TextControlPrivate::mouseReleaseEvent(QMouseEvent *e, const QPointF &pos)
{
    Q_Q(TextControl);

    if (sendMouseEventToInputContext(e, pos))
        return;

    const QTextCursor oldSelection = cursor;
    const int oldCursorPos = cursor.position();

    if (mousePressed) {
        mousePressed = false;
#ifndef QT_NO_CLIPBOARD
        setClipboardSelection();
        selectionChanged(true);
    } else if (e->button() == Qt::MidButton
               && (interactionFlags & Qt::TextEditable)
               && QGuiApplication::clipboard()->supportsSelection()) {
        setCursorPosition(pos);
        const QMimeData *md = QGuiApplication::clipboard()->mimeData(QClipboard::Selection);
        if (md)
            q->insertFromMimeData(md);
#endif
    }

    repaintOldAndNewSelection(oldSelection);

    if (cursor.position() != oldCursorPos) {
        emit q->cursorPositionChanged();
        q->updateCursorRectangle(true);
    }

    if (interactionFlags & Qt::LinksAccessibleByMouse) {
        if (!(e->button() & Qt::LeftButton))
            return;

        const QString anchor = q->anchorAt(pos);

        if (anchor.isEmpty())
            return;

        if (!cursor.hasSelection()
            || (anchor == anchorOnMousePress && hadSelectionOnMousePress)) {

            const int anchorPos = q->hitTest(pos, Qt::ExactHit);
            if (anchorPos != -1) {
                cursor.setPosition(anchorPos);

                QString anchor = anchorOnMousePress;
                anchorOnMousePress = QString();
                activateLinkUnderCursor(anchor);
            }
        }
    }
}

void TextControlPrivate::mouseDoubleClickEvent(QMouseEvent *e, const QPointF &pos)
{
    Q_Q(TextControl);

    if (e->button() == Qt::LeftButton && (interactionFlags & Qt::TextSelectableByMouse)) {
#ifndef QT_NO_IM
        commitPreedit();
#endif

        const QTextCursor oldSelection = cursor;
        setCursorPosition(pos);
        QTextLine line = currentTextLine(cursor);
        bool doEmit = false;
        if (line.isValid() && line.textLength()) {
            cursor.select(QTextCursor::WordUnderCursor);
            doEmit = true;
        }
        repaintOldAndNewSelection(oldSelection);

        cursorIsFocusIndicator = false;
        selectedWordOnDoubleClick = cursor;

        tripleClickPoint = pos;
        tripleClickTimer.start(QGuiApplication::styleHints()->mouseDoubleClickInterval(), q);
        if (doEmit) {
            selectionChanged();
#ifndef QT_NO_CLIPBOARD
            setClipboardSelection();
#endif
            emit q->cursorPositionChanged();
            q->updateCursorRectangle(true);
        }
    } else if (!sendMouseEventToInputContext(e, pos)) {
        e->ignore();
    }
}

bool TextControlPrivate::sendMouseEventToInputContext(QMouseEvent *e, const QPointF &pos)
{
#if !defined(QT_NO_IM)
    Q_Q(TextControl);

    Q_UNUSED(e);

    if (isPreediting()) {
        QTextLayout *layout = cursor.block().layout();
        int cursorPos = q->hitTest(pos, Qt::FuzzyHit) - cursor.position();

        if (cursorPos >= 0 && cursorPos <= layout->preeditAreaText().length()) {
            if (e->type() == QEvent::MouseButtonRelease) {
                QGuiApplication::inputMethod()->invokeAction(QInputMethod::Click, cursorPos);
            }

            return true;
        }
    }
#else
    Q_UNUSED(e);
    Q_UNUSED(pos);
#endif
    return false;
}

#ifndef QT_NO_IM
void TextControlPrivate::inputMethodEvent(QInputMethodEvent *e)
{
    Q_Q(TextControl);
    if (!(interactionFlags & Qt::TextEditable) || cursor.isNull()) {
        e->ignore();
        return;
    }
    bool isGettingInput = !e->commitString().isEmpty()
            || e->preeditString() != cursor.block().layout()->preeditAreaText()
            || e->replacementLength() > 0;
    bool forceSelectionChanged = false;

    cursor.beginEditBlock();
    if (isGettingInput) {
        cursor.removeSelectedText();
    }

    // insert commit string
    if (!e->commitString().isEmpty() || e->replacementLength()) {
        QTextCursor c = cursor;
        c.setPosition(c.position() + e->replacementStart());
        c.setPosition(c.position() + e->replacementLength(), QTextCursor::KeepAnchor);
        c.insertText(e->commitString());
    }

    for (int i = 0; i < e->attributes().size(); ++i) {
        const QInputMethodEvent::Attribute &a = e->attributes().at(i);
        if (a.type == QInputMethodEvent::Selection) {
            QTextCursor oldCursor = cursor;
            int blockStart = a.start + cursor.block().position();
            cursor.setPosition(blockStart, QTextCursor::MoveAnchor);
            cursor.setPosition(blockStart + a.length, QTextCursor::KeepAnchor);
            repaintOldAndNewSelection(oldCursor);
            forceSelectionChanged = true;
        }
    }

    QTextBlock block = cursor.block();
    QTextLayout *layout = block.layout();
    if (isGettingInput) {
        layout->setPreeditArea(cursor.position() - block.position(), e->preeditString());
        emit q->preeditTextChanged();
    }
    QVector<QTextLayout::FormatRange> overrides;
    const int oldPreeditCursor = preeditCursor;
    preeditCursor = e->preeditString().length();
    hasImState = !e->preeditString().isEmpty();
    cursorVisible = true;
    for (int i = 0; i < e->attributes().size(); ++i) {
        const QInputMethodEvent::Attribute &a = e->attributes().at(i);
        if (a.type == QInputMethodEvent::Cursor) {
            hasImState = true;
            preeditCursor = a.start;
            cursorVisible = a.length != 0;
        } else if (a.type == QInputMethodEvent::TextFormat) {
            hasImState = true;
            QTextCharFormat f = qvariant_cast<QTextFormat>(a.value).toCharFormat();
            if (f.isValid()) {
                QTextLayout::FormatRange o;
                o.start = a.start + cursor.position() - block.position();
                o.length = a.length;
                o.format = f;
                overrides.append(o);
            }
        }
    }
    layout->setFormats(overrides);

    cursor.endEditBlock();


    // THIS IS TO BE REMOVED
//    QTextCursorPrivate *cursor_d = QTextCursorPrivate::getPrivate(&cursor);
//    if (cursor_d)
//        cursor_d->setX();

    // TO BE REMOVED


    q->updateCursorRectangle(oldPreeditCursor != preeditCursor || forceSelectionChanged || isGettingInput);
    selectionChanged(forceSelectionChanged);
}

QVariant TextControl::inputMethodQuery(Qt::InputMethodQuery property) const
{
    return inputMethodQuery(property, QVariant());
}

QVariant TextControl::inputMethodQuery(Qt::InputMethodQuery property, QVariant argument) const
{
    Q_D(const TextControl);
    QTextBlock block = d->cursor.block();
    switch (property) {
    case Qt::ImCursorRectangle:
        return cursorRect();
    case Qt::ImAnchorRectangle:
        return anchorRect();
    case Qt::ImFont:
        return QVariant(d->cursor.charFormat().font());
    case Qt::ImCursorPosition: {
        const QPointF pt = argument.toPointF();
        if (!pt.isNull())
            return QVariant(d->doc->documentLayout()->hitTest(pt, Qt::FuzzyHit) - block.position());
        return QVariant(d->cursor.position() - block.position());
    }
    case Qt::ImSurroundingText:
        return QVariant(block.text());
    case Qt::ImCurrentSelection:
        return QVariant(d->cursor.selectedText());
    case Qt::ImMaximumTextLength:
        return QVariant(); // No limit.
    case Qt::ImAnchorPosition:
        return QVariant(d->cursor.anchor() - block.position());
    case Qt::ImAbsolutePosition:
        return QVariant(d->cursor.anchor());
    case Qt::ImTextAfterCursor:
    {
        int maxLength = argument.isValid() ? argument.toInt() : 1024;
        QTextCursor tmpCursor = d->cursor;
        int localPos = d->cursor.position() - block.position();
        QString result = block.text().mid(localPos);
        while (result.length() < maxLength) {
            int currentBlock = tmpCursor.blockNumber();
            tmpCursor.movePosition(QTextCursor::NextBlock);
            if (tmpCursor.blockNumber() == currentBlock)
                break;
            result += QLatin1Char('\n') + tmpCursor.block().text();
        }
        return QVariant(result);
    }
    case Qt::ImTextBeforeCursor:
    {
        int maxLength = argument.isValid() ? argument.toInt() : 1024;
        QTextCursor tmpCursor = d->cursor;
        int localPos = d->cursor.position() - block.position();
        int numBlocks = 0;
        int resultLen = localPos;
        while (resultLen < maxLength) {
            int currentBlock = tmpCursor.blockNumber();
            tmpCursor.movePosition(QTextCursor::PreviousBlock);
            if (tmpCursor.blockNumber() == currentBlock)
                break;
            numBlocks++;
            resultLen += tmpCursor.block().length();
        }
        QString result;
        while (numBlocks) {
            result += tmpCursor.block().text() + QLatin1Char('\n');
            tmpCursor.movePosition(QTextCursor::NextBlock);
            --numBlocks;
        }
        result += block.text().midRef(0,localPos);
        return QVariant(result);
    }
    default:
        return QVariant();
    }
}
#endif // QT_NO_IM

void TextControlPrivate::focusEvent(QFocusEvent *e)
{
    Q_Q(TextControl);
    emit q->updateRequest();
    hasFocus = e->gotFocus();
    if (e->gotFocus()) {
        setBlinkingCursorEnabled(interactionFlags & (Qt::TextEditable | Qt::TextSelectableByKeyboard));
    } else {
        setBlinkingCursorEnabled(false);

        if (cursorIsFocusIndicator
            && e->reason() != Qt::ActiveWindowFocusReason
            && e->reason() != Qt::PopupFocusReason
            && cursor.hasSelection() && clearSelectionOnFocus)
        {
            cursor.clearSelection();
            emit q->selectionChanged();
        }
    }
}

void TextControlPrivate::hoverEvent(QHoverEvent *e, const QPointF &pos)
{
    Q_Q(TextControl);

    QString link;
    if (e->type() != QEvent::HoverLeave)
        link = q->anchorAt(pos);

    if (hoveredLink != link) {
        hoveredLink = link;
        emit q->linkHovered(link);
    }
}

bool TextControl::hasImState() const
{
    Q_D(const TextControl);
    return d->hasImState;
}

bool TextControl::cursorVisible() const
{
    Q_D(const TextControl);
    return d->cursorVisible;
}

void TextControl::setCursorVisible(bool visible)
{
    Q_D(TextControl);
    d->cursorVisible = visible;
    d->setBlinkingCursorEnabled(d->cursorVisible
            && (d->interactionFlags & (Qt::TextEditable | Qt::TextSelectableByKeyboard)));
}

QRectF TextControl::anchorRect() const
{
    Q_D(const TextControl);
    QRectF rect;
    QTextCursor cursor = d->cursor;
    if (!cursor.isNull()) {
        rect = d->rectForPosition(cursor.anchor());
    }
    return rect;
}

QRectF TextControl::cursorRect(const QTextCursor &cursor) const
{
    Q_D(const TextControl);
    if (cursor.isNull())
        return QRectF();

    return d->rectForPosition(cursor.position());
}

QRectF TextControl::cursorRect() const
{
    Q_D(const TextControl);
    return cursorRect(d->cursor);
}

QString TextControl::hoveredLink() const
{
    Q_D(const TextControl);
    return d->hoveredLink;
}

QString TextControl::anchorAt(const QPointF &pos) const
{
    Q_D(const TextControl);
    return d->doc->documentLayout()->anchorAt(pos);
}

void TextControl::setAcceptRichText(bool accept)
{
    Q_D(TextControl);
    d->acceptRichText = accept;
}

void TextControl::moveCursor(QTextCursor::MoveOperation op, QTextCursor::MoveMode mode)
{
    Q_D(TextControl);
    const QTextCursor oldSelection = d->cursor;
    const bool moved = d->cursor.movePosition(op, mode);
    d->_q_updateCurrentCharFormatAndSelection();
    updateCursorRectangle(true);
    d->repaintOldAndNewSelection(oldSelection);
    if (moved)
        emit cursorPositionChanged();
}

bool TextControl::canPaste() const
{
#ifndef QT_NO_CLIPBOARD
    Q_D(const TextControl);
    if (d->interactionFlags & Qt::TextEditable) {
        const QMimeData *md = QGuiApplication::clipboard()->mimeData();
        return md && canInsertFromMimeData(md);
    }
#endif
    return false;
}

void TextControl::setCursorIsFocusIndicator(bool b)
{
    Q_D(TextControl);
    d->cursorIsFocusIndicator = b;
    d->repaintCursor();
}

void TextControl::setWordSelectionEnabled(bool enabled)
{
    Q_D(TextControl);
    d->wordSelectionEnabled = enabled;
}

QMimeData *TextControl::createMimeDataFromSelection() const
{
    Q_D(const TextControl);
    const QTextDocumentFragment fragment(d->cursor);
    return new TextEditMimeData(fragment);
}

bool TextControl::canInsertFromMimeData(const QMimeData *source) const
{
    Q_D(const TextControl);
    if (d->acceptRichText)
        return source->hasText()
            || source->hasHtml()
            || source->hasFormat(QLatin1String("application/x-qrichtext"))
            || source->hasFormat(QLatin1String("application/x-qt-richtext"));
    else
        return source->hasText();
}

void TextControl::insertFromMimeData(const QMimeData *source)
{
    Q_D(TextControl);
    if (!(d->interactionFlags & Qt::TextEditable) || !source)
        return;

    bool hasData = false;
    QTextDocumentFragment fragment;
#ifndef QT_NO_TEXTHTMLPARSER
    if (source->hasFormat(QLatin1String("application/x-qrichtext")) && d->acceptRichText) {
        // x-qrichtext is always UTF-8 (taken from Qt3 since we don't use it anymore).
        QString richtext = QString::fromUtf8(source->data(QLatin1String("application/x-qrichtext")));
        richtext.prepend(QLatin1String("<meta name=\"qrichtext\" content=\"1\" />"));
        fragment = QTextDocumentFragment::fromHtml(richtext, d->doc);
        hasData = true;
    } else if (source->hasHtml() && d->acceptRichText) {
        fragment = QTextDocumentFragment::fromHtml(source->html(), d->doc);
        hasData = true;
    } else {
        QString text = source->text();
        if (!text.isNull()) {
            fragment = QTextDocumentFragment::fromPlainText(text);
            hasData = true;
        }
    }
#else
    fragment = QTextDocumentFragment::fromPlainText(source->text());
#endif // QT_NO_TEXTHTMLPARSER

    if (hasData)
        d->cursor.insertFragment(fragment);
    updateCursorRectangle(true);
}

void TextControlPrivate::activateLinkUnderCursor(QString href)
{
    QTextCursor oldCursor = cursor;

    if (href.isEmpty()) {
        QTextCursor tmp = cursor;
        if (tmp.selectionStart() != tmp.position())
            tmp.setPosition(tmp.selectionStart());
        tmp.movePosition(QTextCursor::NextCharacter);
        href = tmp.charFormat().anchorHref();
    }
    if (href.isEmpty())
        return;

    if (!cursor.hasSelection()) {
        QTextBlock block = cursor.block();
        const int cursorPos = cursor.position();

        QTextBlock::Iterator it = block.begin();
        QTextBlock::Iterator linkFragment;

        for (; !it.atEnd(); ++it) {
            QTextFragment fragment = it.fragment();
            const int fragmentPos = fragment.position();
            if (fragmentPos <= cursorPos &&
                fragmentPos + fragment.length() > cursorPos) {
                linkFragment = it;
                break;
            }
        }

        if (!linkFragment.atEnd()) {
            it = linkFragment;
            cursor.setPosition(it.fragment().position());
            if (it != block.begin()) {
                do {
                    --it;
                    QTextFragment fragment = it.fragment();
                    if (fragment.charFormat().anchorHref() != href)
                        break;
                    cursor.setPosition(fragment.position());
                } while (it != block.begin());
            }

            for (it = linkFragment; !it.atEnd(); ++it) {
                QTextFragment fragment = it.fragment();
                if (fragment.charFormat().anchorHref() != href)
                    break;
                cursor.setPosition(fragment.position() + fragment.length(), QTextCursor::KeepAnchor);
            }
        }
    }

    if (hasFocus) {
        cursorIsFocusIndicator = true;
    } else {
        cursorIsFocusIndicator = false;
        cursor.clearSelection();
    }
    repaintOldAndNewSelection(oldCursor);

    emit q_func()->linkActivated(href);
}

#ifndef QT_NO_IM
bool TextControlPrivate::isPreediting() const
{
    QTextLayout *layout = cursor.block().layout();
    if (layout && !layout->preeditAreaText().isEmpty())
        return true;

    return false;
}

void TextControlPrivate::commitPreedit()
{
    Q_Q(TextControl);

    if (!hasImState)
        return;

    QGuiApplication::inputMethod()->commit();

    if (!hasImState)
        return;

    QInputMethodEvent event;
    QCoreApplication::sendEvent(q->parent(), &event);
}

void TextControlPrivate::cancelPreedit()
{
    Q_Q(TextControl);

    if (!hasImState)
        return;

    QGuiApplication::inputMethod()->reset();

    QInputMethodEvent event;
    QCoreApplication::sendEvent(q->parent(), &event);
}
#endif // QT_NO_IM

void TextControl::setTextEdit(TextEdit* te)
{
    Q_D(TextControl);
    d->textEdit = te;
}

void TextControl::setTextInteractionFlags(Qt::TextInteractionFlags flags)
{
    Q_D(TextControl);
    if (flags == d->interactionFlags)
        return;
    d->interactionFlags = flags;

    if (d->hasFocus)
        d->setBlinkingCursorEnabled(flags & (Qt::TextEditable | Qt::TextSelectableByKeyboard));
}

Qt::TextInteractionFlags TextControl::textInteractionFlags() const
{
    Q_D(const TextControl);
    return d->interactionFlags;
}

QString TextControl::toPlainText() const
{
    return document()->toPlainText();
}

#ifndef QT_NO_TEXTHTMLPARSER
QString TextControl::toHtml() const
{
    return document()->toHtml();
}
#endif

bool TextControl::cursorOn() const
{
    Q_D(const TextControl);
    return d->cursorOn;
}

int TextControl::hitTest(const QPointF &point, Qt::HitTestAccuracy accuracy) const
{
    Q_D(const TextControl);
    return d->doc->documentLayout()->hitTest(point, accuracy);
}

QRectF TextControl::blockBoundingRect(const QTextBlock &block) const
{
    Q_D(const TextControl);
    return d->doc->documentLayout()->blockBoundingRect(block);
}

QString TextControl::preeditText() const
{
#ifndef QT_NO_IM
    Q_D(const TextControl);
    QTextLayout *layout = d->cursor.block().layout();
    if (!layout)
        return QString();

    return layout->preeditAreaText();
#else
    return QString();
#endif
}


QStringList TextEditMimeData::formats() const
{
    if (!fragment.isEmpty())
        return QStringList() << QString::fromLatin1("text/plain") << QString::fromLatin1("text/html")
#ifndef QT_NO_TEXTODFWRITER
            << QString::fromLatin1("application/vnd.oasis.opendocument.text")
#endif
        ;
    else
        return QMimeData::formats();
}

QVariant TextEditMimeData::retrieveData(const QString &mimeType, QVariant::Type type) const
{
    if (!fragment.isEmpty())
        setup();
    return QMimeData::retrieveData(mimeType, type);
}

void TextEditMimeData::setup() const
{
    TextEditMimeData *that = const_cast<TextEditMimeData *>(this);
#ifndef QT_NO_TEXTHTMLPARSER
    that->setData(QLatin1String("text/html"), fragment.toHtml("utf-8").toUtf8());
#endif
#ifndef QT_NO_TEXTODFWRITER
    {
        QBuffer buffer;
        QTextDocumentWriter writer(&buffer, "ODF");
        writer.write(fragment);
        buffer.close();
        that->setData(QLatin1String("application/vnd.oasis.opendocument.text"), buffer.data());
    }
#endif
    that->setText(fragment.toPlainText());
    fragment = QTextDocumentFragment();
}


}

#include "moc_textcontrol_p.cpp"

#endif // QT_NO_TEXTCONTROL
