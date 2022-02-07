/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#include "textdocumentdata.h"
#include "qtextobject.h"
#include "qtextdocument.h"
#include <QDebug>

namespace lv {

TextDocumentData::TextDocumentData()
{
    // rows.push_back(std::vector<ushort>());
}

std::vector<std::pair<uint32_t, uint32_t>> TextDocumentData::contentsChange(QTextDocument *document, int position, int removed, int added)
{
    // assumption: we will always have blocks containing "position" and "removed" blocks

    size_t i = 0;
    size_t total = 0;

    std::pair<uint32_t, uint32_t> start, old_end, new_end;

    if (!rows.empty())

    {
        while (i < rows.size())
        {
            auto& row = rows.at(i);
            if (total <= static_cast<uint32_t>(position) && static_cast<uint32_t>(position) < total + row.size())
            {
                start.first = static_cast<uint32_t>(i);
                start.second = static_cast<uint32_t>(position) - static_cast<uint32_t>(total);
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
                old_end.first = static_cast<uint32_t>(i);
                old_end.second = start.second + static_cast<uint32_t>(removed) - static_cast<uint32_t>(total);
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
//        QTextBlock first = document->findBlock(position);
        QTextBlock last = document->findBlock(position + added);

        if (last == document->end()) last = document->lastBlock();

        new_end.first = last.blockNumber();
        new_end.second = position + added - last.position();

    } else {
        new_end = start;
    }

    if (old_end.first != new_end.first)
    {
//        int delta = new_end.first - old_end.first;
        if (new_end.first > old_end.first)
        {
            for (uint32_t i = 0; i < new_end.first - old_end.first; ++i)
            {
                rows.insert(rows.begin() + static_cast<size_t>(start.first), std::u16string());
            }
        } else {
            for (uint32_t i = 0; i < old_end.first - new_end.first; ++i)
            {
                rows.erase(rows.begin() + start.first);
            }
        }
    }

    QTextBlock block = document->findBlockByNumber(start.first);
    int finalBlockNumber = document->lastBlock().blockNumber();
    for (uint32_t i = start.first; i <= new_end.first; ++i)
    {
        std::u16string u16s;
        QString text = block.text();
        for (int idx = 0; idx < text.length(); ++idx) u16s.push_back(text[idx].unicode());
        if (i != static_cast<uint32_t>(finalBlockNumber))
            u16s.push_back(QChar('\n').unicode());

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
    return rows.at(static_cast<size_t>(i));
}




} // namespace
