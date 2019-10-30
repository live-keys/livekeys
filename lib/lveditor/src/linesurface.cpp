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
#include "linecontrol.h"
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
    , m_font(QFont())
    , m_lineControl(nullptr)
    , m_document(new QTextDocument(this))
    , m_updateType(UpdatePaintNode)
    , m_textEdit(nullptr), m_previousLineNumber(0)
    , m_lineNumber(0), m_dirtyPos(0)
    , m_updatePending(false)
{
    setFlag(QQuickItem::ItemHasContents);
    setAcceptHoverEvents(true);
    m_document->documentLayout(); // bug fix
    m_document->rootFrame(); // bug fix
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
    m_dirtyPos = blockNum;
    m_document->markContentsDirty(firstDirtyBlock.position() - 1, m_document->characterCount() - firstDirtyBlock.position() + 1);


    triggerUpdate();
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
    m_lineControl = te->lineControl();
    setLineDocumentFont(te->font());
    setFlag(QQuickItem::ItemHasContents);
    setAcceptedMouseButtons(Qt::AllButtons);

    m_dirtyPos = 0;
    m_previousLineNumber = 0;
    m_lineNumber = 0;

    QObject::connect(m_textEdit->lineControl(), &LineControl::lineDelta, this, &LineSurface::lineNumberChanged);
    QObject::connect(m_textEdit->lineControl(), &LineControl::refreshAfterCollapseChange, this, &LineSurface::triggerUpdate);
    QObject::connect(m_textEdit->lineControl(), &LineControl::refreshAfterPaletteChange, this, &LineSurface::paletteSlot);

    if (m_viewportBuffer != QRect())
    {
        setViewport(m_viewportBuffer);
        m_viewportBuffer = QRect();
    }
    triggerUpdate();
}

void LineSurface::resetViewportDocument()
{
    if (!m_document) return;
    m_document->clear();
    QTextCursor cursor(m_document);
    cursor.movePosition(QTextCursor::MoveOperation::End);
    for (int i = m_bounds.first+1; i <= m_bounds.second; i++)
    {
        if (i!=m_bounds.first+1) cursor.insertBlock();
        std::string s = std::to_string(i) + "  ";
        if (i < 10) s = " " + s;
        const QString a(s.c_str());

        cursor.insertText(a);
    }


    updateCollapseSymbols();
}

void LineSurface::setViewport(QRect view)
{
    if (m_viewport.y() == view.y() && m_viewport.height() == view.height() && m_bounds != std::make_pair(-1,-1)) return;
    if (!m_lineControl){
        m_viewportBuffer = view;
        return;
    }

    m_viewport = view;

    checkSectionsAndBounds();

    emit viewportChanged(view);

    polish();
    if (isComponentComplete())
    {
        updateSize();
        m_updateType = LineSurface::UpdatePaintNode;
        update();
    }
}

void LineSurface::clearViewportDocument()
{
    if (m_document) m_document->clear();
    polish();
    if (isComponentComplete())
    {
        updateSize();
        m_updateType = LineSurface::UpdatePaintNode;
        update();
    }
}

void LineSurface::checkSectionsAndBounds()
{
    auto oldSections = m_visibleSections;
    m_visibleSections = m_lineControl->visibleSectionsForViewport(m_viewport);

    auto oldBounds = m_bounds;
    m_bounds = visibleSectionsBounds();

    if (oldBounds.first != m_bounds.first || oldBounds.second != m_bounds.second)
    {
        resetViewportDocument();
    }
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

void LineSurface::changeLastCharInViewportDocumentBlock(int blockNumber, char c)
{
    QTextBlock b = m_document->findBlockByNumber(blockNumber);
    QTextCursor cursor(b);
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::EndOfBlock);
    cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveMode::KeepAnchor);
    cursor.removeSelectedText();
    cursor.insertText(QString(c));
    cursor.endEditBlock();
}

void LineSurface::replaceTextInViewportDocumentBlock(int blockNumber, std::string s)
{
    QTextBlock b = m_document->findBlockByNumber(blockNumber);
    QTextCursor cursor(b);
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::EndOfBlock);
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveMode::KeepAnchor);
    cursor.removeSelectedText();
    cursor.insertText(QString(s.c_str()));
    cursor.endEditBlock();
}

void LineSurface::updateCollapseSymbols()
{
    int curr = m_bounds.first;
    auto it = m_document->rootFrame()->begin();
    while (it != m_document->rootFrame()->end() && curr < m_bounds.second)
    {
        if (!m_textEdit || !m_textEdit->documentHandler()) break;
        auto currBlock = m_textEdit->documentHandler()->target()->findBlockByNumber(curr);
        lv::ProjectDocumentBlockData* userData =
                static_cast<lv::ProjectDocumentBlockData*>(currBlock.userData());

        if (userData) {
            if (userData->isCollapsible())
            {
                if (m_lineControl->isJumpForwardLine(curr, true) > 0) // is first line of collapse?
                    changeLastCharInViewportDocumentBlock(curr-m_bounds.first, '>');
                else
                    changeLastCharInViewportDocumentBlock(curr-m_bounds.first, 'v');
            } else
                changeLastCharInViewportDocumentBlock(curr-m_bounds.first, ' ');

            userData->setStateChangeFlag(false);
        } else changeLastCharInViewportDocumentBlock(curr-m_bounds.first, ' ');

        ++curr; ++it;
    }

}

std::pair<int, int> LineSurface::visibleSectionsBounds()
{
    int start = -1;
    for (auto it = m_visibleSections.begin(); it != m_visibleSections.end(); ++it)
    {
        if (it->palette) continue;

        start = it->start;
        break;
    }

    if (start == -1) return std::make_pair(-1, -1);

    auto rit = m_visibleSections.rbegin();
    while (rit->palette) ++rit;

    return std::make_pair(start, rit->start + rit->size);
}

void LineSurface::mousePressEvent(QMouseEvent* event)
{
    if (!m_document) return;

    int visibleBlockNumber = static_cast<int>(event->localPos().y()) / m_textEdit->lineControl()->blockHeight();
    int absBlockNum = m_textEdit->lineControl()->visibleToAbsolute(visibleBlockNumber);

    const QTextBlock& matchingBlock = m_textEdit->documentHandler()->target()->findBlockByNumber(absBlockNum);
    const QTextBlock& lineDocBlock = m_document->findBlockByNumber(absBlockNum - m_bounds.first);
    lv::ProjectDocumentBlockData* userData = static_cast<lv::ProjectDocumentBlockData*>(matchingBlock.userData());
    if (userData && userData->isCollapsible())
    {
        QString s = lineDocBlock.text();
        if (s[s.length()-1] == 'v')
        {
            m_textEdit->manageExpandCollapse(matchingBlock.blockNumber(), true);
        }
        else if (s[s.length()-1] == '>')
        {
            m_textEdit->manageExpandCollapse(matchingBlock.blockNumber(), false);
        }


        m_visibleSections = m_lineControl->visibleSectionsForViewport(m_viewport);
        m_bounds = visibleSectionsBounds();
        resetViewportDocument();
        polish();

        if (isComponentComplete())
        {
            updateSize();
            m_updateType = LineSurface::UpdatePaintNode;
            update();
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
/*
void LineSurface::setDocument(QTextDocument *doc)
{
    if (!doc) return;
    m_document = doc;
    m_updatePending = false;
    m_document->rootFrame(); // bug fix
    triggerUpdate(m_document->lineCount(), 0);
}
*/


void LineSurface::triggerUpdate()
{
    if (m_viewportBuffer != QRect())
    {
        setViewport(m_viewportBuffer);
        m_viewportBuffer = QRect();
    }

    checkSectionsAndBounds();

    resetViewportDocument();

    polish();
    if (isComponentComplete())
    {
        updateSize();
        m_updateType = LineSurface::UpdatePaintNode;
        update();
    }
}

void LineSurface::setLineDocumentFont(const QFont &font)
{
    m_document->setDefaultFont(font);
}

void LineSurface::lineNumberChanged()
{
    m_previousLineNumber = m_lineNumber;
    m_lineNumber = m_textEdit->documentHandler()->target()->blockCount();

    m_visibleSections = m_lineControl->visibleSectionsForViewport(m_viewport);
    m_bounds = visibleSectionsBounds();
    resetViewportDocument();
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

    m_dirtyPos = 0; // TODO: better solution needed

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

                            int offset = m_bounds.first * m_textEdit->lineControl()->blockHeight();
                            if (m_textEdit && m_textEdit->lineControl())
                                offset += m_textEdit->lineControl()->drawingOffset(block.blockNumber() + m_bounds.first, false);
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
    setImplicitSize(layoutSize.width(), m_textEdit->totalHeight());
}

}
