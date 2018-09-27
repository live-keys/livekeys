#include "linesurface.h"
#include "linesurface_p.h"
#include "textcontrol_p.h"
#include "qquickwindow.h"
#include "textnodeengine_p.h"
#include "textutil_p.h"

#include <QtCore/qmath.h>
#include <QtGui/qguiapplication.h>
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>
#include <QtGui/qtextobject.h>
#include <QtGui/qtexttable.h>
#include <QtQml/qqmlinfo.h>
#include <QtQuick/qsgsimplerectnode.h>
#include <QTimer>

#include "private/qquicktextnode_p.h"
#include "private/qquickevents_p_p.h"

#include "private/qqmlproperty_p.h"

#include "private/qtextengine_p.h"
#include "private/qsgadaptationlayer_p.h"

#include <algorithm>
#include "textedit_p.h"
#include "linemanager.h"
#include "collapsedsection.h"


namespace lv {

namespace {
    class ProtectedLayoutAccessor: public QAbstractTextDocumentLayout
    {
    public:
        inline QTextCharFormat formatAccessor(int pos)
        {
            return format(pos);
        }
    };

    class RootNode : public QSGTransformNode
    {
    public:
        RootNode() : frameDecorationsNode(nullptr)
        { }

        void resetFrameDecorations(TextNode* newNode)
        {
            if (frameDecorationsNode) {
                removeChildNode(frameDecorationsNode);
                delete frameDecorationsNode;
            }
            frameDecorationsNode = newNode;
            newNode->setFlag(QSGNode::OwnedByParent);
        }
        TextNode* frameDecorationsNode;

    };
}

LineSurface::LineSurface(QQuickImplicitSizeItem *parent)
: QQuickImplicitSizeItem(*(new LineSurfacePrivate), parent)
{
    Q_D(LineSurface);
    d->init();
}

LineSurface::LineSurface(LineSurfacePrivate &dd, QQuickImplicitSizeItem *parent)
: QQuickImplicitSizeItem(dd, parent)
{
    Q_D(LineSurface);
    d->init();
}

QFont LineSurface::font() const
{
    Q_D(const LineSurface);
    return d->sourceFont;
}

void LineSurface::setFont(const QFont &font)
{
    Q_D(LineSurface);
    if (d->sourceFont == font)
        return;

    d->sourceFont = font;
    QFont oldFont = d->font;
    d->font = font;
    if (static_cast<int>(d->font.pointSizeF()) != -1) {
        // 0.5pt resolution
        qreal size = qRound(d->font.pointSizeF()*2.0);
        d->font.setPointSizeF(size/2.0);
    }

    if (oldFont != d->font) {
        if (d->document) d->document->setDefaultFont(d->font);
        if (d->cursorItem) {
            d->cursorItem->setHeight(QFontMetrics(d->font).height());
        }
        updateSize();
        updateWholeDocument();
#ifndef QT_NO_IM
        updateInputMethod(Qt::ImCursorRectangle | Qt::ImAnchorRectangle | Qt::ImFont);
#endif
    }
    emit fontChanged(d->sourceFont);
}

void LineSurface::textDocumentFinished()
{
    Q_D(LineSurface);
    auto teDocument = d->textEdit->documentHandler()->target();
    d->prevLineNumber = d->lineNumber;
    d->lineNumber = teDocument->blockCount();

    d->deltaLineNumber = abs(d->prevLineNumber - d->lineNumber);
    if (d->deltaLineNumber)
    {
        if (d->prevLineNumber < d->lineNumber) linesAdded();
        else linesRemoved();
    }
    else
    {
        QTextBlock block = teDocument->findBlockByNumber(d->dirtyPos);
        auto userData = static_cast<ProjectDocumentBlockData*>(block.userData());
        if (userData && userData->stateChangeFlag())
        {
            userData->setStateChangeFlag(false);
            updateLineDocument();
        }
    }
}

void LineSurface::setDirtyBlockPosition(int pos)
{
    Q_D(LineSurface);
    d->dirtyPos = pos;
}


QColor LineSurface::color() const
{
    Q_D(const LineSurface);
    return d->color;
}

void LineSurface::setColor(const QColor &color)
{
    Q_D(LineSurface);
    if (d->color == color)
        return;

    d->color = color;

    updateWholeDocument();
    emit colorChanged(d->color);
}

void LineSurface::setComponents(lv::TextEdit* te)
{
    Q_D(LineSurface);
    d->textEdit = te;
    d->init();

    d->document = new QTextDocument(this);
    //this is to create the layout!
    d->document->documentLayout();


    d->font = te->font();
    d->document->setDefaultFont(d->font);
    QTextOption opt;
    opt.setAlignment(Qt::AlignRight | Qt::AlignTop);
    opt.setTextDirection(Qt::LeftToRight);
    d->document->setDefaultTextOption(opt);
    setFlag(QQuickItem::ItemHasContents);

    d->dirtyPos = 0;
    d->prevLineNumber = 0;
    d->lineNumber = 0;
    textDocumentFinished();

    QObject::connect(d->textEdit, &TextEdit::dirtyBlockPosition, this, &LineSurface::setDirtyBlockPosition);
    QObject::connect(d->textEdit, &TextEdit::textDocumentFinishedUpdating, this, &LineSurface::textDocumentFinished);

    setAcceptedMouseButtons(Qt::AllButtons);
}

void LineSurface::showHideLines(bool show, int pos, int num)
{
    Q_D(LineSurface);
    auto it = d->document->rootFrame()->begin();
    Q_ASSERT(d->document->blockCount() > pos);
    Q_ASSERT(d->document->blockCount() >= pos + num);
    for (int i = 0; i < pos+1; i++, ++it);
    int start = it.currentBlock().position();

    int length = 0;
    for (int i = 0; i < num; i++)
    {
        it.currentBlock().setVisible(show);
        length += it.currentBlock().length();
        ++it;
    }

    d->document->markContentsDirty(start, length);
}

void LineSurface::replaceTextInBlock(int blockNumber, std::string s)
{
    Q_D(LineSurface);
    QTextBlock b = d->document->findBlockByNumber(blockNumber);
    QTextCursor cursor(b);
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::EndOfBlock);
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveMode::KeepAnchor);
    cursor.removeSelectedText();
    cursor.insertText(QString(s.c_str()));
    cursor.endEditBlock();
}


bool LineSurfacePrivate::isImplicitResizeEnabled() const
{
    return !extra.isAllocated() || extra->implicitResize;
}

void LineSurfacePrivate::setImplicitResizeEnabled(bool enabled)
{
    if (!enabled)
        extra.value().implicitResize = false;
    else if (extra.isAllocated())
        extra->implicitResize = true;
}

void LineSurface::geometryChanged(const QRectF &newGeometry,
                                  const QRectF &oldGeometry)
{
    Q_D(LineSurface);
    if (!d->inLayout && ((abs(newGeometry.width() - oldGeometry.width()) > LV_ACCURACY && widthValid())
        || (abs(newGeometry.height() - oldGeometry.height()) > LV_ACCURACY && heightValid()))) {
        updateSize();
        updateWholeDocument();
    }
    QQuickItem::geometryChanged(newGeometry, oldGeometry);

}

void LineSurface::componentComplete()
{
    Q_D(LineSurface);

    QQuickItem::componentComplete();
    if (!d->document) return;

    if (d->dirty) {
        d->updateDefaultTextOption();
        updateSize();
        d->dirty = false;
    }
}

void LineSurface::mousePressEvent(QMouseEvent* event)
{
    Q_D(LineSurface);
    // find block that was clicked
    int position = d->document->documentLayout()->hitTest(event->localPos(), Qt::FuzzyHit);
    QTextBlock block = d->document->findBlock(position);
    int blockNum = block.blockNumber();

    QTextBlock matchingBlock = d->textEdit->documentHandler()->target()->findBlockByNumber(blockNum);
    lv::ProjectDocumentBlockData* userData = static_cast<lv::ProjectDocumentBlockData*>(matchingBlock.userData());
    if (userData)
    {
        if (userData->collapseState() == lv::ProjectDocumentBlockData::Collapse)
        {
            userData->collapse();
            int num; QString repl;
            userData->onCollapse()(matchingBlock, num, repl);
            userData->setNumOfCollapsedLines(num);
            userData->setReplacementString(repl);
            d->lineManager->collapseLines(blockNum, userData->numOfCollapsedLines());
        }
        else if (userData->collapseState() == lv::ProjectDocumentBlockData::Expand)
        {
            std::string result;
            userData->expand();
            d->lineManager->expandLines(blockNum, userData->numOfCollapsedLines());
        }

    }
}

void LineSurface::triggerPreprocess()
{
    Q_D(LineSurface);
    if (d->updateType == LineSurfacePrivate::UpdateNone)
        d->updateType = LineSurfacePrivate::UpdateOnlyPreprocess;
    polish();
    update();
}

typedef QList<LineSurfacePrivate::Node*>::iterator TextNodeIterator;


static bool comesBefore(LineSurfacePrivate::Node* n1, LineSurfacePrivate::Node* n2)
{
    return n1->startPos() < n2->startPos();
}

static inline void updateNodeTransform(TextNode* node, const QPointF &topLeft)
{
    QMatrix4x4 transformMatrix;
    transformMatrix.translate(static_cast<float>(topLeft.x()), static_cast<float>(topLeft.y()));
    node->setMatrix(transformMatrix);
}

void LineSurface::invalidateFontCaches()
{
    Q_D(LineSurface);
    if (!d->document) return;


    QTextBlock block;
    for (block = d->document->firstBlock(); block.isValid(); block = block.next()) {
        if (block.layout() != nullptr && block.layout()->engine() != nullptr)
            block.layout()->engine()->resetFontEngineCache();
    }
}

inline void resetEngine(TextNodeEngine *engine, const QColor& textColor, const QColor& selectedTextColor, const QColor& selectionColor)
{
    *engine = TextNodeEngine();
    engine->setTextColor(textColor);
    engine->setSelectedTextColor(selectedTextColor);
    engine->setSelectionColor(selectionColor);
}

QSGNode *LineSurface::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData)
{
    Q_UNUSED(updatePaintNodeData);
    Q_D(LineSurface);

    if (!d->document) {
        if (oldNode) delete oldNode;
        return nullptr;
    }

    if (d->updateType != LineSurfacePrivate::UpdatePaintNode && oldNode != nullptr) {
        // Update done in preprocess() in the nodes
        d->updateType = LineSurfacePrivate::UpdateNone;
        return oldNode;
    }

    d->updateType = LineSurfacePrivate::UpdateNone;

    if (!oldNode) {
        // If we had any QQuickTextNode node references, they were deleted along with the root node
        // But here we must delete the Node structures in textNodeMap
        qDeleteAll(d->textNodeMap);
        d->textNodeMap.clear();
    }

    RootNode *rootNode = static_cast<RootNode *>(oldNode);
    /*TextNodeIterator nodeIterator = d->textNodeMap.begin();

    while (nodeIterator != d->textNodeMap.end() && !(*nodeIterator)->dirty())
        ++nodeIterator;
*/
    TextNodeEngine engine;
    TextNodeEngine frameDecorationsEngine;

    d->lastHighlightChangeStart = INT_MAX;
    d->lastHighlightChangeEnd   = 0;

    if (numberOfDigits(d->prevLineNumber) != numberOfDigits(d->lineNumber) || d->dirtyPos >= d->textNodeMap.size())
        d->dirtyPos = 0;

    if (!oldNode  || d->dirtyPos != -1/* || nodeIterator < d->textNodeMap.end()*/) {

        if (!oldNode)
            rootNode = new RootNode;

        /*int firstDirtyPos = 0;
        if (nodeIterator != d->textNodeMap.end()) {
            firstDirtyPos = (*nodeIterator)->startPos();
            do {
                rootNode->removeChildNode((*nodeIterator)->textNode());
                delete (*nodeIterator)->textNode();
                delete *nodeIterator;
                nodeIterator = d->textNodeMap.erase(nodeIterator);
            } while (nodeIterator != d->textNodeMap.end() && (*nodeIterator)->dirty());
        }
*/
        // delete all dirty nodes
        auto lineNumIt = d->textNodeMap.begin();
        for (int k=0; k<d->dirtyPos; k++, lineNumIt++);
        while (lineNumIt != d->textNodeMap.end())
        {

            rootNode->removeChildNode((*lineNumIt)->textNode());
            delete (*lineNumIt)->textNode();
            delete *lineNumIt;
            lineNumIt = d->textNodeMap.erase(lineNumIt);
        }

        // FIXME: the text decorations could probably be handled separately (only updated for affected textFrames)
        rootNode->resetFrameDecorations(d->createTextNode());
        resetEngine(&frameDecorationsEngine, d->color, d->selectedTextColor, d->selectionColor);

        TextNode *node = nullptr;

        int currentNodeSize = 0;
        //int nodeStart = firstDirtyPos;
        QPointF basePosition(d->xoff, d->yoff);
        QMatrix4x4 basePositionMatrix;
        basePositionMatrix.translate(static_cast<float>(basePosition.x()), static_cast<float>(basePosition.y()));
        rootNode->setMatrix(basePositionMatrix);

        QPointF nodeOffset;
        //LineSurfacePrivate::Node *firstCleanNode = (nodeIterator != d->textNodeMap.end()) ? *nodeIterator : nullptr;

        if (d->document) {
            QList<QTextFrame *> frames;
            frames.append(d->document->rootFrame());

            while (!frames.isEmpty()) { //INFO: Root frame
                QTextFrame *textFrame = frames.takeFirst();
                frames.append(textFrame->childFrames());
                frameDecorationsEngine.addFrameDecorations(d->document, textFrame);

                /*if (textFrame->lastPosition() < firstDirtyPos || (firstCleanNode && textFrame->firstPosition() >= firstCleanNode->startPos()))
                    continue;
*/
                //INFO: creating the text node
                node = d->createTextNode();
                resetEngine(&engine, d->color, d->selectedTextColor, d->selectionColor);

                if (textFrame->firstPosition() > textFrame->lastPosition()
                        && textFrame->frameFormat().position() != QTextFrameFormat::InFlow) {

                    updateNodeTransform(node, d->document->documentLayout()->frameBoundingRect(textFrame).topLeft());
                    const int pos = textFrame->firstPosition() - 1;
                    ProtectedLayoutAccessor *a = static_cast<ProtectedLayoutAccessor *>(d->document->documentLayout());
                    QTextCharFormat format = a->formatAccessor(pos);
                    QTextBlock block = textFrame->firstCursorPosition().block();
                    engine.setCurrentLine(block.layout()->lineForTextPosition(pos - block.position()));
                    engine.addTextObject(QPointF(0, 0), format, TextNodeEngine::Unselected, d->document,
                                                  pos, textFrame->frameFormat().position());
                    //nodeStart = pos;
                } else {
                    // Having nodes spanning across frame boundaries will break the current bookkeeping mechanism. We need to prevent that.
                    QList<int> frameBoundaries;
                    frameBoundaries.reserve(frames.size());
                    Q_FOREACH (QTextFrame *frame, frames)
                        frameBoundaries.append(frame->firstPosition());
                    std::sort(frameBoundaries.begin(), frameBoundaries.end());

                    QTextFrame::iterator it = textFrame->begin();
                    for (int k=0; k<d->dirtyPos; k++, it++);

                    while (!it.atEnd()) {

                        QTextBlock block = it.currentBlock();
                        ++it;
/*
                        if (block.position() < firstDirtyPos)
                            continue;
*/
                        if (!block.isVisible()) continue;

                        if (!engine.hasContents()) {
                            nodeOffset = d->document->documentLayout()->blockBoundingRect(block).topLeft();
                            updateNodeTransform(node, nodeOffset);
                            //nodeStart = block.position();
                        }

                        engine.addTextBlock(d->document, block, -nodeOffset, d->color, QColor(), -1, -1);
                        currentNodeSize += block.length();
/*
                        if ((it.atEnd()) || (firstCleanNode && block.next().position() >= firstCleanNode->startPos())) // last node that needed replacing or last block of the frame
                            break;
*/
                        //if (currentNodeSize > nodeBreakingSize || lowerBound == frameBoundaries.constEnd() || *lowerBound > nodeStart) {
                            currentNodeSize = 0;
                            //d->addCurrentTextNodeToRoot(&engine, rootNode, node, nodeIterator, nodeStart);
                            engine.addToSceneGraph(node, QQuickText::Normal, QColor());
                            d->textNodeMap.append(new LineSurfacePrivate::Node(-1, node));
                            rootNode->appendChildNode(node);
                            /*void LineSurfacePrivate::addCurrentTextNodeToRoot(TextNodeEngine *engine, QSGTransformNode *root, TextNode *node, TextNodeIterator &it, int startPos)
                            {*/

                            /*}*/


                            node = d->createTextNode();
                            resetEngine(&engine, d->color, d->selectedTextColor, d->selectionColor);
                            //nodeStart = block.next().position();
                        //}
                    }
                }
                engine.addToSceneGraph(node, QQuickText::Normal, QColor());
                d->textNodeMap.append(new LineSurfacePrivate::Node(-1, node));
                rootNode->appendChildNode(node);            }
        }


        frameDecorationsEngine.addToSceneGraph(rootNode->frameDecorationsNode, QQuickText::Normal, QColor());
        // Now prepend the frame decorations since we want them rendered first, with the text nodes and cursor in front.
        rootNode->prependChildNode(rootNode->frameDecorationsNode);

        /*Q_ASSERT(nodeIterator == d->textNodeMap.end() || (*nodeIterator) == firstCleanNode);
        // Update the position of the subsequent text blocks.
        if (d->document && firstCleanNode) {
            QPointF oldOffset = firstCleanNode->textNode()->matrix().map(QPointF(0,0));
            QPointF currentOffset = d->document->documentLayout()->blockBoundingRect(d->document->findBlock(firstCleanNode->startPos())).topLeft();
            QPointF delta = currentOffset - oldOffset;
            while (nodeIterator != d->textNodeMap.end()) {
                QMatrix4x4 transformMatrix = (*nodeIterator)->textNode()->matrix();
                transformMatrix.translate(static_cast<float>(delta.x()), static_cast<float>(delta.y()));
                (*nodeIterator)->textNode()->setMatrix(transformMatrix);
                ++nodeIterator;
            }

        }
*/
        // Since we iterate over blocks from different text frames that are potentially not sorted
        // we need to ensure that our list of nodes is sorted again:
        std::sort(d->textNodeMap.begin(), d->textNodeMap.end(), &comesBefore);
    }

    invalidateFontCaches();

    return rootNode;
}

void LineSurface::updatePolish()
{
    invalidateFontCaches();
}

LineSurfacePrivate::ExtraData::ExtraData()
    : padding(0)
    , topPadding(0)
    , leftPadding(0)
    , rightPadding(0)
    , bottomPadding(0)
    , explicitTopPadding(false)
    , explicitLeftPadding(false)
    , explicitRightPadding(false)
    , explicitBottomPadding(false)
    , implicitResize(true)
{
}

void LineSurface::singleShotUpdate()
{
    Q_D(LineSurface);
    d->highlightingInProgress = true;
    emit dirtyBlockPosition(-1);
    d->document->markContentsDirty(0, d->document->characterCount());
}

void LineSurfacePrivate::init()
{
    Q_Q(LineSurface);

#ifndef QT_NO_CLIPBOARD
    if (QGuiApplication::clipboard()->supportsSelection())
        q->setAcceptedMouseButtons(Qt::LeftButton | Qt::MiddleButton);
    else
#endif
        q->setAcceptedMouseButtons(Qt::LeftButton);

#ifndef QT_NO_IM
    q->setFlag(QQuickItem::ItemAcceptsInputMethod);
#endif
    q->setFlag(QQuickItem::ItemHasContents);

    q->setAcceptHoverEvents(true);
    lineManager->setLineSurface(q);
}

void LineSurfacePrivate::resetInputMethod()
{
    Q_Q(LineSurface);
    if (q->hasActiveFocus() && qGuiApp)
        QGuiApplication::inputMethod()->reset();
}

void LineSurface::q_textChanged()
{
    Q_D(LineSurface);
    if (!d->document) return;
    d->textCached = false;
    d->updateDefaultTextOption();
    updateSize();
    emit textChanged();
}

void LineSurface::markDirtyNodesForRange(int start, int end, int charDelta)
{
    Q_D(LineSurface);
    if (start == end)
        return;

    int oldEnd = end;
    bool nodesUpdate = true;

    if ( start < d->lastHighlightChangeStart && end > d->lastHighlightChangeEnd ){ // stretch both ways
        d->lastHighlightChangeStart = start;
        d->lastHighlightChangeEnd   = end;
    } else if ( start < d->lastHighlightChangeStart ){ // stretch left
        end = d->lastHighlightChangeStart;
        d->lastHighlightChangeStart = start;
    } else if ( end > d->lastHighlightChangeEnd ){ // stretch right
        start = d->lastHighlightChangeEnd;
        d->lastHighlightChangeEnd = end;
    } else {
        nodesUpdate = false; // this is inside the interval we're updating
    }

    if(start == end) nodesUpdate = false;

    LineSurfacePrivate::Node dummyNode(start, nullptr);
    TextNodeIterator it = std::lower_bound(d->textNodeMap.begin(), d->textNodeMap.end(), &dummyNode, &comesBefore);
    if (nodesUpdate && charDelta)
    {
        // qLowerBound gives us the first node past the start of the affected portion, rewind to the first node
        // that starts at the last position before the edit position. (there might be several because of images)
        if (it != d->textNodeMap.begin()) {
            --it;
            LineSurfacePrivate::Node otherDummy((*it)->startPos(), nullptr);
            it = std::lower_bound(d->textNodeMap.begin(), d->textNodeMap.end(), &otherDummy, &comesBefore);
        }
    }

    TextNodeIterator otherIt = it;

    if (nodesUpdate)
    {
        while (it != d->textNodeMap.end() && (*it)->startPos() <= end)
        {
            (*it)->setDirty(); ++it;
        }
    }

    if (charDelta)
    {
        while (otherIt != d->textNodeMap.end() && (*otherIt)->startPos() <= oldEnd)
        {
            ++otherIt;
        }

        while (otherIt != d->textNodeMap.end())
        {
            (*otherIt)->moveStartPos(charDelta);
            ++otherIt;
        }
    }
}

void LineSurface::q_contentsChange(int pos, int charsRemoved, int charsAdded)
{
    Q_D(LineSurface);

    const int editRange = pos + qMax(charsAdded, charsRemoved);
    const int delta = charsAdded - charsRemoved;

    emit dirtyBlockPosition(d->document->findBlock(pos).blockNumber());

    markDirtyNodesForRange(pos, editRange, delta);

    d->highlightingInProgress = true;

    polish();
    if (isComponentComplete()) {
        d->updateType = LineSurfacePrivate::UpdatePaintNode;
        update();
    }
}

qreal LineSurfacePrivate::getImplicitWidth() const
{
    Q_Q(const LineSurface);
    if (!requireImplicitWidth) {
        // We don't calculate implicitWidth unless it is required.
        // We need to force a size update now to ensure implicitWidth is calculated
        const_cast<LineSurfacePrivate*>(this)->requireImplicitWidth = true;
        const_cast<LineSurface*>(q)->updateSize();
    }
    return implicitWidth;
}

//### we should perhaps be a bit smarter here -- depending on what has changed, we shouldn't
//    need to do all the calculations each time
void LineSurface::updateSize()
{
    Q_D(LineSurface);
    if (!d->document) return;
    QSizeF layoutSize = d->document->documentLayout()->documentSize();
    setImplicitSize(layoutSize.width(), layoutSize.height());
}

void LineSurface::updateWholeDocument()
{
    Q_D(LineSurface);
    if (!d->textNodeMap.isEmpty()) {
        Q_FOREACH (LineSurfacePrivate::Node* node, d->textNodeMap)
            node->setDirty();
    }

    polish();
    if (isComponentComplete()) {
        d->updateType = LineSurfacePrivate::UpdatePaintNode;
        update();
    }
}

void LineSurface::linesAdded()
{
    Q_D(LineSurface);
    d->lineManager->linesAdded(d->dirtyPos, d->deltaLineNumber);

    QTextCursor cursor(d->document);
    cursor.movePosition(QTextCursor::MoveOperation::End);
    for (int i = d->prevLineNumber + 1; i <= d->lineNumber; i++)
    {

        if (i!=1) cursor.insertBlock();
        std::string s = std::to_string(i) + "  ";
        if (i < 10) s = " " + s;
        const QString a(s.c_str());

        cursor.insertText(a);
    }
    updateLineDocument();
}

void LineSurface::linesRemoved()
{
    Q_D(LineSurface);
    d->lineManager->linesRemoved(d->dirtyPos, d->deltaLineNumber);

    for (int i = d->prevLineNumber-1; i >= d->lineNumber; i--)
    {
        QTextCursor cursor(d->document->findBlockByNumber(i));
        cursor.select(QTextCursor::BlockUnderCursor);
        cursor.removeSelectedText();
    }

    updateLineDocument();
}

void LineSurface::updateLineDocument()
{
    Q_D(LineSurface);
    // we look for a collapsed section after the position where we made a change
    std::list<CollapsedSection*> &sections = d->lineManager->getSections();
    auto itSections = sections.begin();
    while (itSections != sections.end() && (*itSections)->position < d->dirtyPos) ++itSections;
    int curr = d->dirtyPos;
    auto it = d->document->rootFrame()->begin();
    for (int i = 0; i < curr; i++) ++it;
    while (it != d->document->rootFrame()->end())
    {
        auto currBlock = d->textEdit->documentHandler()->target()->findBlockByNumber(curr);
        lv::ProjectDocumentBlockData* userData =
                static_cast<lv::ProjectDocumentBlockData*>(currBlock.userData());

        bool visible = true;
        if (itSections != sections.end())
        {
            CollapsedSection* sec = (*itSections);
            if (curr == sec->position && userData
                    && userData->collapseState() == lv::ProjectDocumentBlockData::Expand)
            {
                changeLastCharInBlock(curr, '>');

            }
            // if we're in a collapsed section, block shouldn't be visible
            if (curr > sec->position && curr <= sec->position + sec->numberOfLines)
            {
                visible = false;
            }
            // if we just exited a collapsed section, move to the next one
            if (curr == sec->position + sec->numberOfLines) ++itSections;

        }

        visible = visible && currBlock.isVisible();

        it.currentBlock().setVisible(visible);
        if (visible) {
            if (userData && userData->collapseState() == lv::ProjectDocumentBlockData::Collapse)
            {
                changeLastCharInBlock(curr, 'v');
            }
            else if (userData && userData->collapseState() != lv::ProjectDocumentBlockData::Expand)
                changeLastCharInBlock(curr, ' ');
        }
        ++curr; ++it;
    }
    auto firstDirtyBlock = d->document->findBlockByNumber(d->dirtyPos);
    d->document->markContentsDirty(firstDirtyBlock.position(), d->document->characterCount() - firstDirtyBlock.position());

    polish();
    if (isComponentComplete())
    {
        updateSize();
        d->updateType = LineSurfacePrivate::UpdatePaintNode;

        update();
    }
}

void LineSurface::changeLastCharInBlock(int blockNumber, char c)
{
    Q_D(LineSurface);
    QTextBlock b = d->document->findBlockByNumber(blockNumber);
    QTextCursor cursor(b);
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::EndOfBlock);
    cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveMode::KeepAnchor);
    cursor.removeSelectedText();
    cursor.insertText(QString(c));
    cursor.endEditBlock();
}

void LineSurface::collapseLines(int pos, int num)
{
    Q_D(LineSurface);
    changeLastCharInBlock(pos, '>');

    QTextBlock matchingBlock = d->textEdit->documentHandler()->target()->findBlockByNumber(pos);
    // ProjectDocumentBlockData* userData = static_cast<ProjectDocumentBlockData*>(matchingBlock.userData());
    QString s = matchingBlock.text();
    // s+=userData->replacementString;
    expandCollapseSkeleton(pos, num, s, false);
}

void LineSurface::expandLines(int pos, int num)
{
    Q_D(LineSurface);
    changeLastCharInBlock(pos, 'v');

    QTextBlock matchingBlock = d->textEdit->documentHandler()->target()->findBlockByNumber(pos);
    // lv::ProjectDocumentBlockData* userData = static_cast<lv::ProjectDocumentBlockData*>(matchingBlock.userData());
    QString s = matchingBlock.text();
    // s.chop(userData->replacementString.length());
    expandCollapseSkeleton(pos, num, s, true);
}

void LineSurface::expandCollapseSkeleton(int pos, int num, QString &replacement, bool show)
{
    Q_D(LineSurface);
    if (show)
    {
        d->textEdit->expandLines(pos, num, replacement);
        // lineManager->expandLines(pos, num);
    }
    else
    {
        d->textEdit->collapseLines(pos, num, replacement);
        // lineManager->collapseLines(pos, num);
    }


    showHideLines(show, pos, num);
    d->dirtyPos = pos;

    updateLineDocument();
}




void LineSurface::highlightingDone(const QRectF &)
{
    Q_D(LineSurface);

    if (d->highlightingInProgress)
    {
        d->highlightingInProgress = false;
        emit textDocumentFinishedUpdating();
    }
}

void LineSurface::q_updateAlignment()
{
    Q_D(LineSurface);
    if (!d->document) return;

        d->updateDefaultTextOption();
}

void LineSurface::updateTotalLines()
{
    Q_D(LineSurface);

    int subLines = 0;
    if (!d->document) return;

    for (QTextBlock it = d->document->begin(); it != d->document->end(); it = it.next()) {
        QTextLayout *layout = it.layout();
        if (!layout)
            continue;
        subLines += layout->lineCount()-1;
    }

    int newTotalLines = d->document->lineCount() + subLines;
    if (d->lineCount != newTotalLines) {
        d->lineCount = newTotalLines;
        emit lineCountChanged();
    }
}

void LineSurfacePrivate::updateDefaultTextOption()
{
    if (!document) return;

    QTextOption opt = document->defaultTextOption();
    int oldAlignment = opt.alignment();
    Qt::LayoutDirection oldTextDirection = opt.textDirection();

#ifndef QT_NO_IM
    if (contentDirection == Qt::LayoutDirectionAuto) {
        opt.setTextDirection(qGuiApp->inputMethod()->inputDirection());
    } else
#endif
    {
        opt.setTextDirection(contentDirection);
    }

    QTextOption::WrapMode oldWrapMode = opt.wrapMode();

    bool oldUseDesignMetrics = opt.useDesignMetrics();

    if (oldWrapMode != opt.wrapMode() || oldAlignment != opt.alignment()
        || oldTextDirection != opt.textDirection()
        || oldUseDesignMetrics != opt.useDesignMetrics()) {
        document->setDefaultTextOption(opt);
    }
}

void LineSurface::focusInEvent(QFocusEvent *event)
{
    Q_D(LineSurface);
    d->handleFocusEvent(event);
    QQuickItem::focusInEvent(event);
}

void LineSurface::focusOutEvent(QFocusEvent *event)
{
    Q_D(LineSurface);
    d->handleFocusEvent(event);
    QQuickItem::focusOutEvent(event);
}

void LineSurfacePrivate::handleFocusEvent(QFocusEvent *event)
{
    Q_Q(LineSurface);
    if (!document) return;
    bool focus = event->type() == QEvent::FocusIn;

    if (focus) {
        q->q_updateAlignment();
#ifndef QT_NO_IM
        if (focusOnPress)
            qGuiApp->inputMethod()->show();
        q->connect(QGuiApplication::inputMethod(), SIGNAL(inputDirectionChanged(Qt::LayoutDirection)),
                q, SLOT(q_updateAlignment()));
#endif
    } else {
#ifndef QT_NO_IM
        q->disconnect(QGuiApplication::inputMethod(), SIGNAL(inputDirectionChanged(Qt::LayoutDirection)),
                   q, SLOT(q_updateAlignment()));
#endif
        emit q->editingFinished();
    }
}

void LineSurfacePrivate::addCurrentTextNodeToRoot(TextNodeEngine *engine, QSGTransformNode *root, TextNode *node, TextNodeIterator &it, int startPos)
{
    engine->addToSceneGraph(node, QQuickText::Normal, QColor());
    it = textNodeMap.insert(it, new LineSurfacePrivate::Node(startPos, node));
    ++it;
    root->appendChildNode(node);
}

TextNode *LineSurfacePrivate::createTextNode()
{
    Q_Q(LineSurface);
    TextNode* node = new TextNode(q);
    return node;
}

void LineSurface::insert(int position, const QString &text)
{
    Q_D(LineSurface);
    if (!d->document) return;

    if (position < 0 || position >= d->document->characterCount())
        return;
    QTextCursor cursor(d->document);
    cursor.setPosition(position);
    cursor.insertText(text);

}


void LineSurface::remove(int start, int end)
{
    Q_D(LineSurface);
    if (!d->document) return;

    start = qBound(0, start, d->document->characterCount() - 1);
    end = qBound(0, end, d->document->characterCount() - 1);
    QTextCursor cursor(d->document);
    cursor.setPosition(start, QTextCursor::MoveAnchor);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();
}

bool LineSurfacePrivate::isLinkHoveredConnected()
{
    Q_Q(LineSurface);
    LV_IS_SIGNAL_CONNECTED(q, LineSurface, linkHovered, (const QString &));
}

void LineSurface::append(const QString &text)
{
    Q_D(LineSurface);
    if (!d->document) return;
    QTextCursor cursor(d->document);
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::End);

    if (!d->document->isEmpty())
        cursor.insertBlock();

#ifndef QT_NO_TEXTHTMLPARSER
        cursor.insertText(text);
#else
    cursor.insertText(text);
#endif // QT_NO_TEXTHTMLPARSER

    cursor.endEditBlock();
}

void LineSurface::clear()
{
    Q_D(LineSurface);
    if (!d->document) return;

    d->resetInputMethod();
}


void LineSurfacePrivate::implicitWidthChanged()
{
    Q_Q(LineSurface);
    QQuickImplicitSizeItemPrivate::implicitWidthChanged();
    emit q->implicitWidthChanged2();
}

void LineSurfacePrivate::implicitHeightChanged()
{
    Q_Q(LineSurface);
    QQuickImplicitSizeItemPrivate::implicitHeightChanged();
    emit q->implicitHeightChanged2();
}


}
