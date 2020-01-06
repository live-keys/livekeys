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

#include "syntaxhighlighter.h"
#include <qtextdocument.h>
#include <qtextlayout.h>
#include <qpointer.h>
#include <qtextobject.h>
#include <qtextcursor.h>
#include <qdebug.h>
#include <qtimer.h>
#include "live/projectdocument.h"
#include <algorithm>

namespace lv{

class SyntaxHighlighterPrivate{

    Q_DECLARE_PUBLIC(SyntaxHighlighter)

public:
    inline SyntaxHighlighterPrivate(SyntaxHighlighter* qptr)
        : q_ptr(qptr), rehighlightPending(false), inReformatBlocks(false)
    {}

    QPointer<QTextDocument> doc;

    void _q_reformatBlocks(int from, int charsRemoved, int charsAdded);
    void reformatBlocks(int from, int charsRemoved, int charsAdded);
//    bool reformatBlock(const QTextBlock &block);

    inline void rehighlight(QTextCursor &cursor, QTextCursor::MoveOperation operation) {
        inReformatBlocks = true;
        cursor.beginEditBlock();
        int from = cursor.position();
        cursor.movePosition(operation);
        reformatBlocks(from, 0, cursor.position() - from);
        cursor.endEditBlock();
        inReformatBlocks = false;
    }

    inline void _q_delayedRehighlight() {
        if (!rehighlightPending)
            return;
        rehighlightPending = false;
        q_func()->rehighlight();
    }

    void distributeFormats(QTextBlock startBlock, QList<SyntaxHighlighter::TextFormatRange>& textFormatRanges, bool setStates = true);
    SyntaxHighlighter* q_ptr;

    bool applyFormatChanges();
    QVector<QTextCharFormat> formatChanges;
    QTextBlock currentBlock;
    bool rehighlightPending;
    bool inReformatBlocks;

private:
    void populateSectionList(QList<ProjectDocumentSection::Ptr> &sectionList, ProjectDocument* projectDocument, int start, int end);
};

bool SyntaxHighlighterPrivate::applyFormatChanges()
{
    bool formatsChanged = false;

    QTextLayout *layout = currentBlock.layout();

    QVector<QTextLayout::FormatRange> ranges = layout->formats();

    const int preeditAreaStart = layout->preeditAreaPosition();
    const int preeditAreaLength = layout->preeditAreaText().length();

    if (preeditAreaLength != 0) {
        auto isOutsidePreeditArea = [=](const QTextLayout::FormatRange &range) {
            return range.start < preeditAreaStart
                    || range.start + range.length > preeditAreaStart + preeditAreaLength;
        };
        const auto it = std::remove_if(ranges.begin(), ranges.end(),
                                       isOutsidePreeditArea);
        if (it != ranges.end()) {
            ranges.erase(it, ranges.end());
            formatsChanged = true;
        }
    } else if (!ranges.isEmpty()) {
        ranges.clear();
        formatsChanged = true;
    }

    int i = 0;
    while (i < formatChanges.count()) {
        QTextLayout::FormatRange r;

        while (i < formatChanges.count() && formatChanges.at(i) == r.format)
            ++i;

        if (i == formatChanges.count())
            break;

        r.start = i;
        r.format = formatChanges.at(i);

        while (i < formatChanges.count() && formatChanges.at(i) == r.format)
            ++i;

        Q_ASSERT(i <= formatChanges.count());
        r.length = i - r.start;

        if (preeditAreaLength != 0) {
            if (r.start >= preeditAreaStart)
                r.start += preeditAreaLength;
            else if (r.start + r.length >= preeditAreaStart)
                r.length += preeditAreaLength;
        }

        ranges << r;
        formatsChanged = true;
    }

    if (formatsChanged) {
        layout->setFormats(ranges);
        // moved this to reformatBlocks, to not trigger a document refresh after each block
//        doc->markContentsDirty(currentBlock.position(), currentBlock.length());
    }

    return formatsChanged;
}

void SyntaxHighlighterPrivate::populateSectionList(QList<ProjectDocumentSection::Ptr> &sectionList, ProjectDocument* projectDocument, int start, int end)
{
    for (auto iter  = projectDocument->sectionsBegin();
              iter != projectDocument->sectionsEnd();
              ++iter)
    {
        int secStart = iter->data()->position();
        int secEnd = secStart + iter->data()->length();

        bool toBeAdded = start <= secStart && secStart <= end;
        toBeAdded = toBeAdded || (start <= secEnd && secEnd <= end);
        toBeAdded = toBeAdded || (secStart <= start && end <= secEnd);

        if (toBeAdded) sectionList.push_back(*iter);
    }
}

QString SyntaxHighlighter::stateToString(int state)
{
    if (state == -1) return "blank";
    int regexp = state & 4;
    state = state & 3;

    QString result = "";
    if (regexp != 0) result += "regexp, ";

    switch (state)
    {
    case 0: result += "normal"; break;
    case 1: result += "multiline comment"; break;
    case 2: result += "multiline string - double quote"; break;
    case 3: result += "multiline string - single quote"; break;
    default: break;
    }

    return result;
}

void SyntaxHighlighterPrivate::_q_reformatBlocks(int from, int charsRemoved, int charsAdded)
{
    if (!inReformatBlocks)
        reformatBlocks(from, charsRemoved, charsAdded);
}

void SyntaxHighlighterPrivate::reformatBlocks(int from, int charsRemoved, int charsAdded)
{
    Q_Q(SyntaxHighlighter);
    rehighlightPending = false;

    QTextBlock block = doc->findBlock(from);
    if (!block.isValid())
        return;

    int startPosition, endPosition;
    QTextBlock lastBlock = doc->findBlock(from + charsAdded + (charsRemoved > 0 ? 1 : 0));
    if (!lastBlock.isValid()) lastBlock = doc->lastBlock();
    endPosition = lastBlock.position() + lastBlock.length();

    int formatsChangedStartPosition = -1;
    int formatsChangedEndPosition = from;

    int prevState = -1;

    QString text = "";
    auto startBlock = block;
    while (block.isValid() && (block.position() < endPosition)) {

        if (prevState == -1)
        {
            auto prevBlock = block.previous();
            if (prevBlock.isValid())
            {
                prevState = prevBlock.userState();
            } else prevState = 0;
        }

        text += block.text();

        if (block != lastBlock) text += "\n"; // check how QTextCursor selects this

        if ( formatsChangedStartPosition == -1 ) {
            formatsChangedStartPosition = block.position();
        }
        formatsChangedEndPosition = block.position() + block.length();

        block = block.next();
    }

    int lastBlockState = lastBlock.userState();
    auto textFormatRangeList = q->highlight(prevState, formatsChangedStartPosition, text);
    distributeFormats(startBlock, textFormatRangeList);

    /*if (!sectionList.empty())
    {
        textFormatRangeList.clear();
        textFormatRangeList = q->highlightSections(sectionList);
        distributeFormats(startBlock, textFormatRangeList);
        sectionList.clear();
    }*/

    int size = 0;
    while (lastBlockState != lastBlock.userState()) // force highlight of next batch
    {

        textFormatRangeList.clear();
        QString text = "";
        size = (size == 0) ? 1 : 2*size;

        prevState = lastBlock.userState();
        startBlock = lastBlock.next();

        if (startBlock == doc->end()) break;

        lastBlock = startBlock;
        for (int i = 0; i < size && lastBlock != doc->end(); ++i)
        {
            text += lastBlock.text();

            text += "\n";
            formatsChangedEndPosition = lastBlock.position() + lastBlock.length();
            lastBlock = lastBlock.next();
        }
        lastBlock = lastBlock.previous();
        text = text.left(text.length()-1);

        lastBlockState = lastBlock.userState();
        textFormatRangeList = q->highlight(prevState, startBlock.position(), text);
        distributeFormats(startBlock, textFormatRangeList);

    }

    block = doc->findBlock(from);
    startPosition = block.position();
    QList<ProjectDocumentSection::Ptr> sectionList;
    auto projectDocument = static_cast<ProjectDocument*>(doc->parent());
    populateSectionList(sectionList, projectDocument, startPosition, endPosition);
    textFormatRangeList = q->highlightSections(sectionList);
    distributeFormats(block, textFormatRangeList);


    if ( formatsChangedStartPosition > -1 ){
        doc->markContentsDirty(formatsChangedStartPosition, formatsChangedEndPosition - formatsChangedStartPosition);
    }

    formatChanges.clear();
}

//bool SyntaxHighlighterPrivate::reformatBlock(const QTextBlock &block)
//{
//    Q_Q(SyntaxHighlighter);

//    Q_ASSERT_X(!currentBlock.isValid(), "SyntaxHighlighter::reformatBlock()", "reFormatBlock() called recursively");

//    currentBlock = block;

//    formatChanges.fill(QTextCharFormat(), block.length() - 1);
//    q->highlightBlock(block.text());
//    bool formatsChanged = applyFormatChanges();

//    currentBlock = QTextBlock();

//    return formatsChanged;
//}

void SyntaxHighlighterPrivate::distributeFormats(QTextBlock startBlock, QList<SyntaxHighlighter::TextFormatRange> &textFormatRanges, bool setStates)
{
    auto currentBlock = startBlock;
    QTextLayout* layout = currentBlock.layout();
    QVector<QTextLayout::FormatRange> ranges;// = layout->formats();

    bool blockSwitched = true;
    bool formatsChanged = false;
    int stateToSet = -1;
//    int preeditAreaStart = layout->preeditAreaPosition();
//    int preeditAreaLength = layout->preeditAreaText().length();
    bool shouldCollapse = false;
    CollapseFunctionType func = nullptr;

    ProjectDocument* projectDocument = static_cast<ProjectDocument*>(doc->parent());

    for (auto tfr: textFormatRanges)
    {
        while (currentBlock.isValid() && (tfr.start < currentBlock.position() || tfr.start >= currentBlock.position() + currentBlock.length()))
        {
            if (formatsChanged)
            {
                layout->setFormats(ranges);
                ProjectDocumentBlockData* bd = static_cast<ProjectDocumentBlockData*>(currentBlock.userData());

                if (setStates && (stateToSet & 15) != 0 && bd && bd->isCollapsible())
                {
                    projectDocument->resetCollapseSignal(currentBlock.blockNumber());
                }
                if (!bd){

                    bd = new ProjectDocumentBlockData;
                    currentBlock.setUserData(bd);
                }
                if (shouldCollapse)
                {
                    bd->setCollapsible(true);
                    bd->setCollapse(func);
                } else bd->setCollapsible(false);
                if (setStates) currentBlock.setUserState(stateToSet);
                formatsChanged = false;
            }
            currentBlock = currentBlock.next();

            if (!currentBlock.isValid())
            {
                layout = nullptr;
                //ranges = QVector<QTextLayout::FormatRange>();
            }
            else {
                layout = currentBlock.layout();
                //ranges = layout->formats();
            }

            ranges = QVector<QTextLayout::FormatRange>();
            blockSwitched = true;
            shouldCollapse = false;
            func = nullptr;
        }

//        if (blockSwitched && currentBlock.isValid() && layout)
//        {
//            preeditAreaStart = layout->preeditAreaPosition();
//            preeditAreaLength = layout->preeditAreaText().length();

//            if (preeditAreaLength != 0) {
//                auto isOutsidePreeditArea = [=](const QTextLayout::FormatRange &range) {
//                    return range.start < preeditAreaStart
//                            || range.start + range.length > preeditAreaStart + preeditAreaLength;
//                };
//                const auto it = std::remove_if(ranges.begin(), ranges.end(),
//                                               isOutsidePreeditArea);
//                if (it != ranges.end()) {
//                    ranges.erase(it, ranges.end());
//                    formatsChanged = true;
//                }
//            } else if (!ranges.isEmpty()) {
//                ranges.clear();
//                formatsChanged = true;
//            }
//        }

        QTextLayout::FormatRange formatRange;
        if (currentBlock.isValid()){
            if (tfr.start + tfr.length <= currentBlock.position() + currentBlock.length()) // single block case
            {
                formatRange.start = tfr.start - currentBlock.position();
                formatRange.length = tfr.length;
                formatRange.format = tfr.format;

                ranges << formatRange;
                formatsChanged = true;
                if (tfr.function)
                {
                    shouldCollapse = tfr.collapsible;
                    func = tfr.function;
                }
                if (setStates) stateToSet = tfr.userstateFollows;
            } else {

                // handle end of current block
                formatRange.start = tfr.start - currentBlock.position();
                formatRange.length = currentBlock.position() + currentBlock.length() - tfr.start;
                formatRange.format = tfr.format;

                ranges << formatRange;
                layout->setFormats(ranges);
                ProjectDocumentBlockData* bd = static_cast<ProjectDocumentBlockData*>(currentBlock.userData());

                if (setStates && (tfr.userstate & 15) != 0 && bd && bd->isCollapsible())
                {
                    projectDocument->resetCollapseSignal(currentBlock.blockNumber());
                }

                if (!bd){

                    bd = new ProjectDocumentBlockData;
                    currentBlock.setUserData(bd);
                }
                bd->setCollapsible(tfr.collapsible);
                bd->setCollapse(tfr.function);
                if (setStates) currentBlock.setUserState(tfr.userstate);

                int end = tfr.start + tfr.length;
                while (true) // handle blocks contained within range completely
                {
                    currentBlock = currentBlock.next();

                    layout = currentBlock.layout();
                    if (!currentBlock.isValid() || !layout) break;
                    // ranges = layout->formats();
                    ranges = QVector<QTextLayout::FormatRange>();

                    shouldCollapse = false;

                    if (end <= currentBlock.position()) break;

                    formatRange.start = 0;
                    formatRange.length = currentBlock.length();
                    formatRange.format = tfr.format;

                    ranges << formatRange;
                    layout->setFormats(ranges);

                    ProjectDocumentBlockData* bd = static_cast<ProjectDocumentBlockData*>(currentBlock.userData());
                    if (setStates && (tfr.userstate & 15) != 0 && bd && bd->isCollapsible())
                    {
                        projectDocument->resetCollapseSignal(currentBlock.blockNumber());
                    }
                    if (setStates) currentBlock.setUserState(tfr.userstate);
                }

                if (currentBlock.isValid() && end < currentBlock.position())
                {
                    // handle block where range ends
                    formatRange.start = 0;
                    formatRange.length = tfr.start+tfr.length - currentBlock.position();
                    formatRange.format = tfr.format;

                    ranges << formatRange;
                    formatsChanged = true;

                    bd = static_cast<ProjectDocumentBlockData*>(currentBlock.userData());
                    if (!bd){

                        bd = new ProjectDocumentBlockData;
                        currentBlock.setUserData(bd);
                    }

                    bd->setCollapsible(tfr.collapsible);
                    bd->setCollapse(tfr.function);
                    if (setStates) stateToSet = tfr.userstateFollows;

                    // tbd
                }

            }
        }
        blockSwitched = false;
    }

    if (formatsChanged && layout)
    {
        layout->setFormats(ranges);

        if (!currentBlock.isValid()) return;
        ProjectDocumentBlockData* bd = static_cast<ProjectDocumentBlockData*>(currentBlock.userData());
        if (setStates && (stateToSet & 15) != 0 && bd && bd->isCollapsible())
        {
            projectDocument->resetCollapseSignal(currentBlock.blockNumber());
        }
        if (!bd){

            bd = new ProjectDocumentBlockData;
            currentBlock.setUserData(bd);
        }
        if (shouldCollapse)
        {
            bd->setCollapsible(true);
            bd->setCollapse(func);
        } else bd->setCollapsible(false);
        if (setStates) currentBlock.setUserState(stateToSet);

        // formatsChanged = false;
    }

}

SyntaxHighlighter::SyntaxHighlighter(QObject *parent)
    : QObject(parent)
    , d_ptr(new SyntaxHighlighterPrivate(this))
{
    if (parent->inherits("QTextEdit")) {
        QTextDocument *doc = parent->property("document").value<QTextDocument *>();
        if (doc)
            setDocument(doc);
    }
}

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QObject(parent)
    , d_ptr(new SyntaxHighlighterPrivate(this))
{
    setDocument(parent);
}

SyntaxHighlighter::~SyntaxHighlighter()
{
    setDocument(nullptr);
}

void SyntaxHighlighter::setDocument(QTextDocument *doc)
{
    Q_D(SyntaxHighlighter);
    if (d->doc) {
        disconnect(d->doc, SIGNAL(contentsChange(int,int,int)),
                   this, SLOT(_q_reformatBlocks(int,int,int)));

        QTextCursor cursor(d->doc);
        cursor.beginEditBlock();
        for (QTextBlock blk = d->doc->begin(); blk.isValid(); blk = blk.next())
        {
            blk.layout()->setFormats(QVector<QTextLayout::FormatRange>());
            d->doc->markContentsDirty(blk.position(), blk.length());
        }
        //{ blk.layout()->clearFormats(); }
        cursor.endEditBlock();
    }

    d->doc = doc;
    if (d->doc) {
        connect(d->doc, SIGNAL(contentsChange(int,int,int)),
                this, SLOT(_q_reformatBlocks(int,int,int)));
        d->rehighlightPending = true;
        QTimer::singleShot(0, this, SLOT(_q_delayedRehighlight()));
    }
}

QTextDocument *SyntaxHighlighter::document() const
{
    Q_D(const SyntaxHighlighter);
    return d->doc;
}

void SyntaxHighlighter::rehighlight()
{
    Q_D(SyntaxHighlighter);
    if (!d->doc)
        return;

    QTextCursor cursor(d->doc);
    d->rehighlight(cursor, QTextCursor::End);
}

void SyntaxHighlighter::rehighlightBlock(const QTextBlock &block)
{
    Q_D(SyntaxHighlighter);
    if (!d->doc || !block.isValid() || block.document() != d->doc)
        return;

    const bool rehighlightPending = d->rehighlightPending;

    QTextCursor cursor(block);
    d->rehighlight(cursor, QTextCursor::EndOfBlock);

    if (rehighlightPending)
        d->rehighlightPending = rehighlightPending;
}

void SyntaxHighlighter::_q_reformatBlocks(int from, int charsRemoved, int charsAdded){
    Q_D(SyntaxHighlighter);
    documentChanged(from, charsRemoved, charsAdded);
    d->_q_reformatBlocks(from, charsRemoved, charsAdded);
}

void SyntaxHighlighter::_q_delayedRehighlight(){
    Q_D(SyntaxHighlighter);
    QTextDocument* doc = static_cast<QTextDocument*>(parent());
    documentChanged(0, 0, doc ? doc->characterCount() : INT_MAX);
    d->_q_delayedRehighlight();
}

void SyntaxHighlighter::setFormat(int start, int count, const QTextCharFormat &format)
{
    Q_D(SyntaxHighlighter);
    if (start < 0 || start >= d->formatChanges.count())
        return;

    const int end = qMin(start + count, d->formatChanges.count());
    for (int i = start; i < end; ++i)
        d->formatChanges[i] = format;
}

void SyntaxHighlighter::setFormat(int start, int count, const QColor &color)
{
    QTextCharFormat format;
    format.setForeground(color);
    setFormat(start, count, format);
}

void SyntaxHighlighter::setFormat(int start, int count, const QFont &font)
{
    QTextCharFormat format;
    format.setFont(font);
    setFormat(start, count, format);
}

QTextCharFormat SyntaxHighlighter::format(int pos) const
{
    Q_D(const SyntaxHighlighter);
    if (pos < 0 || pos >= d->formatChanges.count())
        return QTextCharFormat();
    return d->formatChanges.at(pos);
}

int SyntaxHighlighter::previousBlockState() const
{
    Q_D(const SyntaxHighlighter);
    if (!d->currentBlock.isValid())
        return -1;

    const QTextBlock previous = d->currentBlock.previous();
    if (!previous.isValid())
        return -1;

    return previous.userState();
}

int SyntaxHighlighter::currentBlockState() const
{
    Q_D(const SyntaxHighlighter);
    if (!d->currentBlock.isValid())
        return -1;

    return d->currentBlock.userState();
}

void SyntaxHighlighter::setCurrentBlockState(int newState)
{
    Q_D(SyntaxHighlighter);
    if (!d->currentBlock.isValid())
        return;

    d->currentBlock.setUserState(newState);
}

void SyntaxHighlighter::setCurrentBlockUserData(QTextBlockUserData *data)
{
    Q_D(SyntaxHighlighter);
    if (!d->currentBlock.isValid())
        return;

    d->currentBlock.setUserData(data);
}

QTextBlockUserData *SyntaxHighlighter::currentBlockUserData() const
{
    Q_D(const SyntaxHighlighter);
    if (!d->currentBlock.isValid())
        return 0;

    return d->currentBlock.userData();
}

QTextBlock SyntaxHighlighter::currentBlock() const
{
    Q_D(const SyntaxHighlighter);
    return d->currentBlock;
}

}// namespace
