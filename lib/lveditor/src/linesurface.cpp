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

LineSurface::LineSurface(QQuickItem *parent)
: QQuickItem(*(new LineSurfacePrivate), parent)
{
    Q_D(LineSurface);
    d->init();
}
LineSurface::LineSurface(LineSurfacePrivate &dd, QQuickItem *parent)
: QQuickItem(dd, parent)
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
        updateSize();
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

    TextNodeEngine engine;
    TextNodeEngine frameDecorationsEngine;

    if (numberOfDigits(d->prevLineNumber) != numberOfDigits(d->lineNumber) || d->dirtyPos >= d->textNodeMap.size())
        d->dirtyPos = 0;

    if (!oldNode  || d->dirtyPos != -1) {

        if (!oldNode)
            rootNode = new RootNode;

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
        rootNode->resetFrameDecorations(new TextNode(this));
        resetEngine(&frameDecorationsEngine, d->color, QColor(), QColor());

        TextNode *node = nullptr;

        int currentNodeSize = 0;
        QMatrix4x4 basePositionMatrix;
        rootNode->setMatrix(basePositionMatrix);

        QPointF nodeOffset;
        if (d->document) {
            QList<QTextFrame *> frames;
            frames.append(d->document->rootFrame());

            while (!frames.isEmpty()) { //INFO: Root frame
                QTextFrame *textFrame = frames.takeFirst();
                frames.append(textFrame->childFrames());
                frameDecorationsEngine.addFrameDecorations(d->document, textFrame);

                //INFO: creating the text node
                node = new TextNode(this);
                resetEngine(&engine, d->color, QColor(), QColor());

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
                        if (!block.isVisible()) continue;

                        if (!engine.hasContents()) {
                            nodeOffset = d->document->documentLayout()->blockBoundingRect(block).topLeft();
                            updateNodeTransform(node, nodeOffset);
                        }

                        engine.addTextBlock(d->document, block, -nodeOffset, d->color, QColor(), -1, -1);
                        currentNodeSize += block.length();

                        currentNodeSize = 0;
                        engine.addToSceneGraph(node, QQuickText::Normal, QColor());
                        d->textNodeMap.append(new LineSurfacePrivate::Node(-1, node));
                        rootNode->appendChildNode(node);
                        node = new TextNode(this);
                        resetEngine(&engine, d->color, QColor(), QColor());

                    }
                }
                engine.addToSceneGraph(node, QQuickText::Normal, QColor());
                d->textNodeMap.append(new LineSurfacePrivate::Node(-1, node));
                rootNode->appendChildNode(node);            }
        }


        frameDecorationsEngine.addToSceneGraph(rootNode->frameDecorationsNode, QQuickText::Normal, QColor());
        // Now prepend the frame decorations since we want them rendered first, with the text nodes and cursor in front.
        rootNode->prependChildNode(rootNode->frameDecorationsNode);

        // Since we iterate over blocks from different text frames that are potentially not sorted
        // we need to ensure that our list of nodes is sorted again:
        std::sort(d->textNodeMap.begin(), d->textNodeMap.end(), &comesBefore);
    }

    QTextBlock block;
    for (block = d->document->firstBlock(); block.isValid(); block = block.next()) {
        if (block.layout() != nullptr && block.layout()->engine() != nullptr)
            block.layout()->engine()->resetFontEngineCache();
    }

    return rootNode;
}

void LineSurfacePrivate::init()
{
    Q_Q(LineSurface);
    q->setFlag(QQuickItem::ItemHasContents);

    q->setAcceptHoverEvents(true);
    lineManager->setLineSurface(q);
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

}
