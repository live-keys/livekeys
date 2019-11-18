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

        rows.push_back(std::u16string());
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

        if (last == document->end()) last = document->lastBlock();

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
                rows.insert(rows.begin() + start.first, std::u16string());
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
        std::u16string u16s;
        QString text = block.text();
        for (int idx = 0; idx < text.length(); ++idx) u16s.push_back(text[idx].unicode());
        if (i != finalBlockNumber) u16s.push_back(QChar('\n').unicode());
        rows[i] = u16s;
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

std::u16string &TextDocumentData::rowAt(unsigned i)
{
    return rows[i];
}

const char *TextDocumentData::parsingCallback(void *payload, uint32_t, TSPoint position, uint32_t *bytes_read)
{
    TextDocumentData* textDocumentData = reinterpret_cast<TextDocumentData*>(payload);
    unsigned ushortsize = sizeof(ushort) / sizeof(char);

    if (position.row >= textDocumentData->size())
    {
        *bytes_read = 0;
        return nullptr;
    }
    std::u16string& row = textDocumentData->rowAt(position.row);
    if (position.column >= row.size() * ushortsize)
    {
        *bytes_read = 0;
        return nullptr;
    }

    *bytes_read = row.size()*ushortsize - position.column;
    return reinterpret_cast<const char*>(row.data() + position.column / ushortsize);
}


} // namespace
