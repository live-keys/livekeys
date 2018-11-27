#include "linesurface.h"
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
#include "private/qtextengine_p.h"
#include <algorithm>
#include "textedit_p.h"
#include "linemanager.h"
#include "collapsedsection.h"
#include "palettemanager.h"

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
: QQuickItem(parent)
, m_color(QRgb(0xFF000000))
, m_font(QFont()), document(nullptr)
, updateType(UpdatePaintNode)
, textEdit(nullptr), prevLineNumber(0)
, lineNumber(0), dirtyPos(0)
, lineManager(new LineManager)
, updatePending(false)
{
    setFlag(QQuickItem::ItemHasContents);
    setAcceptHoverEvents(true);
    lineManager->setLineSurface(this);
}

LineSurface::~LineSurface()
{
    qDeleteAll(textNodeMap);
}

QFont LineSurface::font() const
{
    return m_font;
}

void LineSurface::setFont(const QFont &f)
{
    if (m_font == f)
        return;

    m_font = f;
    if (static_cast<int>(m_font.pointSizeF()) != -1) {
        // 0.5pt resolution
        qreal size = qRound(m_font.pointSizeF()*2.0);
        m_font.setPointSizeF(size/2.0);
    }

    if (document) document->setDefaultFont(m_font);
    updateSize();

    emit fontChanged(m_font);
}

void LineSurface::textDocumentFinished()
{

    auto teDocument = textEdit->documentHandler()->target();
    prevLineNumber = lineNumber;
    lineNumber = teDocument->blockCount();

    deltaLineNumber = abs(prevLineNumber - lineNumber);
    if (deltaLineNumber)
    {
        if (prevLineNumber < lineNumber) linesAdded();
        else linesRemoved();
    }
    else
    {
        QTextBlock block = teDocument->findBlockByNumber(dirtyPos);
        auto userData = static_cast<ProjectDocumentBlockData*>(block.userData());
        if (userData && userData->stateChangeFlag())
        {
            userData->setStateChangeFlag(false);
            updateLineDocument();
        }
    }
}

void LineSurface::paletteSlot(int blockNum)
{
    auto firstDirtyBlock = document->findBlockByNumber(blockNum);
    document->markContentsDirty(firstDirtyBlock.position(), document->characterCount() - firstDirtyBlock.position());

    polish();
    if (isComponentComplete())
    {
        updateSize();
        updateType = LineSurface::UpdatePaintNode;

        update();
    }
}

void LineSurface::setDirtyBlockPosition(int pos)
{
    if (!updatePending || pos < dirtyPos)
    {
        updatePending = true;
        dirtyPos = pos;
    }
}

QColor LineSurface::color() const
{
    return m_color;
}

void LineSurface::setColor(const QColor &color)
{

    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged(m_color);
}

void LineSurface::setComponents(lv::TextEdit* te)
{

    textEdit = te;

    document = new QTextDocument(this);
    //this is to create the layout!
    document->documentLayout();


    m_font = te->font();
    document->setDefaultFont(m_font);
    QTextOption opt;
    opt.setAlignment(Qt::AlignRight | Qt::AlignTop);
    opt.setTextDirection(Qt::LeftToRight);
    document->setDefaultTextOption(opt);
    setFlag(QQuickItem::ItemHasContents);

    dirtyPos = 0;
    prevLineNumber = 0;
    lineNumber = 0;
    textDocumentFinished();

    QObject::connect(textEdit, &TextEdit::dirtyBlockPosition, this, &LineSurface::setDirtyBlockPosition);
    QObject::connect(textEdit, &TextEdit::textDocumentFinishedUpdating, this, &LineSurface::textDocumentFinished);
    QObject::connect(textEdit, &TextEdit::paletteChange, this, &LineSurface::paletteSlot);
    setAcceptedMouseButtons(Qt::AllButtons);
}

void LineSurface::showHideLines(bool show, int pos, int num)
{
    auto it = document->rootFrame()->begin();
    Q_ASSERT(document->blockCount() > pos);
    Q_ASSERT(document->blockCount() >= pos + num);
    for (int i = 0; i < pos+1; i++, ++it);
    int start = it.currentBlock().position();

    int length = 0;
    for (int i = 0; i < num; i++)
    {
        it.currentBlock().setVisible(show);
        length += it.currentBlock().length();
        ++it;
    }

    document->markContentsDirty(start, length);
}

void LineSurface::replaceTextInBlock(int blockNumber, std::string s)
{
    QTextBlock b = document->findBlockByNumber(blockNumber);
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
    changeLastCharInBlock(pos, '>');

    QTextBlock matchingBlock = textEdit->documentHandler()->target()->findBlockByNumber(pos);
    // ProjectDocumentBlockData* userData = static_cast<ProjectDocumentBlockData*>(matchingBlock.userData());
    QString s = matchingBlock.text();
    // s+=userData->replacementString;
    expandCollapseSkeleton(pos, num, s, false);
}


void LineSurface::expandLines(int pos, int num)
{

    changeLastCharInBlock(pos, 'v');

    QTextBlock matchingBlock = textEdit->documentHandler()->target()->findBlockByNumber(pos);
    // lv::ProjectDocumentBlockData* userData = static_cast<lv::ProjectDocumentBlockData*>(matchingBlock.userData());
    QString s = matchingBlock.text();
    // s.chop(userData->replacementString.length());
    expandCollapseSkeleton(pos, num, s, true);
}

void LineSurface::expandCollapseSkeleton(int pos, int num, QString &replacement, bool show)
{

    if (show)
    {
        textEdit->expandLines(pos, num, replacement);
        // lineManager->expandLines(pos, num);
    }
    else
    {
        textEdit->collapseLines(pos, num, replacement);
        // lineManager->collapseLines(pos, num);
    }


    showHideLines(show, pos, num);
    dirtyPos = pos;

    updateLineDocument();
}

void LineSurface::writeOutBlockStates()
{

    qDebug() << "----------blockStates---------------";
    auto it = textEdit->documentHandler()->target()->rootFrame()->begin();
    while (it != textEdit->documentHandler()->target()->rootFrame()->end())
    {
        QTextBlock block = it.currentBlock();
        lv::ProjectDocumentBlockData* userData = static_cast<lv::ProjectDocumentBlockData*>(block.userData());

        QString print(to_string(block.blockNumber()).c_str());
        print += " : ";
        if (userData)
        {
            switch (userData->collapseState())
            {
            case lv::ProjectDocumentBlockData::NoCollapse:
                    print += "NoCollapse"; break;
            case lv::ProjectDocumentBlockData::Collapse:
                print += "Collapse"; break;
            case lv::ProjectDocumentBlockData::Expand:
                print += "Expand"; break;
            }
        } else print += "none";

        qDebug() << print;
        ++it;
    }
}

void LineSurface::mousePressEvent(QMouseEvent* event)
{
    // find block that was clicked
    int position = document->documentLayout()->hitTest(event->localPos(), Qt::FuzzyHit);
    QTextBlock block = document->findBlock(position);
    int blockNum = block.blockNumber();

    QTextBlock matchingBlock = textEdit->documentHandler()->target()->findBlockByNumber(blockNum);
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
            lineManager->collapseLines(blockNum, userData->numOfCollapsedLines());
        }
        else if (userData->collapseState() == lv::ProjectDocumentBlockData::Expand)
        {
            std::string result;
            userData->expand();
            lineManager->expandLines(blockNum, userData->numOfCollapsedLines());
        }
    }
}

void LineSurface::triggerPreprocess()
{
    if (updateType == LineSurface::UpdateNone)
        updateType = LineSurface::UpdateOnlyPreprocess;
    polish();
    update();
}

void LineSurface::changeLastCharInBlock(int blockNumber, char c)
{

    QTextBlock b = document->findBlockByNumber(blockNumber);
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

    lineManager->linesAdded(dirtyPos, deltaLineNumber);

    QTextCursor cursor(document);
    cursor.movePosition(QTextCursor::MoveOperation::End);
    for (int i = prevLineNumber + 1; i <= lineNumber; i++)
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

    lineManager->linesRemoved(dirtyPos, deltaLineNumber);

    for (int i = prevLineNumber-1; i >= lineNumber; i--)
    {
        QTextCursor cursor(document->findBlockByNumber(i));
        cursor.select(QTextCursor::BlockUnderCursor);
        cursor.removeSelectedText();
    }

    updateLineDocument();
}

void LineSurface::updateLineDocument()
{
    updatePending = false;
    // we look for a collapsed section after the position where we made a change
    std::list<CollapsedSection*> &sections = lineManager->getSections();
    auto itSections = sections.begin();
    while (itSections != sections.end() && (*itSections)->position < dirtyPos) ++itSections;
    int curr = dirtyPos;
    auto it = document->rootFrame()->begin();
    for (int i = 0; i < curr; i++) ++it;
    while (it != document->rootFrame()->end())
    {
        auto currBlock = textEdit->documentHandler()->target()->findBlockByNumber(curr);
        lv::ProjectDocumentBlockData* userData =
                static_cast<lv::ProjectDocumentBlockData*>(currBlock.userData());

        bool visible = true;
        if (itSections != sections.end())
        {
            CollapsedSection* sec = (*itSections);
            if (curr == sec->position)
            {
                changeLastCharInBlock(curr, '>');
                userData->collapse();
                userData->setNumOfCollapsedLines(sec->numberOfLines);
                userData->setStateChangeFlag(false);
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
                userData->setStateChangeFlag(false);
            }
            else if (userData && userData->collapseState() != lv::ProjectDocumentBlockData::Expand)
            {
                changeLastCharInBlock(curr, ' ');
                userData->setStateChangeFlag(false);
            }
        }
        ++curr; ++it;
    }
    auto firstDirtyBlock = document->findBlockByNumber(dirtyPos);
    document->markContentsDirty(firstDirtyBlock.position(), document->characterCount() - firstDirtyBlock.position());

    polish();
    if (isComponentComplete())
    {
        updateSize();
        updateType = LineSurface::UpdatePaintNode;

        update();
    }
}

static bool comesBefore(LineSurface::Node* n1, LineSurface::Node* n2)
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


    if (!document) {
        if (oldNode) delete oldNode;
        return nullptr;
    }

    if (updateType != LineSurface::UpdatePaintNode && oldNode != nullptr) {
        // Update done in preprocess() in the nodes
        updateType = LineSurface::UpdateNone;
        return oldNode;
    }

    updateType = LineSurface::UpdateNone;

    if (!oldNode) {
        // If we had any QQuickTextNode node references, they were deleted along with the root node
        // But here we must delete the Node structures in textNodeMap
        qDeleteAll(textNodeMap);
        textNodeMap.clear();
    }

    RootNode *rootNode = static_cast<RootNode *>(oldNode);

    TextNodeEngine engine;
    TextNodeEngine frameDecorationsEngine;

    if (numberOfDigits(prevLineNumber) != numberOfDigits(lineNumber) || dirtyPos >= textNodeMap.size()){
        dirtyPos = 0;
    }
    if (!oldNode  || dirtyPos != -1) {

        if (!oldNode)
            rootNode = new RootNode;

        // delete all dirty nodes
        auto lineNumIt = textNodeMap.begin();
        for (int k=0; k<dirtyPos; k++, lineNumIt++);
        while (lineNumIt != textNodeMap.end())
        {

            rootNode->removeChildNode((*lineNumIt)->textNode());
            delete (*lineNumIt)->textNode();
            delete *lineNumIt;
            lineNumIt = textNodeMap.erase(lineNumIt);
        }

        // FIXME: the text decorations could probably be handled separately (only updated for affected textFrames)
        rootNode->resetFrameDecorations(new TextNode(this));
        resetEngine(&frameDecorationsEngine, m_color, QColor(), QColor());

        TextNode *node = nullptr;

        int currentNodeSize = 0;
        QMatrix4x4 basePositionMatrix;
        rootNode->setMatrix(basePositionMatrix);

        QPointF nodeOffset;
        if (document) {
            QList<QTextFrame *> frames;
            frames.append(document->rootFrame());

            while (!frames.isEmpty()) { //INFO: Root frame
                QTextFrame *textFrame = frames.takeFirst();
                frames.append(textFrame->childFrames());
                frameDecorationsEngine.addFrameDecorations(document, textFrame);

                //INFO: creating the text node
                node = new TextNode(this);
                resetEngine(&engine, m_color, QColor(), QColor());

                if (textFrame->firstPosition() > textFrame->lastPosition()
                        && textFrame->frameFormat().position() != QTextFrameFormat::InFlow) {

                    updateNodeTransform(node, document->documentLayout()->frameBoundingRect(textFrame).topLeft());
                    const int pos = textFrame->firstPosition() - 1;
                    ProtectedLayoutAccessor *a = static_cast<ProtectedLayoutAccessor *>(document->documentLayout());
                    QTextCharFormat format = a->formatAccessor(pos);
                    QTextBlock block = textFrame->firstCursorPosition().block();
                    engine.setCurrentLine(block.layout()->lineForTextPosition(pos - block.position()));
                    engine.addTextObject(QPointF(0, 0), format, TextNodeEngine::Unselected, document,
                                                  pos, textFrame->frameFormat().position());
                } else {
                    // Having nodes spanning across frame boundaries will break the current bookkeeping mechanism. We need to prevent that.
                    QList<int> frameBoundaries;
                    frameBoundaries.reserve(frames.size());
                    Q_FOREACH (QTextFrame *frame, frames)
                        frameBoundaries.append(frame->firstPosition());
                    std::sort(frameBoundaries.begin(), frameBoundaries.end());

                    QTextFrame::iterator it = textFrame->begin();
                    for (int k=0; k<dirtyPos; k++, it++);

                    while (!it.atEnd()) {

                        QTextBlock block = it.currentBlock();
                        ++it;
                        if (!block.isVisible()) continue;

                        if (!engine.hasContents()) {
                            nodeOffset = document->documentLayout()->blockBoundingRect(block).topLeft();
                            updateNodeTransform(node, nodeOffset);
                            int offset = textEdit->getPaletteManager()->drawingOffset(block.blockNumber(), false);
                            nodeOffset.setY(nodeOffset.y() - offset);
                        }

                        engine.addTextBlock(document, block, -nodeOffset, m_color, QColor(), -1, -1);
                        currentNodeSize += block.length();

                        currentNodeSize = 0;
                        engine.addToSceneGraph(node, QQuickText::Normal, QColor());
                        textNodeMap.append(new LineSurface::Node(-1, node));
                        rootNode->appendChildNode(node);
                        node = new TextNode(this);
                        resetEngine(&engine, m_color, QColor(), QColor());

                    }
                }
                engine.addToSceneGraph(node, QQuickText::Normal, QColor());
                textNodeMap.append(new LineSurface::Node(-1, node));
                rootNode->appendChildNode(node);            }
        }


        frameDecorationsEngine.addToSceneGraph(rootNode->frameDecorationsNode, QQuickText::Normal, QColor());
        // Now prepend the frame decorations since we want them rendered first, with the text nodes and cursor in front.
        rootNode->prependChildNode(rootNode->frameDecorationsNode);

        // Since we iterate over blocks from different text frames that are potentially not sorted
        // we need to ensure that our list of nodes is sorted again:
        std::sort(textNodeMap.begin(), textNodeMap.end(), &comesBefore);
    }

    QTextBlock block;
    for (block = document->firstBlock(); block.isValid(); block = block.next()) {
        if (block.layout() != nullptr && block.layout()->engine() != nullptr)
            block.layout()->engine()->resetFontEngineCache();
    }

    return rootNode;
}

//### we should perhaps be a bit smarter here -- depending on what has changed, we shouldn't
//    need to do all the calculations each time
void LineSurface::updateSize()
{
    if (!document) return;
    QSizeF layoutSize = document->documentLayout()->documentSize();
    setImplicitSize(layoutSize.width(), layoutSize.height());
}

}
