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
    , lineManager(new LineManager)
    , textEdit(nullptr), lineDocument(nullptr), prevLineNumber(0)
    , lineNumber(0), lineNodes(), dirtyPos(0)
    , isInitialized(false)
    , m_color(QColor(255, 255, 255, 255))
{
    setFlag(QQuickItem::ItemHasContents, true);
    lineManager->setLineSurface(this);
}

void LineNumberSurface::updateSize()
{
    // qreal newWidth = lineDocument->idealWidth();
    QFontMetricsF fm(font);

    int num = numberOfDigits(lineNumber) + 2;
    qreal newWidth = lineDocument->isEmpty() ? qCeil(fm.maxWidth()) : qCeil(num*fm.maxWidth());
    qreal newHeight = lineDocument->isEmpty() ? qCeil(fm.height()) : lineDocument->size().height();
    lineDocument->setTextWidth(newWidth);
    setImplicitSize(newWidth, newHeight);
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
       delete lineDocument;
}

QSGNode* LineNumberSurface::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData)
{
    Q_UNUSED(updatePaintNodeData);

    LineRootNode* rootNode;
    if (!oldNode)
        rootNode = new LineRootNode;
    else rootNode = static_cast<LineRootNode *>(oldNode);

    TextNodeEngine engine;
    TextNodeEngine frameDecorationsEngine;

    if (numberOfDigits(prevLineNumber) != numberOfDigits(lineNumber)) dirtyPos = 0;

    // dirty fix for overlapping update bug - redraw everything if a line node is missing
    /*if (oldNode && prevLineNumber != lineNodes.count())
    {
        dirtyPos = 0;
    }*/

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

    if (!isInitialized && lineNumber != 0 && lineNodes.size() == lineNumber)
    {
        isInitialized = true;
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
        if (dirtyPos == -1)
        {
            dirtyPos = 0;
            updateLineDocument();
            return;
        }
        QTextBlock block = document->findBlockByNumber(dirtyPos);
        auto userData = static_cast<ProjectDocumentBlockData*>(block.userData());
        if (userData && userData->stateChangeFlag())
        {
            userData->setStateChangeFlag(false);
            updateLineDocument();
        }
    }
}

#ifdef COLLAPSE_DEBUG
void LineNumberSurface::testSetup1()
{
    QTextDocument* td = textEdit->documentHandler()->target();

    QTextCursor cursor(td);

    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveMode::KeepAnchor);
    cursor.removeSelectedText();
    cursor.insertText("#include <iostream>\n\n// test program\n\
// hide/show lines\n\
class Test {\n \
\n \
public:\n\
  int max(int a, int b);\n\
  int min(int a, int b);\n\
  int reduction(int* a, int n, int (*f)(int, int));\n\
\n\
private:\n\
  static int numOfCalls = 0;\n\
\n\
};\n\
\n\
int Test::max(int a, int b)\n\
{\n\
  if (a > b)\n\
  {\n\
      return a;\n\
  }\n\
  else\n\
  {\n\
      return b;\n\
  }\n\
}\n\
\n\
int Test::min(int a, int b)\n\
{\n\
  if (a > b)\n\
  {\n\
      return b;\n\
  }\n\
  else\n\
  {\n\
      return a;\n\
  }\n\
}\n\
\n\
int Test::reduction(int* a, int n, int (*f)(int, int))\n\
{\n\
  int res = a[0];\n\
  for (int i = 1; i < n; i++)\n\
  {\n\
      res = f(res, a[i]);\n\
  }\n\
\n\
  return res;\n\
}"
    );
    cursor.endEditBlock();

}

int callback(int pos, QString &res)
{
    switch (pos)
    {
    case 4:
        res = " ... }";
        return 10;
    case 16:
        res = " { ... }";
        return 10;
    case 18:
        res = " { ... }";
        return 3;
    case 22:
        res = " { ... }";
        return 3;
    case 28:
        res = " { ... }";
        return 10;
    case 30:
        res = " { ... }";
        return 3;
    case 34:
        res = " { ... }";
        return 3;
    case 40:
        res = " { ... }";
        return 9;
    default:
        res = "zamena";
        return 2;
    }

}

void LineNumberSurface::testSetup2()
{
    QTextDocument* td = textEdit->documentHandler()->target();
    int a[8] = {4, 16, 18, 22, 28, 30, 34, 40};

    for (int x: a)
    {
        QTextBlock b = td->findBlockByNumber(x);
        ProjectDocumentBlockData* bd = new ProjectDocumentBlockData();
        bd->m_collapseState = lv::ProjectDocumentBlockData::Collapse;
        bd->numOfCollapsedLines = callback(x, bd->replacementString);
        b.setUserData(bd);
        // changeLastCharInBlock(x, 'v');
    }
}
#endif

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
        lv::ProjectDocumentBlockData* userData =
                static_cast<lv::ProjectDocumentBlockData*>(textEdit->documentHandler()->target()->findBlockByNumber(curr).userData());

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

        it.currentBlock().setVisible(visible);
        if (visible) {
            if (userData && userData->collapseState() == lv::ProjectDocumentBlockData::Collapse)
            {
                changeLastCharInBlock(curr, 'v');
            }
            else if (userData && userData->collapseState() != lv::ProjectDocumentBlockData::Expand)
                changeLastCharInBlock(curr, ' ');

            visibleWidth = it.currentBlock().length();
        }
        ++curr; ++it;
    }
    auto firstDirtyBlock = lineDocument->findBlockByNumber(dirtyPos);
    lineDocument->markContentsDirty(firstDirtyBlock.position(), lineDocument->characterCount() - firstDirtyBlock.position());



    polish();
    updateSize();
    update();
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

#ifdef COLLAPSE_DEBUG
    static int callNum = 0;
    if (!callNum) testSetup2();
    callNum++;
#endif

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
#ifdef COLLAPSE_DEBUG
    testSetup1();
#endif
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
    isInitialized = false;
    textDocumentFinished();


    QObject::connect(textEdit, &TextEdit::dirtyBlockPosition, this, &LineNumberSurface::setDirtyBlockPosition);
    QObject::connect(textEdit, &TextEdit::textDocumentFinishedUpdating, this, &LineNumberSurface::textDocumentFinished);



    setAcceptedMouseButtons(Qt::AllButtons);

    visibleWidth = lineDocument->end().previous().length();
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
