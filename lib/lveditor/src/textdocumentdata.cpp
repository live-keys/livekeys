#include "textdocumentdata.h"
#include "qtextobject.h"
#include "qtextdocument.h"
#include <QDebug>

namespace lv {

TextDocumentData::TextDocumentData()
{
    // rows.push_back(std::vector<ushort>());
}

std::vector<std::pair<unsigned, unsigned>> TextDocumentData::contentsChange(QTextDocument *document, int position, int removed, int added)
{
    // assumption: we will always have blocks containing "position" and "removed" blocks

    unsigned i = 0;
    unsigned total = 0;

    qDebug() << "velicina dokumenta: " << document->characterCount();

    std::pair<unsigned, unsigned> start, old_end, new_end;

    if (!rows.empty())

    {
        while (i < rows.size())
        {
            auto& row = rows.at(i);
            if (total <= position && position < total + row.size())
            {
                start.first = i;
                start.second = position - total;
                break;
            } else {
                ++i;
                total += row.size();
            }
        }
    } else {
        start.first = 0;
        start.second = 0;

        rows.push_back(std::vector<ushort>());
    }


    if (removed != 0)
    {
        total = 0;
        while (i < rows.size())
        {
            auto& row = rows.at(i);
            if (total <= (start.second + removed) && (start.second + removed) < total + row.size())
            {
                old_end.first = i;
                old_end.second = start.second + removed - total;
                break;
            } else {
                ++i;
                total += row.size();
            }
        }
    } else {
        old_end = start;
    }


    if (added != 0)
    {
        QTextBlock first = document->findBlock(position);
        QTextBlock last = document->findBlock(position + added);

        new_end.first = last.blockNumber();
        new_end.second = position + added - last.position();

    } else {
        new_end = start;
    }

    if (old_end.first != new_end.first)
    {
        int delta = new_end.first - old_end.first;
        if (new_end.first > old_end.first)
        {
            for (int i = 0; i < new_end.first - old_end.first; ++i)
            {
                rows.insert(rows.begin() + start.first, std::vector<ushort>());
            }
        } else {
            for (int i = 0; i < old_end.first - new_end.first; ++i)
            {
                rows.erase(rows.begin() + start.first);
            }
        }
    }

    QTextBlock block = document->findBlockByNumber(start.first);
    int finalBlockNumber = document->lastBlock().blockNumber();
    for (unsigned i = start.first; i <= new_end.first; ++i)
    {
        std::vector<ushort> vec;
        QString text = block.text();
        for (int idx = 0; idx < text.length(); ++idx) vec.push_back(text[idx].unicode());
        if (i != finalBlockNumber) vec.push_back(QChar('\n').unicode());
        rows[i] = vec;
        block = block.next();
    }

    std::vector<std::pair<unsigned, unsigned>> result;
    result.push_back(start);
    result.push_back(old_end);
    result.push_back(new_end);

    return result;
}

void TextDocumentData::clear()
{
    rows.clear();
    // rows.push_back(std::vector<ushort>());
}

std::vector<ushort> &TextDocumentData::rowAt(unsigned i)
{
    return rows[i];
}


} // namespace
