#include <queue>
#include <list>
#include "linemanager.h"
#include "linesurface.h"

namespace lv {

LineManager::CollapsedSection::CollapsedSection(int pos, int num)
    : position(pos), numberOfLines(num), nestedSections() {}


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
    auto it = m_sections.begin();
    int cnt = 0;
    while (it != m_sections.end())
    {
        CollapsedSection* cs = *it;
        qDebug() << cnt << ": (" << cs->position << cs->numberOfLines << ")";
        ++it; ++cnt;
    }
    qDebug() << "--------------------------------";
}

void LineManager::replaceTextInLineDocumentBlock(int blockNumber, std::string s)
{
    QTextBlock b = m_lineDocument->findBlockByNumber(blockNumber);
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
    QTextBlock b = m_lineDocument->findBlockByNumber(blockNumber);
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
    auto itSections = m_sections.begin();
    while (itSections != m_sections.end() && (*itSections)->position < m_dirtyPos) ++itSections;
    int curr = m_dirtyPos;
    auto it = m_lineDocument->rootFrame()->begin();
    for (int i = 0; i < curr; i++)
    {
        ++it;
    }
    while (it != m_lineDocument->rootFrame()->end())
    {
        auto currBlock = m_parentDocument->findBlockByNumber(curr);
        lv::ProjectDocumentBlockData* userData =
                static_cast<lv::ProjectDocumentBlockData*>(currBlock.userData());

        bool visible = true;
        if (itSections != m_sections.end())
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
    auto firstDirtyBlock = m_lineDocument->findBlockByNumber(m_dirtyPos);
    m_lineDocument->markContentsDirty(firstDirtyBlock.position(), m_lineDocument->characterCount() - firstDirtyBlock.position());
}


bool cmp(LineManager::CollapsedSection* a, LineManager::CollapsedSection* b)
{
    return a->position < b->position;
}

void LineManager::collapseLines(int pos, int num) {

    std::list<CollapsedSection*> nested;
    auto it = m_sections.begin();
    showHideTextEditLines(false, pos, num);

    while (it != m_sections.end()) {
        CollapsedSection* sec = *it;
        if (before(sec->position, sec->numberOfLines, pos, num)) {
            ++it; continue; }
        if (inside(sec->position, sec->numberOfLines, pos, num))
        {
            nested.emplace_back(sec);
            it = m_sections.erase(it);
            continue;
        }
        if (after(sec->position, sec->numberOfLines, pos, num)) {
            ++it; continue; }

        qDebug() << "Overlap error 1!";
        return;
    }

    CollapsedSection* nuSection = new CollapsedSection(pos, num);
    nuSection->nestedSections = std::move(nested);

    m_sections.emplace_back(nuSection);
    m_sections.sort(cmp);

}

void LineManager::expandLines(int pos, int num)
{
    auto it = m_sections.begin();
    while (it != m_sections.end() && (*it)->position != pos) ++it;

    showHideTextEditLines(true, pos, num);
    if (it != m_sections.end())
    {
        CollapsedSection* sec = *it;
        m_sections.erase(it);

        std::list<CollapsedSection*> nested;
        nested = std::move(sec->nestedSections);

        for (CollapsedSection* nestedSec: nested)
        {
            m_sections.push_back(nestedSec);
            m_sections.sort(cmp);
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
    int pos = m_dirtyPos;
    int num = m_lineNumber - m_previousLineNumber;
    QTextCursor cursor(m_lineDocument);
    cursor.movePosition(QTextCursor::MoveOperation::End);
    for (int i = m_previousLineNumber + 1; i <= m_lineNumber; i++)
    {
        if (i!=1) cursor.insertBlock();
        std::string s = std::to_string(i) + "  ";
        if (i < 10) s = " " + s;
        const QString a(s.c_str());

        cursor.insertText(a);
    }

    auto it = m_sections.begin();
    while (it != m_sections.end())
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
    int pos = m_dirtyPos;
    int num = m_previousLineNumber - m_lineNumber;
    for (int i = m_previousLineNumber-1; i >= m_lineNumber; i--)
    {
        QTextCursor cursor(m_lineDocument->findBlockByNumber(i));
        cursor.select(QTextCursor::BlockUnderCursor);
        cursor.removeSelectedText();
    }

    auto it = m_sections.begin();
    std::queue<CollapsedSection*> qs;

    while (it != m_sections.end())
    {
        CollapsedSection* sec = *it;
        if (before(sec->position, sec->numberOfLines, pos, num)) {
            ++it; continue; }
        if (inside(sec->position, sec->numberOfLines, pos, num))
        {
            qs.emplace(sec);
            it = m_sections.erase(it);
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

std::list<LineManager::CollapsedSection*> & LineManager::getSections()
{
    return m_sections;
}

std::pair<int, int> LineManager::isLineAfterCollapsedSection(int lineNumber)
{
    auto it = m_sections.begin();
    while (it != m_sections.end())
    {
        auto cs = *it;
        if  (lineNumber == cs->position + cs->numberOfLines + 1) return std::make_pair(cs->position, cs->numberOfLines);
        ++it;
    }

    return std::make_pair(-1, -1);
}

std::pair<int, int> LineManager::isFirstLineOfCollapsedSection(int lineNumber)
{
    auto it = m_sections.begin();
    while (it != m_sections.end())
    {
        auto cs = *it;
        if  (lineNumber == cs->position) return std::make_pair(cs->position, cs->numberOfLines);
        ++it;
    }

    return std::make_pair(-1, -1);
}

void LineManager::setDirtyPos(int p)
{
    if (!m_updatePending || p < m_dirtyPos){
        m_updatePending = true;
        m_dirtyPos = p;
    }
}

void LineManager::textDocumentFinishedUpdating(int newLineNumber)
{
    m_previousLineNumber = m_lineNumber;
    m_lineNumber = newLineNumber;

    int deltaLineNumber = abs(m_previousLineNumber - m_lineNumber);
    if (deltaLineNumber)
    {
        if (m_previousLineNumber < m_lineNumber)
            linesAdded();
        else
            linesRemoved();
    }

    updateLineDocument();
    m_updatePending = false;
    emit updateLineSurface(m_previousLineNumber, m_lineNumber, m_dirtyPos);
}

void LineManager::setLineDocumentFont(const QFont &font)
{
    m_lineDocument->setDefaultFont(font);
}

void LineManager::setParentDocument(QTextDocument *td)
{
    m_parentDocument = td;
}

LineManager::LineManager(QObject *parent)
    : QObject(parent), m_sections(), m_lineDocument(new QTextDocument(this)),
      m_previousLineNumber(0), m_lineNumber(0), m_dirtyPos(0), m_updatePending(false)
{
    m_updatePending = false;
    QTextOption opt;
    opt.setAlignment(Qt::AlignRight | Qt::AlignTop);
    opt.setTextDirection(Qt::LeftToRight);
    m_lineDocument->setDefaultTextOption(opt);
}

LineManager::~LineManager() {}

}
