#include <queue>
#include <list>
#include "collapsedsection.h"
#include "linemanager.h"
#include "linesurface.h"
// #include "qtextdocument.h"

namespace lv {

bool LineManager::before(int pos1, int num1, int pos2, int num2)
{
    Q_UNUSED(num2)
    return pos1 < pos2 && pos1 + num1 < pos2;
}

bool LineManager::inside(int pos1, int num1, int pos2, int num2)
{
    return pos1 >= pos2 && (pos1 + num1) <= (pos2 + num2);
}

bool LineManager::after(int pos1, int num1, int pos2, int num2)
{
    Q_UNUSED(num1)
    return pos2 < pos1 && pos2+num2<=pos1;
}

void LineManager::writeOutContentOfSections()
{
    qDebug() << "---------------sections-------------";
    auto it = sections.begin();
    int cnt = 0;
    while (it != sections.end())
    {
        CollapsedSection* cs = *it;
        qDebug() << cnt << ": (" << cs->position << cs->numberOfLines << ")";
        ++it; ++cnt;
    }
    qDebug() << "--------------------------------";
}

void LineManager::replaceTextInLineDocumentBlock(int blockNumber, std::string s)
{
    QTextBlock b = lineDocument->findBlockByNumber(blockNumber);
    QTextCursor cursor(b);
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::EndOfBlock);
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveMode::KeepAnchor);
    cursor.removeSelectedText();
    cursor.insertText(QString(s.c_str()));
    cursor.endEditBlock();
}

void LineManager::changeLastCharInLineDocumentBlock(int blockNumber, char c)
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

void LineManager::updateLineDocument()
{
    auto itSections = sections.begin();
    while (itSections != sections.end() && (*itSections)->position < dirtyPos) ++itSections;
    int curr = dirtyPos;
    auto it = lineDocument->rootFrame()->begin();
    for (int i = 0; i < curr; i++)
    {
        ++it;
    }
    while (it != lineDocument->rootFrame()->end())
    {
        auto currBlock = parentDocument->findBlockByNumber(curr);
        lv::ProjectDocumentBlockData* userData =
                static_cast<lv::ProjectDocumentBlockData*>(currBlock.userData());

        bool visible = true;
        if (itSections != sections.end())
        {
            CollapsedSection* sec = (*itSections);
            if (curr == sec->position)
            {
                userData->collapse();
                userData->setNumOfCollapsedLines(sec->numberOfLines);
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
        if (userData) {
            switch (userData->collapseState())
            {
            case lv::ProjectDocumentBlockData::Collapse:
                changeLastCharInLineDocumentBlock(curr, 'v');
                break;
            case lv::ProjectDocumentBlockData::Expand:
                changeLastCharInLineDocumentBlock(curr, '>');
                break;
            case lv::ProjectDocumentBlockData::NoCollapse:
                changeLastCharInLineDocumentBlock(curr, ' ');
                break;
            }
            userData->setStateChangeFlag(false);
        }
        ++curr; ++it;
    }
    auto firstDirtyBlock = lineDocument->findBlockByNumber(dirtyPos);
    lineDocument->markContentsDirty(firstDirtyBlock.position(), lineDocument->characterCount() - firstDirtyBlock.position());
}


bool cmp(CollapsedSection* a, CollapsedSection* b)
{
    return a->position < b->position;
}

void LineManager::collapseLines(int pos, int num) {

    std::list<CollapsedSection*> nested;
    auto it = sections.begin();
    showHideTextEditLines(false, pos, num);

    while (it != sections.end()) {
        CollapsedSection* sec = *it;
        if (before(sec->position, sec->numberOfLines, pos, num)) {
            ++it; continue; }
        if (inside(sec->position, sec->numberOfLines, pos, num))
        {
            nested.emplace_back(sec);
            it = sections.erase(it);
            continue;
        }
        if (after(sec->position, sec->numberOfLines, pos, num)) {
            ++it; continue; }

        qDebug() << "Overlap error 1!";
        return;
    }

    CollapsedSection* nuSection = new CollapsedSection(pos, num);
    nuSection->nestedSections = std::move(nested);

    sections.emplace_back(nuSection);
    sections.sort(cmp);

}

void LineManager::expandLines(int pos, int num)
{
    auto it = sections.begin();
    while (it != sections.end() && (*it)->position != pos) ++it;

    showHideTextEditLines(true, pos, num);
    if (it != sections.end())
    {
        CollapsedSection* sec = *it;
        sections.erase(it);

        std::list<CollapsedSection*> nested;
        nested = std::move(sec->nestedSections);

        for (CollapsedSection* nestedSec: nested)
        {
            sections.push_back(nestedSec);
            sections.sort(cmp);
            showHideTextEditLines(false, nestedSec->position, nestedSec->numberOfLines);
        }

        delete sec;
    }
    else
    {
        qDebug() << "No matching collapse section!";
    }

}

void LineManager::linesAdded()
{
    int pos = dirtyPos;
    int num = lineNumber - previousLineNumber;
    QTextCursor cursor(lineDocument);
    cursor.movePosition(QTextCursor::MoveOperation::End);
    for (int i = previousLineNumber + 1; i <= lineNumber; i++)
    {
        if (i!=1) cursor.insertBlock();
        std::string s = std::to_string(i) + "  ";
        if (i < 10) s = " " + s;
        const QString a(s.c_str());

        cursor.insertText(a);
    }

    auto it = sections.begin();
    while (it != sections.end())
    {
        CollapsedSection* sec = *it;
        if (sec->position >= pos)
        {
            std::queue<CollapsedSection*> q;
            q.push(sec);
            while (!q.empty())
            {
                CollapsedSection* curr = q.front(); q.pop();
                curr->position += num;
                for (CollapsedSection* sec: curr->nestedSections)
                {
                    q.push(sec);
                }
            }
        }
        ++it;
    }
}

void LineManager::linesRemoved()
{
    int pos = dirtyPos;
    int num = previousLineNumber - lineNumber;
    for (int i = previousLineNumber-1; i >= lineNumber; i--)
    {
        QTextCursor cursor(lineDocument->findBlockByNumber(i));
        cursor.select(QTextCursor::BlockUnderCursor);
        cursor.removeSelectedText();
    }

    auto it = sections.begin();
    std::queue<CollapsedSection*> qs;

    while (it != sections.end())
    {
        CollapsedSection* sec = *it;
        if (before(sec->position, sec->numberOfLines, pos, num)) {
            ++it; continue; }
        if (inside(sec->position, sec->numberOfLines, pos, num))
        {
            qs.emplace(sec);
            it = sections.erase(it);
            while (!qs.empty())
            {
                CollapsedSection* pop = qs.back(); qs.pop();
                for (auto it = pop->nestedSections.begin(); it != pop->nestedSections.end(); ++it)
                    qs.push(*it);
                delete pop;
            }
        }
        if (after(sec->position, sec->numberOfLines, pos, num))
        {
            std::queue<CollapsedSection*> q;
            q.push(sec);
            while (!q.empty())
            {
                CollapsedSection* curr = q.front(); q.pop();
                curr->position -= num;
                for (CollapsedSection* sec: curr->nestedSections)
                {
                    q.push(sec);
                }
            }
            it++;
            continue;
        }

        qDebug() << "Overlap error 2!";
        return;
    }
}

std::list<CollapsedSection*> & LineManager::getSections()
{
    return sections;
}

std::pair<int, int> LineManager::isLineAfterCollapsedSection(int lineNumber)
{
    auto it = sections.begin();
    while (it != sections.end())
    {
        auto cs = *it;
        if  (lineNumber == cs->position + cs->numberOfLines + 1) return std::make_pair(cs->position, cs->numberOfLines);
        ++it;
    }

    return std::make_pair(-1, -1);
}

std::pair<int, int> LineManager::isFirstLineOfCollapsedSection(int lineNumber)
{
    auto it = sections.begin();
    while (it != sections.end())
    {
        auto cs = *it;
        if  (lineNumber == cs->position) return std::make_pair(cs->position, cs->numberOfLines);
        ++it;
    }

    return std::make_pair(-1, -1);
}

void LineManager::setDirtyPos(int p)
{
    if (!updatePending || p < dirtyPos){
        updatePending = true;
        dirtyPos = p;
    }
}

void LineManager::textDocumentFinishedUpdating(int newLineNumber)
{
    previousLineNumber = lineNumber;
    lineNumber = newLineNumber;

    int deltaLineNumber = abs(previousLineNumber - lineNumber);
    if (deltaLineNumber)
    {
        if (previousLineNumber < lineNumber)
            linesAdded();
        else
            linesRemoved();
    }

    updateLineDocument();
    updatePending = false;
    emit updateLineSurface(previousLineNumber, lineNumber, dirtyPos);
}

void LineManager::setLineDocumentFont(const QFont &font)
{
    lineDocument->setDefaultFont(font);
}

void LineManager::setParentDocument(QTextDocument *td)
{
    parentDocument = td;
}

LineManager::LineManager(QObject *parent)
    : QObject(parent), sections(), lineDocument(new QTextDocument(this)),
      previousLineNumber(0), lineNumber(0), dirtyPos(0), updatePending(false)
{
    updatePending = false;
    QTextOption opt;
    opt.setAlignment(Qt::AlignRight | Qt::AlignTop);
    opt.setTextDirection(Qt::LeftToRight);
    lineDocument->setDefaultTextOption(opt);
}

LineManager::~LineManager() {}

}
