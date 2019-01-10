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
, m_font(QFont()), m_document(nullptr)
, m_updateType(UpdatePaintNode)
, m_textEdit(nullptr), m_previousLineNumber(0)
, m_lineNumber(0), m_dirtyPos(0)
, m_updatePending(false)
{
    setFlag(QQuickItem::ItemHasContents);
    setAcceptHoverEvents(true);
}

LineSurface::~LineSurface()
{
    qDeleteAll(m_textNodeMap);
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

    if (m_document) m_document->setDefaultFont(m_font);
    updateSize();

    emit fontChanged(m_font);
}

void LineSurface::paletteSlot(int blockNum)
{
    if (!m_document) return;
    auto firstDirtyBlock = m_document->findBlockByNumber(blockNum);
    m_document->markContentsDirty(firstDirtyBlock.position(), m_document->characterCount() - firstDirtyBlock.position());

    polish();
    if (isComponentComplete())
    {
        updateSize();
        m_updateType = LineSurface::UpdatePaintNode;

        update();
    }
}

void LineSurface::setDirtyBlockPosition(int pos)
{
    if (!m_updatePending || pos < m_dirtyPos)
    {
        m_updatePending = true;
        m_dirtyPos = pos;
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
    m_textEdit = te;
    te->setLineSurface(this);
    setFlag(QQuickItem::ItemHasContents);
    setAcceptedMouseButtons(Qt::AllButtons);

    m_dirtyPos = 0;
    m_previousLineNumber = 0;
    m_lineNumber = 0;

    QObject::connect(m_textEdit, &TextEdit::paletteChange, this, &LineSurface::paletteSlot);
}

void LineSurface::showHideLines(bool show, int pos, int num)
{
    if (!m_document) return;
    auto it = m_document->rootFrame()->begin();
    Q_ASSERT(m_document->blockCount() > pos);
    Q_ASSERT(m_document->blockCount() >= pos + num);
    for (int i = 0; i < pos+1; i++, ++it);
    int start = it.currentBlock().position();

    int length = 0;
    for (int i = 0; i < num; i++)
    {
        it.currentBlock().setVisible(show);
        length += it.currentBlock().length();
        ++it;
    }

    m_document->markContentsDirty(start, length);
}

void LineSurface::writeOutBlockStates()
{

    qDebug() << "----------blockStates---------------";
    auto it = m_textEdit->documentHandler()->target()->rootFrame()->begin();
    while (it != m_textEdit->documentHandler()->target()->rootFrame()->end())
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
    if (!m_document) return;
    qDebug() << "----------visibility---------------";
    auto it = m_document->rootFrame()->begin();
    while (it != m_document->rootFrame()->end())
    {
        QTextBlock block = it.currentBlock();

        qDebug() << block.blockNumber() << block.isVisible();
        ++it;
    }
}

void LineSurface::mousePressEvent(QMouseEvent* event)
{
    if (!m_document) return;
    // find block that was clicked
    int position = m_document->documentLayout()->hitTest(event->localPos(), Qt::FuzzyHit);
    QTextBlock block = m_document->findBlock(position);
    int blockNum = block.blockNumber();

    const QTextBlock& matchingBlock = m_textEdit->documentHandler()->target()->findBlockByNumber(blockNum);
    lv::ProjectDocumentBlockData* userData = static_cast<lv::ProjectDocumentBlockData*>(matchingBlock.userData());
    if (userData)
    {
        if (userData->collapseState() == lv::ProjectDocumentBlockData::Collapse)
        {
            m_textEdit->manageExpandCollapse(matchingBlock.blockNumber(), true);
        }
        else if (userData->collapseState() == lv::ProjectDocumentBlockData::Expand)
        {
            m_textEdit->manageExpandCollapse(matchingBlock.blockNumber(), false);
        }
    }
}

void LineSurface::triggerPreprocess()
{
    if (m_updateType == LineSurface::UpdateNone)
        m_updateType = LineSurface::UpdateOnlyPreprocess;
    polish();
    update();
}

void LineSurface::setDocument(QTextDocument *doc)
{
    if (!doc) return;
    m_document = doc;
    m_updatePending = false;
    m_document->rootFrame(); // bug fix
    LineManager* lm = dynamic_cast<LineManager*>(m_document->parent());
    triggerUpdate(m_document->lineCount(), m_document->lineCount(), 0);
    connect(lm, &LineManager::updateLineSurface, this, &LineSurface::triggerUpdate);
}

void LineSurface::unsetTextDocument()
{
    m_document = nullptr;
    triggerUpdate(m_lineNumber, 0, 0);
}

void LineSurface::triggerUpdate(int prev, int curr, int dirty)
{
    m_previousLineNumber = prev;
    m_lineNumber = curr;
    m_dirtyPos = dirty;
    polish();
    if (isComponentComplete())
    {
        updateSize();
        m_updateType = LineSurface::UpdatePaintNode;
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


    if (!m_document) {
        if (oldNode) delete oldNode;
        return nullptr;
    }

    if (m_updateType != LineSurface::UpdatePaintNode && oldNode != nullptr) {
        // Update done in preprocess() in the nodes
        m_updateType = LineSurface::UpdateNone;
        return oldNode;
    }

    m_updateType = LineSurface::UpdateNone;

    if (!oldNode) {
        // If we had any QQuickTextNode node references, they were deleted along with the root node
        // But here we must delete the Node structures in textNodeMap
        qDeleteAll(m_textNodeMap);
        m_textNodeMap.clear();
    }

    RootNode *rootNode = static_cast<RootNode *>(oldNode);

    TextNodeEngine engine;
    TextNodeEngine frameDecorationsEngine;

    if (numberOfDigits(m_previousLineNumber) != numberOfDigits(m_lineNumber) || m_dirtyPos >= m_textNodeMap.size()){
        m_dirtyPos = 0;
    }
    if (!oldNode  || m_dirtyPos != -1) {

        if (!oldNode)
            rootNode = new RootNode;

        // delete all dirty nodes
        auto lineNumIt = m_textNodeMap.begin();
        int hiddenNum = 0; auto itPrime = m_document->rootFrame()->begin();
        for (int k=0; k<m_dirtyPos; k++)
        {
            if (!itPrime.currentBlock().isVisible()) hiddenNum++;
            ++itPrime;
        }
        for (int k=0; k<m_dirtyPos-hiddenNum; k++, lineNumIt++);
        while (lineNumIt != m_textNodeMap.end())
        {
            rootNode->removeChildNode((*lineNumIt)->textNode());
            delete (*lineNumIt)->textNode();
            delete *lineNumIt;
            lineNumIt = m_textNodeMap.erase(lineNumIt);
        }

        // FIXME: the text decorations could probably be handled separately (only updated for affected textFrames)
        rootNode->resetFrameDecorations(new TextNode(this));
        resetEngine(&frameDecorationsEngine, m_color, QColor(), QColor());

        TextNode *node = nullptr;

        int currentNodeSize = 0;
        QMatrix4x4 basePositionMatrix;
        rootNode->setMatrix(basePositionMatrix);

        QPointF nodeOffset;
        if (m_document) {
            QList<QTextFrame *> frames;
            frames.append(m_document->rootFrame());

            while (!frames.isEmpty()) { //INFO: Root frame
                QTextFrame *textFrame = frames.takeFirst();
                frames.append(textFrame->childFrames());
                frameDecorationsEngine.addFrameDecorations(m_document, textFrame);

                //INFO: creating the text node
                node = new TextNode(this);
                resetEngine(&engine, m_color, QColor(), QColor());

                if (textFrame->firstPosition() > textFrame->lastPosition()
                        && textFrame->frameFormat().position() != QTextFrameFormat::InFlow) {

                    updateNodeTransform(node, m_document->documentLayout()->frameBoundingRect(textFrame).topLeft());
                    const int pos = textFrame->firstPosition() - 1;
                    ProtectedLayoutAccessor *a = static_cast<ProtectedLayoutAccessor *>(m_document->documentLayout());
                    QTextCharFormat format = a->formatAccessor(pos);
                    QTextBlock block = textFrame->firstCursorPosition().block();
                    engine.setCurrentLine(block.layout()->lineForTextPosition(pos - block.position()));
                    engine.addTextObject(QPointF(0, 0), format, TextNodeEngine::Unselected, m_document,
                                                  pos, textFrame->frameFormat().position());
                } else {
                    // Having nodes spanning across frame boundaries will break the current bookkeeping mechanism. We need to prevent that.
                    QList<int> frameBoundaries;
                    frameBoundaries.reserve(frames.size());
                    Q_FOREACH (QTextFrame *frame, frames)
                        frameBoundaries.append(frame->firstPosition());
                    std::sort(frameBoundaries.begin(), frameBoundaries.end());

                    QTextFrame::iterator it = textFrame->begin();
                    for (int k=0; k<m_dirtyPos; k++, it++);

                    while (!it.atEnd()) {

                        QTextBlock block = it.currentBlock();
                        ++it;
                        if (!block.isVisible()) continue;

                        if (!engine.hasContents()) {
                            nodeOffset = m_document->documentLayout()->blockBoundingRect(block).topLeft();
                            updateNodeTransform(node, nodeOffset);
                            int offset = m_textEdit->getPaletteManager()->drawingOffset(block.blockNumber(), false);
                            nodeOffset.setY(nodeOffset.y() - offset);
                        }

                        engine.addTextBlock(m_document, block, -nodeOffset, m_color, QColor(), -1, -1);
                        currentNodeSize += block.length();

                        currentNodeSize = 0;
                        engine.addToSceneGraph(node, QQuickText::Normal, QColor());
                        m_textNodeMap.append(new LineSurface::Node(-1, node));
                        rootNode->appendChildNode(node);
                        node = new TextNode(this);
                        resetEngine(&engine, m_color, QColor(), QColor());
                    }
                }
                engine.addToSceneGraph(node, QQuickText::Normal, QColor());
                m_textNodeMap.append(new LineSurface::Node(-1, node));
                rootNode->appendChildNode(node);            }
        }


        frameDecorationsEngine.addToSceneGraph(rootNode->frameDecorationsNode, QQuickText::Normal, QColor());
        // Now prepend the frame decorations since we want them rendered first, with the text nodes and cursor in front.
        rootNode->prependChildNode(rootNode->frameDecorationsNode);

        // Since we iterate over blocks from different text frames that are potentially not sorted
        // we need to ensure that our list of nodes is sorted again:
        std::sort(m_textNodeMap.begin(), m_textNodeMap.end(), &comesBefore);
    }

    QTextBlock block;
    for (block = m_document->firstBlock(); block.isValid(); block = block.next()) {
        if (block.layout() != nullptr && block.layout()->engine() != nullptr)
            block.layout()->engine()->resetFontEngineCache();
    }

    return rootNode;
}

//### we should perhaps be a bit smarter here -- depending on what has changed, we shouldn't
//    need to do all the calculations each time
void LineSurface::updateSize()
{
    if (!m_document) return;
    QSizeF layoutSize = m_document->documentLayout()->documentSize();
    setImplicitSize(layoutSize.width(), layoutSize.height());
}

}
