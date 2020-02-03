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
    unsigned size() { return rows.size(); }

private:
    std::vector<std::u16string> rows;
};

}

#endif // TEXTDOCUMENTDATA_H
