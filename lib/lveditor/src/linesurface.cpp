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
#include "textdocumentlayout.h"

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
, textEdit(nullptr), previousLineNumber(0)
, lineNumber(0), dirtyPos(0)
, updatePending(false)
{
    setFlag(QQuickItem::ItemHasContents);
    setAcceptHoverEvents(true);
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
    previousLineNumber = lineNumber;
    lineNumber = teDocument->blockCount();

    deltaLineNumber = abs(previousLineNumber - lineNumber);
    if (deltaLineNumber)
    {
        /*if (previousLineNumber < lineNumber) linesAdded();
        else linesRemoved();*/
    }
    else
    {
        // updateLineDocument();
    }
}

void LineSurface::paletteSlot(int blockNum)
{
    if (!document) return;
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
    te->setLineSurface(this);
    setFlag(QQuickItem::ItemHasContents);
    setAcceptedMouseButtons(Qt::AllButtons);

    dirtyPos = 0;
    previousLineNumber = 0;
    lineNumber = 0;
    // textDocumentFinished();

    QObject::connect(textEdit, &TextEdit::textDocumentFinishedUpdating, this, &LineSurface::textDocumentFinished);
    QObject::connect(textEdit, &TextEdit::paletteChange, this, &LineSurface::paletteSlot);
}

void LineSurface::showHideLines(bool show, int pos, int num)
{
    if (!document) return;
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

void LineSurface::writeOutBlockVisibility()
{
    if (!document) return;
    qDebug() << "----------visibility---------------";
    auto it = document->rootFrame()->begin();
    while (it != document->rootFrame()->end())
    {
        QTextBlock block = it.currentBlock();

        qDebug() << block.blockNumber() << block.isVisible();
        ++it;
    }
}

void LineSurface::mousePressEvent(QMouseEvent* event)
{
    if (!document) return;
    // find block that was clicked
    int position = document->documentLayout()->hitTest(event->localPos(), Qt::FuzzyHit);
    QTextBlock block = document->findBlock(position);
    int blockNum = block.blockNumber();

    const QTextBlock& matchingBlock = textEdit->documentHandler()->target()->findBlockByNumber(blockNum);
    lv::ProjectDocumentBlockData* userData = static_cast<lv::ProjectDocumentBlockData*>(matchingBlock.userData());
    if (userData)
    {
        if (userData->collapseState() == lv::ProjectDocumentBlockData::Collapse)
        {
            textEdit->manageExpandCollapse(matchingBlock.blockNumber(), true);
        }
        else if (userData->collapseState() == lv::ProjectDocumentBlockData::Expand)
        {
            textEdit->manageExpandCollapse(matchingBlock.blockNumber(), false);
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

void LineSurface::setDocument(QTextDocument *doc)
{
    if (!doc) return;
    document = doc;
    updatePending = false;
    document->rootFrame(); // bug fix
    LineManager* lm = dynamic_cast<LineManager*>(document->parent());
    triggerUpdate(document->lineCount(), document->lineCount(), 0);
    connect(lm, &LineManager::updateLineSurface, this, &LineSurface::triggerUpdate);
}

void LineSurface::unsetTextDocument()
{
    document = nullptr;
}

void LineSurface::triggerUpdate(int prev, int curr, int dirty)
{
    previousLineNumber = prev;
    lineNumber = curr;
    dirtyPos = dirty;
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

    if (numberOfDigits(previousLineNumber) != numberOfDigits(lineNumber) || dirtyPos >= textNodeMap.size()){
        dirtyPos = 0;
    }
    if (!oldNode  || dirtyPos != -1) {

        if (!oldNode)
            rootNode = new RootNode;

        // delete all dirty nodes
        auto lineNumIt = textNodeMap.begin();
        int hiddenNum = 0; auto itPrime = document->rootFrame()->begin();
        for (int k=0; k<dirtyPos; k++)
        {
            if (!itPrime.currentBlock().isVisible()) hiddenNum++;
            ++itPrime;
        }
        for (int k=0; k<dirtyPos-hiddenNum; k++, lineNumIt++);
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
