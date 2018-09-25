#include "textnodeengine_p.h"
#include "qtextcursor.h"
#include "textedit_p_p.h"
#include "qmath.h"
#include "qtextdocument.h"
#include "qabstracttextdocumentlayout.h"
#include "collapsedsection.h"
#include "linenumbersurface.h"
#include "textnode_p.h"

namespace lv {

LineRootNode::LineRootNode() : frameDecorationsNode(nullptr) { }

void LineRootNode::resetFrameDecorations(TextNode* newNode)
{
    if (frameDecorationsNode) {
        removeChildNode(frameDecorationsNode);
        delete frameDecorationsNode;
    }
    frameDecorationsNode = newNode;
    newNode->setFlag(QSGNode::OwnedByParent);
}

LineNumberSurface::LineNumberSurface(QQuickItem *parent)
    : QQuickItem(parent)
    , textEdit(nullptr), lineDocument(nullptr), prevLineNumber(0)
    , lineNumber(0), lineNodes(), dirtyPos(0)
    , lineManager(new LineManager)
    , m_color(QColor(255, 255, 255, 255))
{
    setFlag(QQuickItem::ItemHasContents, true);
    lineManager->setLineSurface(this);
}

void LineNumberSurface::updateSize()
{
    // qreal newWidth = lineDocument->idealWidth();
    QSizeF layoutSize = lineDocument->documentLayout()->documentSize();
    setImplicitSize(layoutSize.width(), layoutSize.height());
}

void LineNumberSurface::setComponents(TextEdit* te)
{
    textEdit = te;
    init();
}

LineNumberSurface::~LineNumberSurface()
{
       auto it = lineNodes.begin();
       while (it != lineNodes.end())
       {
           delete *it;
           it = lineNodes.erase(it);
       }
}

QSGNode* LineNumberSurface::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData)
{
    Q_UNUSED(updatePaintNodeData);

    if ( !lineDocument )
        return oldNode;

    if (!oldNode) {
        qDeleteAll(lineNodes);
        lineNodes.clear();
    }

    LineRootNode* rootNode;
    if (!oldNode)
        rootNode = new LineRootNode;
    else
        rootNode = static_cast<LineRootNode *>(oldNode);

    TextNodeEngine engine;
    TextNodeEngine frameDecorationsEngine;

    if (numberOfDigits(prevLineNumber) != numberOfDigits(lineNumber) || dirtyPos >= lineNodes.size())
        dirtyPos = 0;

    if (!oldNode || dirtyPos != -1)
    {
        TextNode* node = new TextNode(this);
        node->setUseNativeRenderer(true);
        rootNode->resetFrameDecorations(node);

        frameDecorationsEngine = TextNodeEngine();
        frameDecorationsEngine.setTextColor(m_color);

        QMatrix4x4 basePositionMatrix;
        rootNode->setMatrix(basePositionMatrix);

        QPointF nodeOffset;
        QTextFrame* textFrame = lineDocument->rootFrame();
        frameDecorationsEngine.addFrameDecorations(lineDocument, textFrame);

        // delete all dirty nodes
        QList<TextNode*>::iterator lineNumIt = lineNodes.begin();
        for (int k=0; k<dirtyPos; k++, lineNumIt++);
        while (lineNumIt != lineNodes.end())
        {

            rootNode->removeChildNode((*lineNumIt));
            delete *lineNumIt;
            lineNumIt = lineNodes.erase(lineNumIt);
        }

        QTextFrame::iterator it = textFrame->begin();
        for (int k=0; k<dirtyPos; k++, it++);
        while (!it.atEnd())
        {
            node = new TextNode(this);
            node->setUseNativeRenderer(true);
            engine = TextNodeEngine();
            engine.setTextColor(m_color);

            QTextBlock block = it.currentBlock();
            it++;

            if (!block.isVisible())
            {
                delete node;
                continue;
            }

            nodeOffset = lineDocument->documentLayout()->blockBoundingRect(block).topLeft();
            QMatrix4x4 transformMatrix;
            transformMatrix.translate(static_cast<float>(nodeOffset.x()), static_cast<float>(nodeOffset.y()));
            node->setMatrix(transformMatrix);
            engine.addTextBlock(lineDocument, block, -nodeOffset, m_color, QColor(), -1, -1);
            engine.addToSceneGraph(node, QQuickText::Normal, QColor());
            lineNodes.append(node);
            rootNode->appendChildNode(node);
        }

        frameDecorationsEngine.addToSceneGraph(rootNode->frameDecorationsNode, QQuickText::Normal, QColor());
        rootNode->prependChildNode(rootNode->frameDecorationsNode);
    }

    QTextBlock block;
    for (block = lineDocument->firstBlock(); block.isValid(); block = block.next()) {
        if (block.layout() != nullptr && block.layout()->engine() != nullptr)
            block.layout()->engine()->resetFontEngineCache();
    }

    return rootNode;
}

void LineNumberSurface::textDocumentFinished()
{
    auto document = textEdit->documentHandler()->target();
    prevLineNumber = lineNumber;
    lineNumber = document->blockCount();

    deltaLineNumber = abs(prevLineNumber - lineNumber);
    if (deltaLineNumber)
    {
        if (prevLineNumber < lineNumber) linesAdded();
        else linesRemoved();
    }
    else
    {
        QTextBlock block = document->findBlockByNumber(dirtyPos);
        auto userData = static_cast<ProjectDocumentBlockData*>(block.userData());
        if (userData && userData->stateChangeFlag())
        {
            userData->setStateChangeFlag(false);
            updateLineDocument();
        }
    }
}

void LineNumberSurface::updateLineDocument()
{
    // we look for a collapsed section after the position where we made a change
    std::list<CollapsedSection*> &sections = lineManager->getSections();
    auto itSections = sections.begin();
    while (itSections != sections.end() && (*itSections)->position < dirtyPos) ++itSections;
    int curr = dirtyPos;
    auto it = lineDocument->rootFrame()->begin();
    for (int i = 0; i < curr; i++) ++it;
    while (it != lineDocument->rootFrame()->end())
    {
        auto currBlock = textEdit->documentHandler()->target()->findBlockByNumber(curr);
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
    auto firstDirtyBlock = lineDocument->findBlockByNumber(dirtyPos);
    lineDocument->markContentsDirty(firstDirtyBlock.position(), lineDocument->characterCount() - firstDirtyBlock.position());

    polish();
    if (isComponentComplete())
    {
        updateSize();
        update();
    }
}

void LineNumberSurface::linesAdded()
{

    lineManager->linesAdded(dirtyPos, deltaLineNumber);

    QTextCursor cursor(lineDocument);
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

void LineNumberSurface::linesRemoved()
{
    lineManager->linesRemoved(dirtyPos, deltaLineNumber);

    for (int i = prevLineNumber-1; i >= lineNumber; i--)
    {
        QTextCursor cursor(lineDocument->findBlockByNumber(i));
        cursor.select(QTextCursor::BlockUnderCursor);
        cursor.removeSelectedText();
    }

    updateLineDocument();
}


void LineNumberSurface::expandCollapseSkeleton(int pos, int num, QString &replacement, bool show)
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


void LineNumberSurface::collapseLines(int pos, int num)
{
    changeLastCharInBlock(pos, '>');

    QTextBlock matchingBlock = textEdit->documentHandler()->target()->findBlockByNumber(pos);
    // ProjectDocumentBlockData* userData = static_cast<ProjectDocumentBlockData*>(matchingBlock.userData());
    QString s = matchingBlock.text();
    // s+=userData->replacementString;
    expandCollapseSkeleton(pos, num, s, false);
}

void LineNumberSurface::expandLines(int pos, int num)
{
    changeLastCharInBlock(pos, 'v');

    QTextBlock matchingBlock = textEdit->documentHandler()->target()->findBlockByNumber(pos);
    // lv::ProjectDocumentBlockData* userData = static_cast<lv::ProjectDocumentBlockData*>(matchingBlock.userData());
    QString s = matchingBlock.text();
    // s.chop(userData->replacementString.length());
    expandCollapseSkeleton(pos, num, s, true);
}


void LineNumberSurface::showHideLines(bool show, int pos, int num)
{
    auto it = lineDocument->rootFrame()->begin();
    for (int i = 0; i < pos; i++, ++it);
    int startBlock = it.currentBlock().position();
    ++it;
    int length = 0;
    for (int i = 0; i < num; i++)
    {
        it.currentBlock().setVisible(show);
        length += it.currentBlock().length();
        ++it;
    }
    lineDocument->markContentsDirty(startBlock, length);

}

void LineNumberSurface::setDirtyBlockPosition(int pos)
{
    dirtyPos = pos;
}

void LineNumberSurface::changeLastCharInBlock(int blockNumber, char c)
{
    QTextBlock b = lineDocument->findBlockByNumber(blockNumber);
    QTextCursor cursor(b);
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::EndOfBlock);
    cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveMode::KeepAnchor);
    cursor.removeSelectedText();
    cursor.insertText(QString(c));
    cursor.endEditBlock();
}

void LineNumberSurface::init() {
    font = textEdit->font();

    lineDocument = new QTextDocument(this);
    lineDocument->setDefaultFont(font);
    QTextOption opt;
    opt.setAlignment(Qt::AlignRight | Qt::AlignTop);
    opt.setTextDirection(Qt::LeftToRight);
    lineDocument->setDefaultTextOption(opt);
    setFlag(QQuickItem::ItemHasContents);

    dirtyPos = 0;
    prevLineNumber = 0;
    textDocumentFinished();


    QObject::connect(textEdit, &TextEdit::dirtyBlockPosition, this, &LineNumberSurface::setDirtyBlockPosition);
    QObject::connect(textEdit, &TextEdit::textDocumentFinishedUpdating, this, &LineNumberSurface::textDocumentFinished);



    setAcceptedMouseButtons(Qt::AllButtons);

}



void LineNumberSurface::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    // find block that was clicked
    int position = lineDocument->documentLayout()->hitTest(event->localPos(), Qt::FuzzyHit);
    QTextBlock block = lineDocument->findBlock(position);
    int blockNum = block.blockNumber();

    QTextBlock matchingBlock = textEdit->documentHandler()->target()->findBlockByNumber(blockNum);
    lv::ProjectDocumentBlockData* userData = static_cast<lv::ProjectDocumentBlockData*>(matchingBlock.userData());
    if (userData)
    {
        if (userData->collapseState() == lv::ProjectDocumentBlockData::Collapse)
        {
            userData->collapse();
#ifndef COLLAPSE_DEBUG
            int num; QString repl;
            userData->onCollapse()(matchingBlock, num, repl);
            userData->setNumOfCollapsedLines(num);
            userData->setReplacementString(repl);
#endif
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

}
