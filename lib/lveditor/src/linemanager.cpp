#include <queue>
#include <list>
#include "collapsedsection.h"
#include "linemanager.h"
#include "linenumbersurface.h"

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
    return before(pos2, num2, pos1, num1);
}



bool cmp(CollapsedSection* a, CollapsedSection* b)
{
    return a->position < b->position;
}

void LineManager::collapseLines(int pos, int num) {

    std::list<CollapsedSection*> nested;
    auto it = sections.begin();
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

    myLineSurface->collapseLines(pos, num);
}

void LineManager::expandLines(int pos, int num)
{
    auto it = sections.begin();
    while (it != sections.end() && (*it)->position != pos) ++it;

    if (it != sections.end())
    {
        CollapsedSection* sec = *it;
        sections.erase(it);

        if (sec->numberOfLines != num) qDebug() << "line numbers don't match!";

        myLineSurface->expandLines(pos, num);
        std::list<CollapsedSection*> nested;
        nested = std::move(sec->nestedSections);

        for (CollapsedSection* nestedSec: nested)
        {
            sections.push_back(nestedSec);
            sections.sort(cmp);
            myLineSurface->collapseLines(nestedSec->position, nestedSec->numberOfLines);
        }

        delete sec;
    }
    else
    {
        qDebug() << "No matching collapse section!";
    }

}

void LineManager::linesAdded(int pos, int num)
{
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

void LineManager::linesRemoved(int pos, int num)
{
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

void LineManager::setLineSurface(LineNumberSurface* lns)
{
    myLineSurface = lns;
}

LineManager::LineManager(QObject *parent)
    : QObject(parent), myLineSurface(nullptr), sections() {

}

LineManager::~LineManager() {}

}
