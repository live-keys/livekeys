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

#ifndef TEXTDOCUMENTDATA_H
#define TEXTDOCUMENTDATA_H

#include <vector>
#include <string>
#include "live/lveditorglobal.h"

typedef unsigned short int ushort;

class QTextDocument;

namespace lv {

class LV_EDITOR_EXPORT TextDocumentData
{
public:
    TextDocumentData();
    std::vector<std::pair<unsigned, unsigned>> contentsChange(QTextDocument* document, int position, int removed, int added);
    void clear();
    std::u16string &rowAt(unsigned i);
    unsigned int size() { return static_cast<unsigned int>(rows.size()); }

private:
    std::vector<std::u16string> rows;
};

}

#endif // TEXTDOCUMENTDATA_H
